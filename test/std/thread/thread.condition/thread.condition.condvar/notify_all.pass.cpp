//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// UNSUPPORTED: no-threads

// <condition_variable>

// class condition_variable;

// void notify_all();

#include <condition_variable>
#include <mutex>
#include <thread>
#include <cassert>

#include "make_test_thread.h"
#include "test_macros.h"

static std::condition_variable cv;
static std::mutex mut;

static int test0 = 0;
static int test1 = 0;
static int test2 = 0;

static void f1()
{
    std::unique_lock<std::mutex> lk(mut);
    assert(test1 == 0);
    while (test1 == 0)
        cv.wait(lk);
    assert(test1 == 1);
    test1 = 2;
}

static void f2()
{
    std::unique_lock<std::mutex> lk(mut);
    assert(test2 == 0);
    while (test2 == 0)
        cv.wait(lk);
    assert(test2 == 1);
    test2 = 2;
}

extern "C" int main(int, char**)
{
    std::thread t1 = support::make_test_thread(f1);
    std::thread t2 = support::make_test_thread(f2);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    {
        std::unique_lock<std::mutex>lk(mut);
        test1 = 1;
        test2 = 1;
    }
    cv.notify_all();
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::unique_lock<std::mutex>lk(mut);
    }
    t1.join();
    t2.join();
    assert(test1 == 2);
    assert(test2 == 2);

  return 0;
}
