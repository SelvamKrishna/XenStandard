#pragma once

#ifndef XEN_ERR_CTX
#define XEN_ERR_CTX

#include "err/err.hpp"
#include "str/str.hpp"

namespace xen {

/// @class `err_ctx`
/// @brief A class that contains verbose info on the thrown err.
/// TODO: xen::f_str support
class err_ctx {
public:
	const err TYPE {err::Logic};
	const str DESC;

	[[nodiscard]] constexpr err_ctx() noexcept = delete;

	[[nodiscard]] err_ctx(err type, const char* desc) noexcept : TYPE{type}, DESC{desc} {}

	#ifdef _OSTREAM_
	friend std::ostream& operator<<(std::ostream& os, const err_ctx& err_ctx) noexcept {
		os << "[ERR]: " << static_cast<u8_t>(err_ctx.TYPE) << ": " << err_ctx.DESC.c_str() << std::endl;
		return os;
	}
	#endif /// _OSTREAM_
};

} /// namespace xen

#endif /// XEN_ERR_CTX