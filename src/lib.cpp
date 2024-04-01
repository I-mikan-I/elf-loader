module;
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <ranges>
#include <vector>
#include <tuple>
#include <iterator>
#include <utility>
#include <format>
#include <stdexcept>
#include <sched.h>
#include <errno.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/uio.h>
#include <signal.h>
#include <string.h>
export module Lib;
import Elf;
import Util;

using namespace std::ranges;

export class Loader
{
private:
    static inline auto checked(auto ret)
    {
        if (ret == -1)
        {
            throw std::runtime_error(std::format("Errno: {}", strerror(errno)));
        }
        return ret;
    }
    static inline auto checked(auto ret, auto printable)
    {
        if (ret == -1)
        {
            throw std::runtime_error(std::format("{}: {}", printable, strerror(errno)));
        }
        return ret;
    }

public:
    Loader()
    {
        static void *stack[0];
        auto ret = clone(fn, &stack, CLONE_PARENT_SETTID, nullptr, &child);
        if (ret == -1)
        {
            throw std::runtime_error(std::format("Error code when cloning child: {}", strerror(errno)));
        }
        else if (ret != child)
        {
            throw std::runtime_error("Received wrong child PID");
        }
        int status{};
        checked(ptrace(PTRACE_ATTACH, child));
        checked(waitpid(child, &status, 0), "Error during waitpid");
        if (!WIFSTOPPED(status))
        {
            throw std::runtime_error("Child ptrace not set up correctly");
        }
        debug << "Trace initialized\n";
    }

private:
    pid_t child;

    static int fn(void *)
    {
        checked(ptrace(PTRACE_TRACEME), "Error during ptrace(PTRACE_TRACEME)");
        while (true)
        {
            debug << "loop\n";
        };
        return 1;
    }
};

template <elf::Width W, typename V>
std::vector<std::tuple<iovec, iovec>> getLoadableRegions(const elf::Elf<V> &file)
{
    using namespace std;
    vector<std::tuple<iovec, iovec>> src_dest_ptrs;
    auto headers = file.template GetProgramHeaders<W>();
    debug << "loading headers\n";
    int i = 0;
    for (auto const &h : headers)
    {
        if (h.p_type == elf::PT_LOAD)
        {
            debug << "Loadable Segment: " << i << '\n';
            auto contents = h.ExecutionView(file);
            size_t size = ranges::size(contents);
            debug << "In-File memory contents len: " << hex << "0x" << size << '\n';
            if (size > h.p_memsz)
            {
                throw runtime_error("Program header size too large");
            }
            iovec src_iov = {.iov_base = (void *)(ranges::data(contents)), .iov_len = size};
            iovec dst_iov = {.iov_base = (void *)static_cast<uintptr_t>(h.p_vaddr), .iov_len = size};
            src_dest_ptrs.push_back({src_iov, dst_iov});
        }
        i++;
    }
    return src_dest_ptrs;
}

export void load_elf(std::filesystem::path elf)
{
    std::ifstream input{elf, std::ios_base::binary};
    std::vector<unsigned char> file{};
    copy(subrange(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>{}) | views::transform([](const char &c)
                                                                                                              { return std::bit_cast<unsigned char, char>(c); }),
         std::back_inserter(file));

    elf::Elf parsed((owning_view<decltype(file)>(std::move(file))));
    parsed.PrintProgramHeaders();

    if (parsed.width == elf::X64)
    {
        getLoadableRegions<elf::X64>(parsed);
    }
    else
    {
        getLoadableRegions<elf::X32>(parsed);
    }
}