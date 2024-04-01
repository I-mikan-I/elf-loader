# Linux User-Space Elf loader

Parses System-V Elf files and loads AMD64 and X86 executables into memory.
The loader does not run in kernel mode and utilizes ptrace to create a new task
for the executable.

Currently Work-In-Progress.

## Features

- [x] Parse Elf Files
- [ ] Load Elf Files (In Development)
- [ ] Load dynamic libraries (Planned)

## Building

The [.devcontainer](./.devcontainer/devcontainer.json) offers a quick-and-easy build system,
including clang-17, CMake, and Ninja.

Prerequisites:

- Standard c++20 Toolchain
- CMake 3.28 or later

Setup:

- Update [CMakeLists.txt](./CMakeLists.txt) for your compiler.

Example:
```sh
mkdir build
cd build
cmake .. -GNinja
ninja
```
