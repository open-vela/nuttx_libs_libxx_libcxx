// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP___COMPARE_COMPARE_THREE_WAY_H
#define _LIBCPP___COMPARE_COMPARE_THREE_WAY_H

#include <__config>
#include <__compare/three_way_comparable.h>
#include <__utility/forward.h>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_BEGIN_NAMESPACE_STD

#if _LIBCPP_STD_VER > 17 && !defined(_LIBCPP_HAS_NO_CONCEPTS)

struct _LIBCPP_TEMPLATE_VIS compare_three_way
{
    template<class _T1, class _T2>
        requires three_way_comparable_with<_T1, _T2>
    constexpr _LIBCPP_HIDE_FROM_ABI
    auto operator()(_T1&& __t, _T2&& __u) const
        noexcept(noexcept(_VSTD::forward<_T1>(__t) <=> _VSTD::forward<_T2>(__u)))
        { return          _VSTD::forward<_T1>(__t) <=> _VSTD::forward<_T2>(__u); }

    using is_transparent = void;
};

#endif // _LIBCPP_STD_VER > 17 && !defined(_LIBCPP_HAS_NO_CONCEPTS)

_LIBCPP_END_NAMESPACE_STD

#endif // _LIBCPP___COMPARE_COMPARE_THREE_WAY_H
