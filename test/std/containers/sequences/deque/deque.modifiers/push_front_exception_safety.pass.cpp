//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: no-exceptions
// <deque>

// void push_front(const value_type& x);

#include <deque>
#include <cassert>
#include "test_macros.h"
#include "test_allocator.h"

// Flag that makes the copy constructor for DqCMyClass throw an exception
static bool gCopyConstructorShouldThow = false;


class DqCMyClass {
    public: DqCMyClass(int tag);
    public: DqCMyClass(const DqCMyClass& iOther);
    public: ~DqCMyClass();

    bool equal(const DqCMyClass &rhs) const
        { return fTag == rhs.fTag && fMagicValue == rhs.fMagicValue; }
    private:
        int fMagicValue;
        int fTag;

    private: static int kStartedConstructionMagicValue;
    private: static int kFinishedConstructionMagicValue;
};

// Value for fMagicValue when the constructor has started running, but not yet finished
int DqCMyClass::kStartedConstructionMagicValue = 0;
// Value for fMagicValue when the constructor has finished running
int DqCMyClass::kFinishedConstructionMagicValue = 12345;

DqCMyClass::DqCMyClass(int tag) :
    fMagicValue(kStartedConstructionMagicValue), fTag(tag)
{
    // Signal that the constructor has finished running
    fMagicValue = kFinishedConstructionMagicValue;
}

DqCMyClass::DqCMyClass(const DqCMyClass& iOther) :
    fMagicValue(kStartedConstructionMagicValue), fTag(iOther.fTag)
{
    // If requested, throw an exception _before_ setting fMagicValue to kFinishedConstructionMagicValue
    if (gCopyConstructorShouldThow) {
        throw std::exception();
    }
    // Signal that the constructor has finished running
    fMagicValue = kFinishedConstructionMagicValue;
}

DqCMyClass::~DqCMyClass() {
    // Only instances for which the constructor has finished running should be destructed
    assert(fMagicValue == kFinishedConstructionMagicValue);
}

bool operator==(const DqCMyClass &lhs, const DqCMyClass &rhs) { return lhs.equal(rhs); }

extern "C" int main(int, char**)
{
    DqCMyClass instance(42);
    {
    std::deque<DqCMyClass> vec;

    vec.push_front(instance);
    std::deque<DqCMyClass> vec2(vec);

    gCopyConstructorShouldThow = true;
    try {
        vec.push_front(instance);
        assert(false);
    }
    catch (...) {
        gCopyConstructorShouldThow = false;
        assert(vec==vec2);
    }
    }

    {
    test_allocator_statistics alloc_stats;
    typedef std::deque<DqCMyClass, test_allocator<DqCMyClass> > C;
    C vec((test_allocator<DqCMyClass>(&alloc_stats)));
    C vec2(vec, test_allocator<DqCMyClass>(&alloc_stats));

    alloc_stats.throw_after = 1;
    try {
        vec.push_front(instance);
        assert(false);
    }
    catch (...) {
        assert(vec==vec2);
    }
    }

  return 0;
}
