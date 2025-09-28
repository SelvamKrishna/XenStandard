#pragma once

#ifndef XEN_ERR_CTX
#define XEN_ERR_CTX

#include "core/numdef.hpp"
#include "err/err.hpp"
#include "str/str.hpp"

#include <iostream>
#include <cstdlib>
#include <ostream>

namespace xen {
/// @class err_ctx
/// @brief A class that contains verbose info on the thrown err.
/// @details Info such as Type and Description of the error is stored.
///
/// TODO: xen::f_str support
/// UNTESTED:
class err_ctx {
public:
	const err TYPE {err::Logic};
	const str DESC;

	[[nodiscard]] constexpr err_ctx() noexcept = delete;

	[[nodiscard]] constexpr err_ctx(err type, const char* desc) noexcept : TYPE{type}, DESC{desc} {}

	friend std::ostream& operator<<(std::ostream& os, const err_ctx& err_ctx) noexcept {
		os << "[ERR]: " << static_cast<u8_t>(err_ctx.TYPE) << ": " << err_ctx.DESC << std::endl;
		return os;
	}

	/// @details Logs the error message and terminates the program.
	[[noreturn]] void terminate() const noexcept {
		std::cerr << this;
		std::abort();
	}
};

} /// namespace xen

#endif /// XEN_ERR_CTX