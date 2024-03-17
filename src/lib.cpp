module;
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <ranges>
#include <vector>
#include <iterator>
#include <utility>
#include <format>
#include <stdexcept>
#include <sched.h>
#include <errno.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
export module Lib;
import Elf;
import Util;

using namespace std::ranges;

export void print_elf_header(std::filesystem::path elf)
{
    std::ifstream input{elf, std::ios_base::binary};
    std::vector<unsigned char> file{};
    copy(subrange(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>{}) | views::transform([](const char &c)
                                                                                                              { return std::bit_cast<unsigned char, char>(c); }),
         std::back_inserter(file));

    elf::Elf parsed((owning_view<decltype(file)>(std::move(file))));
    parsed.PrintProgramHeaders();
}

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