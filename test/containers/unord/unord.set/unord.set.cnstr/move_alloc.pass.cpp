//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <unordered_set>

// template <class Value, class Hash = hash<Value>, class Pred = equal_to<Value>,
//           class Alloc = allocator<Value>>
// class unordered_set

// unordered_set(unordered_set&& u, const allocator_type& a);

#include <unordered_set>
#include <cassert>

#include "../../../test_compare.h"
#include "../../../test_hash.h"
#include "../../../test_allocator.h"

int main()
{
#ifdef _LIBCPP_MOVE
    {
        typedef int P;
        typedef test_allocator<int> A;
        typedef std::unordered_set<int,
                                   test_hash<std::hash<int> >,
                                   test_compare<std::equal_to<int> >,
                                   A
                                   > C;
        P a[] =
        {
            P(1),
            P(2),
            P(3),
            P(4),
            P(1),
            P(2)
        };
        C c0(a, a + sizeof(a)/sizeof(a[0]),
            7,
            test_hash<std::hash<int> >(8),
            test_compare<std::equal_to<int> >(9),
            A(10)
           );
        C c(std::move(c0), A(12));
        assert(c.bucket_count() >= 5);
        assert(c.size() == 4);
        assert(c.count(1) == 1);
        assert(c.count(2) == 1);
        assert(c.count(3) == 1);
        assert(c.count(4) == 1);
        assert(c.hash_function() == test_hash<std::hash<int> >(8));
        assert(c.key_eq() == test_compare<std::equal_to<int> >(9));
        assert(c.get_allocator() == A(12));
        assert(!c.empty());
        assert(std::distance(c.begin(), c.end()) == c.size());
        assert(std::distance(c.cbegin(), c.cend()) == c.size());
        assert(c.load_factor() == (float)c.size()/c.bucket_count());
        assert(c.max_load_factor() == 1);

        assert(c0.empty());
    }
    {
        typedef int P;
        typedef test_allocator<int> A;
        typedef std::unordered_set<int,
                                   test_hash<std::hash<int> >,
                                   test_compare<std::equal_to<int> >,
                                   A
                                   > C;
        P a[] =
        {
            P(1),
            P(2),
            P(3),
            P(4),
            P(1),
            P(2)
        };
        C c0(a, a + sizeof(a)/sizeof(a[0]),
            7,
            test_hash<std::hash<int> >(8),
            test_compare<std::equal_to<int> >(9),
            A(10)
           );
        C c(std::move(c0), A(10));
        assert(c.bucket_count() >= 5);
        assert(c.size() == 4);
        assert(c.count(1) == 1);
        assert(c.count(2) == 1);
        assert(c.count(3) == 1);
        assert(c.count(4) == 1);
        assert(c.hash_function() == test_hash<std::hash<int> >(8));
        assert(c.key_eq() == test_compare<std::equal_to<int> >(9));
        assert(c.get_allocator() == A(10));
        assert(!c.empty());
        assert(std::distance(c.begin(), c.end()) == c.size());
        assert(std::distance(c.cbegin(), c.cend()) == c.size());
        assert(c.load_factor() == (float)c.size()/c.bucket_count());
        assert(c.max_load_factor() == 1);

        assert(c0.empty());
    }
#endif
}
