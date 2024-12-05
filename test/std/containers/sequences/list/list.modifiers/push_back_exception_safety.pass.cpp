//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: no-exceptions
// <list>

// void push_back(const value_type& x);

#include <list>
#include <cassert>
#include <exception>

#include "test_macros.h"

// Flag that makes the copy constructor for BackCMyClass throw an exception
static bool gCopyConstructorShouldThow = false;


class BackCMyClass {
    public: BackCMyClass();
    public: BackCMyClass(const BackCMyClass& iOther);
    public: ~BackCMyClass();

    private: int fMagicValue;

    private: static int kStartedConstructionMagicValue;
    private: static int kFinishedConstructionMagicValue;
};

// Value for fMagicValue when the constructor has started running, but not yet finished
int BackCMyClass::kStartedConstructionMagicValue = 0;
// Value for fMagicValue when the constructor has finished running
int BackCMyClass::kFinishedConstructionMagicValue = 12345;

BackCMyClass::BackCMyClass() :
    fMagicValue(kStartedConstructionMagicValue)
{
    // Signal that the constructor has finished running
    fMagicValue = kFinishedConstructionMagicValue;
}

BackCMyClass::BackCMyClass(const BackCMyClass& /*iOther*/) :
    fMagicValue(kStartedConstructionMagicValue)
{
    // If requested, throw an exception _before_ setting fMagicValue to kFinishedConstructionMagicValue
    if (gCopyConstructorShouldThow) {
        throw std::exception();
    }
    // Signal that the constructor has finished running
    fMagicValue = kFinishedConstructionMagicValue;
}

BackCMyClass::~BackCMyClass() {
    // Only instances for which the constructor has finished running should be destructed
    assert(fMagicValue == kFinishedConstructionMagicValue);
}

extern "C" int main(int, char**)
{
    BackCMyClass instance;
    std::list<BackCMyClass> vec;

    vec.push_back(instance);

    gCopyConstructorShouldThow = true;
    try {
        vec.push_back(instance);
    }
    catch (...) {
    }

  return 0;
}
