//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: no-exceptions
// <deque>

// void push_back(const value_type& x);

#include <deque>
#include "test_macros.h"
#include "test_allocator.h"
#include <cassert>

// Flag that makes the copy constructor for DequeCMyClass throw an exception
static bool gCopyConstructorShouldThow = false;

class DequeCMyClass {
    public: DequeCMyClass(int tag);
    public: DequeCMyClass(const DequeCMyClass& iOther);
    public: ~DequeCMyClass();

    bool equal(const DequeCMyClass &rhs) const
        { return fTag == rhs.fTag && fMagicValue == rhs.fMagicValue; }

    private:
        int fMagicValue;
        int fTag;

    private: static int kStartedConstructionMagicValue;
    private: static int kFinishedConstructionMagicValue;
};

// Value for fMagicValue when the constructor has started running, but not yet finished
int DequeCMyClass::kStartedConstructionMagicValue = 0;
// Value for fMagicValue when the constructor has finished running
int DequeCMyClass::kFinishedConstructionMagicValue = 12345;

DequeCMyClass::DequeCMyClass(int tag) :
    fMagicValue(kStartedConstructionMagicValue), fTag(tag)
{
    // Signal that the constructor has finished running
    fMagicValue = kFinishedConstructionMagicValue;
}

DequeCMyClass::DequeCMyClass(const DequeCMyClass& iOther) :
    fMagicValue(kStartedConstructionMagicValue), fTag(iOther.fTag)
{
    // If requested, throw an exception _before_ setting fMagicValue to kFinishedConstructionMagicValue
    if (gCopyConstructorShouldThow) {
        throw std::exception();
    }
    // Signal that the constructor has finished running
    fMagicValue = kFinishedConstructionMagicValue;
}

DequeCMyClass::~DequeCMyClass() {
    // Only instances for which the constructor has finished running should be destructed
    assert(fMagicValue == kFinishedConstructionMagicValue);
}

bool operator==(const DequeCMyClass &lhs, const DequeCMyClass &rhs) { return lhs.equal(rhs); }

extern "C" int main(int, char**)
{
    DequeCMyClass instance(42);
    {
    std::deque<DequeCMyClass> vec;

    vec.push_back(instance);
    std::deque<DequeCMyClass> vec2(vec);

    gCopyConstructorShouldThow = true;
    try {
        vec.push_back(instance);
        assert(false);
    }
    catch (...) {
        gCopyConstructorShouldThow = false;
        assert(vec==vec2);
    }
    }

    {
    test_allocator_statistics alloc_stats;
    typedef std::deque<DequeCMyClass, test_allocator<DequeCMyClass> > C;
    C vec((test_allocator<DequeCMyClass>(&alloc_stats)));
    C vec2(vec, test_allocator<DequeCMyClass>(&alloc_stats));

    alloc_stats.throw_after = 1;
    try {
        vec.push_back(instance);
        assert(false);
    }
    catch (...) {
        assert(vec==vec2);
    }
    }

  return 0;
}
