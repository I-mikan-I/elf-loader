module;
#include <cstdint>
#include <ranges>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <utility>
#include <stdexcept>
export module Elf;
import :Header;
using namespace std;

namespace elf
{
    export template <ranges::view V>
    class Elf
    {
    public:
        Elf(V &&_file) : file{std::forward<V>(_file)}
        {
            unsigned char e_ident[EI_NIDENT];
            if (ranges::size(file) < EI_NIDENT)
            {
                throw std::invalid_argument("File too small");
            }
            ranges::copy_n(file.begin(), EI_NIDENT, e_ident);
            if (!ranges::equal(e_ident | views::take(4), (unsigned char[]){0x7f, 'E', 'L', 'F'}))
            {
                throw std::invalid_argument("ELF header invalid");
            }
            if (e_ident[4] == 1)
            {
                Initialize32();
            }
            else
            {
                Initialize64();
            }
        }

    private:
        V file;
        Width width;
        union
        {
            header_t<X32> x32;
            header_t<X64> x64;
        } header;
        void Initialize32()
        {
            if (ranges::size(file) < header.x32.size)
            {
                throw std::invalid_argument("File too small");
            }
            ranges::copy_n(file.begin(), header.x32.size, reinterpret_cast<unsigned char *>(&(this->header.x32)));
            std::cout << header.x32;
        }
        void Initialize64()
        {
            if (ranges::size(file) < header.x64.size)
            {
                throw std::invalid_argument("File too small");
            }
            ranges::copy_n(file.begin(), header.x64.size, reinterpret_cast<unsigned char *>(&(this->header.x64)));
            std::cout << header.x64;
        }
    };

}