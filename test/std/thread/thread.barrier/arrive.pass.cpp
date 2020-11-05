//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// UNSUPPORTED: libcpp-has-no-threads
// UNSUPPORTED: c++03, c++11

// This test requires the dylib support introduced in D68480,
// which hasn't shipped yet.
// XFAIL: with_system_cxx_lib=macosx
// XFAIL: with_system_cxx_lib=macosx10.15
// XFAIL: with_system_cxx_lib=macosx10.14
// XFAIL: with_system_cxx_lib=macosx10.13
// XFAIL: with_system_cxx_lib=macosx10.12
// XFAIL: with_system_cxx_lib=macosx10.11
// XFAIL: with_system_cxx_lib=macosx10.10
// XFAIL: with_system_cxx_lib=macosx10.9

// <barrier>

#include <barrier>
#include <thread>

#include "test_macros.h"

int main(int, char**)
{
  std::barrier<> b(2);

  auto tok = b.arrive();
  std::thread t([&](){
    (void)b.arrive();
  });
  b.wait(std::move(tok));
  t.join();

  auto tok2 = b.arrive(2);
  b.wait(std::move(tok2));
  return 0;
}
