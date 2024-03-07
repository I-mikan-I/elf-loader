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

add_compile_options(-I/nix/store/0p66fy5qygglhxckxshx15vvh68zm0lv-clang-17.0.6-lib/lib/clang/17/include)
if(CMAKE_EXPORT_COMPILE_COMMANDS)
  set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES 
      ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

add_library(elf)
target_sources(elf
  PUBLIC
    FILE_SET CXX_MODULES FILES
      src/elf/lib.cpp)

add_library(lib)
target_sources(lib
  PUBLIC
    FILE_SET CXX_MODULES FILES
      src/lib.cpp
)
target_link_libraries(lib PRIVATE elf)

add_executable(ElfLoader src/main.cpp)

target_link_libraries(ElfLoader PRIVATE lib)