//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <unordered_map>

// Increment local_iterator past end.

// UNSUPPORTED: libcxx-no-debug-mode
// UNSUPPORTED: c++03

// ADDITIONAL_COMPILE_FLAGS: -D_LIBCPP_DEBUG=1
#define _LIBCPP_ASSERT(x, m) ((x) ? (void)0 : std::exit(0))

#include <unordered_map>
#include <cassert>
#include <functional>
#include <string>

#include "test_macros.h"
#include "min_allocator.h"

int main(int, char**) {
    typedef std::unordered_map<int, std::string, std::hash<int>, std::equal_to<int>,
                        min_allocator<std::pair<const int, std::string>>> C;
    C c({{42, std::string()}});
    C::size_type b = c.bucket(42);
    C::local_iterator i = c.begin(b);
    assert(i != c.end(b));
    ++i;
    assert(i == c.end(b));
    ++i;
    assert(false);

    return 0;
}
