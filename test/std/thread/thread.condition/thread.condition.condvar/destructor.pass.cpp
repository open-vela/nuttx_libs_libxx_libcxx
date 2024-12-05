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

// ~condition_variable();

#include <condition_variable>
#include <mutex>
#include <thread>
#include <cassert>

#include "make_test_thread.h"
#include "test_macros.h"

static std::condition_variable* cv;
static std::mutex m;
typedef std::unique_lock<std::mutex> Lock;

static bool f_ready = false;
static bool g_ready = false;

static void f()
{
    Lock lk(m);
    f_ready = true;
    cv->notify_one();
    delete cv;
}

static void g()
{
    Lock lk(m);
    g_ready = true;
    cv->notify_one();
    while (!f_ready)
        cv->wait(lk);
}

extern "C" int main(int, char**)
{
    cv = new std::condition_variable;
    std::thread th2 = support::make_test_thread(g);
    Lock lk(m);
    while (!g_ready)
        cv->wait(lk);
    lk.unlock();
    std::thread th1 = support::make_test_thread(f);
    th1.join();
    th2.join();

  return 0;
}
