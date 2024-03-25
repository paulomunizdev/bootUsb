# bootUsb 0.1.2

## Introduction
bootUsb is a C++ tool for creating bootable USB drives from ISO files. It lists available disks on the system, allowing the user to choose one for creating the bootable drive, and then uses the `dd` command to perform the operation.

Here's how you can adapt the "How to Use" section for your `bootUsb` tool:

## How to Use

### Downloading the Compiled Tool

1. Clone the repository:
   ```bash
   git clone https://github.com/paulomunizdev/bootUsb.git
   ```
2. Navigate to the bootUsb directory:
   ```bash
   cd bootUsb
   ```
3. Give permission to execute:
   ```bash
   sudo chmod +x bootUsb
   ```   
3. Run bootUsb:
   ```bash
   sudo ./bootUsb
   ```
### Compiling the Source Code

Before compiling, ensure that you have a C++ compiler installed on your system.

Compile the source code using g++:
```bash
g++ -o bootUsb src/bootUsb.cpp
```
This will generate an executable named `bootUsb` in the root directory of the project.

## License
This project is licensed under the MIT License
