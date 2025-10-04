#pragma once

/// @copyright Copyright (c) [Selvam Krishna]. OPEN-SOURCE
/// @link https://github.com/SelvamKrishna @endlink

/// @warning If you are using `std` library include them before including `xen`

#ifndef XEN_CONFIG
#define XEN_CONFIG

#include "core/numdef.hpp"

#define XEN_VER_MAJOR 0
#define XEN_VER_MINOR 3

namespace xen {

inline constexpr u64_t VER_MAJOR = XEN_VER_MAJOR;
inline constexpr u64_t VER_MINOR = XEN_VER_MINOR;

} /// namespace xen

#endif /// XEN_CONFIG