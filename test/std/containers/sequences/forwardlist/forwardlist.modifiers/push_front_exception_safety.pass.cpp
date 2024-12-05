//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: no-exceptions
// <forward_list>

// void push_front(const value_type& x);

#include <forward_list>
#include <cassert>
#include <exception>

#include "test_macros.h"

// Flag that makes the copy constructor for ForwardCMyClass throw an exception
static bool gCopyConstructorShouldThow = false;


class ForwardCMyClass {
    public: ForwardCMyClass();
    public: ForwardCMyClass(const ForwardCMyClass& iOther);
    public: ~ForwardCMyClass();

    private: int fMagicValue;

    private: static int kStartedConstructionMagicValue;
    private: static int kFinishedConstructionMagicValue;
};

// Value for fMagicValue when the constructor has started running, but not yet finished
int ForwardCMyClass::kStartedConstructionMagicValue = 0;
// Value for fMagicValue when the constructor has finished running
int ForwardCMyClass::kFinishedConstructionMagicValue = 12345;

ForwardCMyClass::ForwardCMyClass() :
    fMagicValue(kStartedConstructionMagicValue)
{
    // Signal that the constructor has finished running
    fMagicValue = kFinishedConstructionMagicValue;
}

ForwardCMyClass::ForwardCMyClass(const ForwardCMyClass& /*iOther*/) :
    fMagicValue(kStartedConstructionMagicValue)
{
    // If requested, throw an exception _before_ setting fMagicValue to kFinishedConstructionMagicValue
    if (gCopyConstructorShouldThow) {
        throw std::exception();
    }
    // Signal that the constructor has finished running
    fMagicValue = kFinishedConstructionMagicValue;
}

ForwardCMyClass::~ForwardCMyClass() {
    // Only instances for which the constructor has finished running should be destructed
    assert(fMagicValue == kFinishedConstructionMagicValue);
}

extern "C" int main(int, char**)
{
    ForwardCMyClass instance;
    std::forward_list<ForwardCMyClass> vec;

    vec.push_front(instance);

    gCopyConstructorShouldThow = true;
    try {
        vec.push_front(instance);
    }
    catch (...) {
    }

  return 0;
}
