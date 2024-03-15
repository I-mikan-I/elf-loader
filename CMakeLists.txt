set(CMAKE_CXX_COMPILER clang++-17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_COMPILER clang-17)
set(CMAKE_CXX_SCAN_FOR_MODULES)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
cmake_minimum_required(VERSION 3.28)
project(
  CPPElfLoader
  VERSION 1.0
  LANGUAGES CXX)

add_compile_options(-Wall -fdiagnostics-color=always)

add_library(util)
target_sources(util
  PUBLIC
    FILE_SET CXX_MODULES FILES
      src/util/lib.cpp)
link_libraries(util)

add_library(elf)
target_sources(elf
  PUBLIC
    FILE_SET CXX_MODULES FILES
      src/elf/lib.cpp src/elf/header.cpp)

add_library(lib)
target_sources(lib
  PUBLIC
    FILE_SET CXX_MODULES FILES
      src/lib.cpp
)
target_link_libraries(lib PRIVATE elf)

add_executable(ElfLoader src/main.cpp)

target_link_libraries(ElfLoader PRIVATE lib)