module;
#include <iostream>
export module Util;

#ifndef LOGLEVEL
/*
1 = info
2 = debug
*/
#define LOGLEVEL 1
#endif
#define __STRINGIFY(X) #X
#define STRINGIFY(X) __STRINGIFY(X)

class IgnoreOut
{
public:
    IgnoreOut()
    {
    }
    inline const IgnoreOut &operator<<(const auto &v) const noexcept
    {
        return *this;
    }
};

static IgnoreOut ignore_out = IgnoreOut();

template <int LEVEL>
consteval static auto &getDebug()
{
    if constexpr (LEVEL == 1)
    {
        return ignore_out;
    }
    else if constexpr (LEVEL == 2)
    {
        return std::cout;
    }
    else
    {
        static_assert(false);
    }
}

export auto &info = std::cout;
export auto &debug = getDebug<LOGLEVEL>();