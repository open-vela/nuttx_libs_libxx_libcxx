//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: no-exceptions
// <list>

// void push_front(const value_type& x);

#include <list>
#include <cassert>
#include <exception>

#include "test_macros.h"

// Flag that makes the copy constructor for PushCMyClass throw an exception
static bool gCopyConstructorShouldThow = false;


class PushCMyClass {
    public: PushCMyClass();
    public: PushCMyClass(const PushCMyClass& iOther);
    public: ~PushCMyClass();

    private: int fMagicValue;

    private: static int kStartedConstructionMagicValue;
    private: static int kFinishedConstructionMagicValue;
};

// Value for fMagicValue when the constructor has started running, but not yet finished
int PushCMyClass::kStartedConstructionMagicValue = 0;
// Value for fMagicValue when the constructor has finished running
int PushCMyClass::kFinishedConstructionMagicValue = 12345;

PushCMyClass::PushCMyClass() :
    fMagicValue(kStartedConstructionMagicValue)
{
    // Signal that the constructor has finished running
    fMagicValue = kFinishedConstructionMagicValue;
}

PushCMyClass::PushCMyClass(const PushCMyClass& /*iOther*/) :
    fMagicValue(kStartedConstructionMagicValue)
{
    // If requested, throw an exception _before_ setting fMagicValue to kFinishedConstructionMagicValue
    if (gCopyConstructorShouldThow) {
        throw std::exception();
    }
    // Signal that the constructor has finished running
    fMagicValue = kFinishedConstructionMagicValue;
}

PushCMyClass::~PushCMyClass() {
    // Only instances for which the constructor has finished running should be destructed
    assert(fMagicValue == kFinishedConstructionMagicValue);
}

extern "C" int main(int, char**)
{
    PushCMyClass instance;
    std::list<PushCMyClass> vec;

    vec.push_front(instance);

    gCopyConstructorShouldThow = true;
    try {
        vec.push_front(instance);
    }
    catch (...) {
    }

  return 0;
}
