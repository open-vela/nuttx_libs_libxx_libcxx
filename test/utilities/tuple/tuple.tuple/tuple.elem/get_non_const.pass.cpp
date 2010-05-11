//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <tuple>

// template <class... Types> class tuple;

// template <size_t I, class... Types>
//   typename tuple_element<I, tuple<Types...> >::type&
//   get(tuple<Types...>& t);

#include <tuple>
#include <string>
#include <cassert>

int main()
{
    {
        typedef std::tuple<int> T;
        T t(3);
        assert(std::get<0>(t) == 3);
        std::get<0>(t) = 2;
        assert(std::get<0>(t) == 2);
    }
    {
        typedef std::tuple<std::string, int> T;
        T t("high", 5);
        assert(std::get<0>(t) == "high");
        assert(std::get<1>(t) == 5);
        std::get<0>(t) = "four";
        std::get<1>(t) = 4;
        assert(std::get<0>(t) == "four");
        assert(std::get<1>(t) == 4);
    }
    {
        typedef std::tuple<double&, std::string, int> T;
        double d = 1.5;
        T t(d, "high", 5);
        assert(std::get<0>(t) == 1.5);
        assert(std::get<1>(t) == "high");
        assert(std::get<2>(t) == 5);
        std::get<0>(t) = 2.5;
        std::get<1>(t) = "four";
        std::get<2>(t) = 4;
        assert(std::get<0>(t) == 2.5);
        assert(std::get<1>(t) == "four");
        assert(std::get<2>(t) == 4);
        assert(d == 2.5);
    }
}
