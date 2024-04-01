//===------------------------- thread.cpp----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include <__config>

#ifndef _LIBCPP_HAS_NO_THREADS

#include <__thread/poll_with_backoff.h>
#include <__thread/timed_backoff_policy.h>
#include <exception>
#include <future>
#include <limits>
#include <thread>
#include <vector>

#if __has_include(<unistd.h>)
# include <unistd.h> // for sysconf
#endif

#if defined(__NetBSD__)
#pragma weak pthread_create // Do not create libpthread dependency
#endif

#if defined(__NuttX__) && !defined(BUILD_KERNEL)
#include <nuttx/tls.h>
#endif

#if defined(_LIBCPP_WIN32API)
#include <windows.h>
#endif

#if defined(__ELF__) && defined(_LIBCPP_LINK_PTHREAD_LIB)
#pragma comment(lib, "pthread")
#endif

_LIBCPP_BEGIN_NAMESPACE_STD

thread::~thread()
{
    if (!__libcpp_thread_isnull(&__t_))
        terminate();
}

void
thread::join()
{
    int ec = EINVAL;
    if (!__libcpp_thread_isnull(&__t_))
    {
        ec = __libcpp_thread_join(&__t_);
        if (ec == 0)
            __t_ = _LIBCPP_NULL_THREAD;
    }

    if (ec)
        __throw_system_error(ec, "thread::join failed");
}

void
thread::detach()
{
    int ec = EINVAL;
    if (!__libcpp_thread_isnull(&__t_))
    {
        ec = __libcpp_thread_detach(&__t_);
        if (ec == 0)
            __t_ = _LIBCPP_NULL_THREAD;
    }

    if (ec)
        __throw_system_error(ec, "thread::detach failed");
}

unsigned
thread::hardware_concurrency() noexcept
{
#if defined(_SC_NPROCESSORS_ONLN)
    long result = sysconf(_SC_NPROCESSORS_ONLN);
    // sysconf returns -1 if the name is invalid, the option does not exist or
    // does not have a definite limit.
    // if sysconf returns some other negative number, we have no idea
    // what is going on. Default to something safe.
    if (result < 0)
        return 0;
    return static_cast<unsigned>(result);
#elif defined(_LIBCPP_WIN32API)
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return info.dwNumberOfProcessors;
#else  // defined(CTL_HW) && defined(HW_NCPU)
    // TODO: grovel through /proc or check cpuid on x86 and similar
    // instructions on other architectures.
#   if defined(_LIBCPP_WARNING)
        _LIBCPP_WARNING("hardware_concurrency not yet implemented")
#   else
#       warning hardware_concurrency not yet implemented
#   endif
    return 0;  // Means not computable [thread.thread.static]
#endif // defined(CTL_HW) && defined(HW_NCPU)
}

namespace this_thread
{

void
sleep_for(const chrono::nanoseconds& ns)
{
    if (ns > chrono::nanoseconds::zero())
    {
        __libcpp_thread_sleep_for(ns);
    }
}

}  // this_thread

#if defined(__NuttX__) && !defined(BUILD_KERNEL)
#if !defined(CONFIG_TLS_TASK_NELEM) || CONFIG_TLS_TASK_NELEM == 0
#error "Thread.cpp needs to enable config of CONFIG_TLS_TASK_NELEM," \
       " and CONFIG_TLS_TASK_NELEM > 0, please enable it"
#endif
static void __free_thread_local_data(void* data)
{
    __thread_specific_ptr<__thread_struct>* __p = static_cast<__thread_specific_ptr<__thread_struct>*>(data);
    delete __p;
}

__thread_specific_ptr<__thread_struct>&
__thread_local_data()
{
    __thread_specific_ptr<__thread_struct>* __p = NULL;
    static int __index = -1;

    if (__index < 0)
        __index = task_tls_alloc(__free_thread_local_data);

    if (__index >= 0)
    {
        __p = reinterpret_cast<__thread_specific_ptr<__thread_struct>*>(task_tls_get_value(__index));
        if (__p == NULL)
        {
            __p = new __thread_specific_ptr<__thread_struct>();
            if (__p)
                task_tls_set_value(__index, reinterpret_cast<uintptr_t>(__p));
        }
    }
    return *__p;
}
#else
__thread_specific_ptr<__thread_struct>&
__thread_local_data()
{
  // Even though __thread_specific_ptr's destructor doesn't actually destroy
  // anything (see comments there), we can't call it at all because threads may
  // outlive the static variable and calling its destructor means accessing an
  // object outside of its lifetime, which is UB.
  alignas(__thread_specific_ptr<__thread_struct>) static char __b[sizeof(__thread_specific_ptr<__thread_struct>)];
  static __thread_specific_ptr<__thread_struct>* __p = new (__b) __thread_specific_ptr<__thread_struct>();
  return *__p;
}
#endif

// __thread_struct_imp

template <class T>
class _LIBCPP_HIDDEN __hidden_allocator
{
public:
    typedef T  value_type;

    T* allocate(size_t __n)
        {return static_cast<T*>(::operator new(__n * sizeof(T)));}
    void deallocate(T* __p, size_t) {::operator delete(static_cast<void*>(__p));}

    size_t max_size() const {return size_t(~0) / sizeof(T);}
};

class _LIBCPP_HIDDEN __thread_struct_imp
{
    typedef vector<__assoc_sub_state*,
                          __hidden_allocator<__assoc_sub_state*> > _AsyncStates;
    typedef vector<pair<condition_variable*, mutex*>,
               __hidden_allocator<pair<condition_variable*, mutex*> > > _Notify;

    _AsyncStates async_states_;
    _Notify notify_;

    __thread_struct_imp(const __thread_struct_imp&);
    __thread_struct_imp& operator=(const __thread_struct_imp&);
public:
    __thread_struct_imp() {}
    ~__thread_struct_imp();

    void notify_all_at_thread_exit(condition_variable* cv, mutex* m);
    void __make_ready_at_thread_exit(__assoc_sub_state* __s);
};

__thread_struct_imp::~__thread_struct_imp()
{
    for (_Notify::iterator i = notify_.begin(), e = notify_.end();
            i != e; ++i)
    {
        i->first->notify_all();
        i->second->unlock();
    }
    for (_AsyncStates::iterator i = async_states_.begin(), e = async_states_.end();
            i != e; ++i)
    {
        (*i)->__make_ready();
        (*i)->__release_shared();
    }
}

void
__thread_struct_imp::notify_all_at_thread_exit(condition_variable* cv, mutex* m)
{
    notify_.push_back(pair<condition_variable*, mutex*>(cv, m));
}

void
__thread_struct_imp::__make_ready_at_thread_exit(__assoc_sub_state* __s)
{
    async_states_.push_back(__s);
    __s->__add_shared();
}

// __thread_struct

__thread_struct::__thread_struct()
    : __p_(new __thread_struct_imp)
{
}

__thread_struct::~__thread_struct()
{
    delete __p_;
}

void
__thread_struct::notify_all_at_thread_exit(condition_variable* cv, mutex* m)
{
    __p_->notify_all_at_thread_exit(cv, m);
}

void
__thread_struct::__make_ready_at_thread_exit(__assoc_sub_state* __s)
{
    __p_->__make_ready_at_thread_exit(__s);
}

_LIBCPP_END_NAMESPACE_STD

#endif // !_LIBCPP_HAS_NO_THREADS
