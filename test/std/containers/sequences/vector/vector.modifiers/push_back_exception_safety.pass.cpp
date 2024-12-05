//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <vector>

// void push_back(const value_type& x);

#include <vector>
#include <cassert>

#include "asan_testing.h"
#include "test_macros.h"

// Flag that makes the copy constructor for VCMyClass throw an exception
static bool gCopyConstructorShouldThrow = false;

class VCMyClass {
    public: VCMyClass(int tag);
    public: VCMyClass(const VCMyClass& iOther);
    public: ~VCMyClass();

    bool equal(const VCMyClass &rhs) const
        { return fTag == rhs.fTag && fMagicValue == rhs.fMagicValue; }
    private:
        int fMagicValue;
        int fTag;

    private: static int kStartedConstructionMagicValue;
    private: static int kFinishedConstructionMagicValue;
};

// Value for fMagicValue when the constructor has started running, but not yet finished
int VCMyClass::kStartedConstructionMagicValue = 0;
// Value for fMagicValue when the constructor has finished running
int VCMyClass::kFinishedConstructionMagicValue = 12345;

VCMyClass::VCMyClass(int tag) :
    fMagicValue(kStartedConstructionMagicValue), fTag(tag)
{
    // Signal that the constructor has finished running
    fMagicValue = kFinishedConstructionMagicValue;
}

VCMyClass::VCMyClass(const VCMyClass& iOther) :
    fMagicValue(kStartedConstructionMagicValue), fTag(iOther.fTag)
{
    // If requested, throw an exception _before_ setting fMagicValue to kFinishedConstructionMagicValue
    if (gCopyConstructorShouldThrow) {
        TEST_THROW(std::exception());
    }
    // Signal that the constructor has finished running
    fMagicValue = kFinishedConstructionMagicValue;
}

VCMyClass::~VCMyClass() {
    // Only instances for which the constructor has finished running should be destructed
    assert(fMagicValue == kFinishedConstructionMagicValue);
}

bool operator==(const VCMyClass &lhs, const VCMyClass &rhs) { return lhs.equal(rhs); }

extern "C" int main(int, char**)
{
    VCMyClass instance(42);
    std::vector<VCMyClass> vec;

    vec.push_back(instance);
    std::vector<VCMyClass> vec2(vec);
    assert(is_contiguous_container_asan_correct(vec));
    assert(is_contiguous_container_asan_correct(vec2));

#ifndef TEST_HAS_NO_EXCEPTIONS
    gCopyConstructorShouldThrow = true;
    try {
        vec.push_back(instance);
        assert(false);
    }
    catch (...) {
        assert(vec==vec2);
        assert(is_contiguous_container_asan_correct(vec));
    }
#endif

  return 0;
}
