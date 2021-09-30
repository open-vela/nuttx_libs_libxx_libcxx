set(CMAKE_BUILD_TYPE MinSizeRel CACHE STRING "")
set(CMAKE_POSITION_INDEPENDENT_CODE OFF CACHE BOOL "")

set(LIBCXX_USE_COMPILER_RT ON CACHE BOOL "")
set(LIBCXX_ENABLE_ASSERTIONS OFF CACHE BOOL "")
set(LIBCXX_ABI_VERSION "1" CACHE STRING "")
set(LIBCXX_ENABLE_EXPERIMENTAL_LIBRARY OFF CACHE BOOL "")
set(LIBCXX_ENABLE_STATIC ON CACHE BOOL "")
set(LIBCXX_ENABLE_SHARED ON CACHE BOOL "")
set(LIBCXX_CXX_ABI libcxxabi CACHE STRING "")
set(LIBCXX_HIDE_FROM_ABI_PER_TU_BY_DEFAULT ON CACHE BOOL "")
set(LIBCXX_ENABLE_DEBUG_MODE_SUPPORT OFF CACHE BOOL "")
set(LIBCXX_ENABLE_VENDOR_AVAILABILITY_ANNOTATIONS ON CACHE BOOL "")
set(LIBCXX_ENABLE_INCOMPLETE_FEATURES OFF CACHE BOOL "")

set(LIBCXX_HERMETIC_STATIC_LIBRARY ON CACHE BOOL "")
set(LIBCXXABI_HERMETIC_STATIC_LIBRARY ON CACHE BOOL "")

set(LIBCXXABI_ENABLE_ASSERTIONS OFF CACHE BOOL "")
set(LIBCXXABI_ENABLE_FORGIVING_DYNAMIC_CAST ON CACHE BOOL "")
