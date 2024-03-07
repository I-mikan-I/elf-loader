module;
#include <filesystem>
#include <iostream>
#include <fstream>
#include <ranges>
#include <vector>
export module Lib;
import Elf;

using namespace std::ranges;

export void print_elf_header(std::filesystem::path elf) {
    std::ifstream input{elf};
    unsigned char headerBuf[HEADER_SIZE]{};
    input.read(reinterpret_cast<char*>(headerBuf), HEADER_SIZE);

    ElfHeader header(subrange(headerBuf, headerBuf + HEADER_SIZE));
}