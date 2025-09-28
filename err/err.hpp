#pragma once

#ifndef XEN_ERR
#define XEN_ERR

#include "core/numdef.hpp"

namespace xen {

/// @enum err
/// @brief Represents different categories of runtime errors.
/// @section Types:
/// - Logic          : The program logic does not match the expected logic.
/// - IndexOutOfRange: Attempt to access outside the bounds of an array or container.
/// - InvalidArgument: A function received an unsupported or invalid argument.
/// - NumOverflow    : A numeric value exceeded the maximum representable capacity.
/// - NumUnderflow   : A numeric value went below the minimum representable capacity.
/// - DivideByZero   : Trying to divide numeric by 0.
enum class err: u8_t {
	Logic,
	IndexOutOfRange,
	InvalidArgument,
	NumOverflow,
	NumUnderflow,
	DivideByZero,
};

} /// namespace xen

#endif /// XEN_ERR