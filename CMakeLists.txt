set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_COMPILER clang)
cmake_minimum_required(VERSION 3.28)

project(
  CPPElfLoader
  VERSION 1.0
  LANGUAGES CXX)

add_library(lib)
target_sources(lib
  PUBLIC
    FILE_SET CXX_MODULES FILES
      src/lib.cpp
)

add_executable(ElfLoader src/main.cpp)

target_link_libraries(ElfLoader PRIVATE lib)