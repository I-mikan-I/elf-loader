module;
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <ranges>
#include <vector>
#include <iterator>
#include <utility>
export module Lib;
import Elf;

using namespace std::ranges;

export void print_elf_header(std::filesystem::path elf)
{
    std::ifstream input{elf, std::ios_base::binary};
    std::vector<unsigned char> file{};
    copy(subrange(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>{}) | views::transform([](const char &c)
                                                                                                              { return std::bit_cast<unsigned char, char>(c); }),
         std::back_inserter(file));

    elf::Elf parsed((owning_view<decltype(file)>(std::move(file))));
}
