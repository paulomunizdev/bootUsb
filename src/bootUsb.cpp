/*
 * Title:       bootUsb
 * Version:     1.0.0
 * Author:      Paulo Muniz
 * GitHub:      https://github.com/paulomunizdev/bootUsb
 * Description: bootUsb is a C++ tool for creating bootable USB drives from ISO files.
 */
 #include <iostream>
 #include <cstdlib>
 #include <string>
 #include <vector>
 #include <stdexcept>

 // Function to list available disks directly from the system using fdisk
 /*
  * @brief                  Function to list the available disks directly from the system using fdisk, along with their sizes.
  * @return std::vector<std::pair<std::string, std::string>>  A vector containing the names and sizes of available disks.
  */
 std::vector<std::pair<std::string, std::string>> listDisksFromSystem() {
     std::vector<std::pair<std::string, std::string>> disks;
     std::string command = "fdisk -l | grep 'Disk /dev/' | grep -v '/dev/loop' | grep -v '/dev/ram' | cut -d' ' -f2 | cut -d':' -f1";
     FILE* stream = popen(command.c_str(), "r");
     if (stream) {
         const int bufferSize = 256;
         char buffer[bufferSize];
         while (!feof(stream) && fgets(buffer, bufferSize, stream) != NULL) {
             std::string diskName(buffer);
             diskName.pop_back(); // Remove the trailing newline character
             std::string sizeCommand = "fdisk -l " + diskName + " | grep Disk | cut -d' ' -f3,4";
             FILE* sizeStream = popen(sizeCommand.c_str(), "r");
             if (sizeStream) {
                 char sizeBuffer[bufferSize];
                 fgets(sizeBuffer, bufferSize, sizeStream);
                 std::string diskSize(sizeBuffer);
                 diskSize.pop_back(); // Remove the trailing newline character
                 disks.push_back(std::make_pair(diskName, diskSize));
                 pclose(sizeStream);
             } else {
                 throw std::runtime_error("Failed to execute fdisk command for disk size");
             }
         }
         pclose(stream);
     } else {
         throw std::runtime_error("Failed to execute fdisk command");
     }
     return disks;
 }

 // Function to choose a disk
 /*
  * @brief                  Function to allow the user to choose a disk from the list of available disks.
  * @param disks            A vector containing the names and sizes of available disks.
  * @return std::string     The chosen disk name.
  */
 std::string chooseDisk(const std::vector<std::pair<std::string, std::string>>& disks) {
     std::cout << "Available disks:" << std::endl;
     for (size_t i = 0; i < disks.size(); ++i) {
         std::cout << i + 1 << ". " << disks[i].first << " (" << disks[i].second << ")" << std::endl;
     }
     while (true) {
         std::cout << "Choose a disk (1, 2, ...): ";
         int choice;
         std::cin >> choice;
         if (choice >= 1 && choice <= static_cast<int>(disks.size())) {
             return disks[choice - 1].first;
         } else {
             std::cout << "Invalid choice. Please enter a valid number." << std::endl;
         }
     }
 }

 // Function to choose an ISO file
 /*
  * @brief                  Function to allow the user to input the path to an ISO file.
  * @return std::string     The path to the chosen ISO file.
  */
 std::string chooseISO() {
     std::string isoPath;
     std::cout << "Enter the path to the ISO file: ";
     std::cin >> isoPath;
     return isoPath;
 }

 // Function to format the disk before creating a bootable USB drive
/*
 * @brief              Function to format the disk before creating a bootable USB drive.
 * @param disk         The name of the disk to be formatted.
 * @param fileSystem   The file system to be used for formatting (e.g., ext4, fat32).
 * @return void
 */
void formatDisk(const std::string& disk, const std::string& fileSystem) {
    try {
        // Unmount the disk if it is mounted
        std::string umountCommand = "sudo umount " + disk + "*";
        int umountResult = system(umountCommand.c_str());

        // Clear all signatures and partitions from the disk
        std::string clearCommand = "sudo wipefs --all --force " + disk;
        int clearResult = system(clearCommand.c_str());
        if (clearResult != 0) {
            throw std::runtime_error("Error clearing disk signatures and partitions.");
        }

        // Create a new partition table (msdos)
        std::string partedCommand = "sudo parted -s " + disk + " mklabel msdos";
        int partedResult = system(partedCommand.c_str());
        if (partedResult != 0) {
            throw std::runtime_error("Error creating a new partition table.");
        }

        // Create a new primary partition spanning the whole disk
        std::string mkpartCommand = "sudo parted -s " + disk + " mkpart primary 0% 100%";
        int mkpartResult = system(mkpartCommand.c_str());
        if (mkpartResult != 0) {
            throw std::runtime_error("Error creating a new partition.");
        }

        // Format the partition with the specified file system
        std::string formatCommand;
        if (fileSystem == "fat32") {
            formatCommand = "sudo mkfs.vfat -n bootUsb " + disk + "1";
        } else if (fileSystem == "ext4") {
            formatCommand = "sudo mkfs.ext4 -L bootUsb " + disk + "1";
        } else {
            throw std::invalid_argument("Unsupported file system.");
        }
        int formatResult = system(formatCommand.c_str());
        if (formatResult != 0) {
            throw std::runtime_error("Error formatting the disk.");
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}


 // Function to create a bootable USB drive using dd
 /*
  * @brief                      Function to create a bootable USB drive using the dd command.
  * @param disk                 The name of the disk to be used for creating the bootable drive.
  * @param isoPath              The path to the ISO file to be used for creating the bootable drive.
  * @return void
  */
 void createBootableUSB(const std::string& disk, const std::string& isoPath) {
     try {
         std::string command = "sudo dd if=" + isoPath + " of=" + disk + " status=progress bs=4M";
         int result = system(command.c_str());
         if (result == 0) {
             std::cout << "Bootable USB drive created successfully!" << std::endl;
         } else {
             throw std::runtime_error("Error creating the bootable USB drive.");
         }
     } catch (const std::exception& e) {
         std::cerr << e.what() << std::endl;
     }
 }

 int main() {
     try {
         std::vector<std::pair<std::string, std::string>> disks = listDisksFromSystem();
         std::string chosenDisk = chooseDisk(disks);
         std::string chosenISO = chooseISO();
         formatDisk(chosenDisk, "vfat"); // Format the disk as FAT32 before creating the bootable USB drive
         createBootableUSB(chosenDisk, chosenISO);
     } catch (const std::exception& e) {
         std::cerr << e.what() << std::endl;
     }
     return 0;
 }
