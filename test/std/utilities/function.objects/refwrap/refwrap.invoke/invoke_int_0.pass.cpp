//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <functional>

// reference_wrapper

// template <class... ArgTypes>
//   requires Callable<T, ArgTypes&&...>
//   Callable<T, ArgTypes&&...>::result_type
//   operator()(ArgTypes&&... args) const;

#include <functional>
#include <cassert>

#include "test_macros.h"

// 0 args, return int

static int count = 0;

static int f_int_0()
{
    return 3;
}

struct A_int_0
{
    int operator()() {return 4;}
};

static void
test_int_0()
{
    // function
    {
    std::reference_wrapper<int ()> r1(f_int_0);
    assert(r1() == 3);
    }
    // function pointer
    {
    int (*fp)() = f_int_0;
    std::reference_wrapper<int (*)()> r1(fp);
    assert(r1() == 3);
    }
    // functor
    {
    A_int_0 a0;
    std::reference_wrapper<A_int_0> r1(a0);
    assert(r1() == 4);
    }
}

// 1 arg, return void

static void f_void_1(int i)
{
    count += i;
}

struct A_void_1
{
    void operator()(int i)
    {
        count += i;
    }
};

extern "C" int main(int, char**)
{
    test_int_0();

  return 0;
}
