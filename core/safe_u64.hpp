#pragma once

#ifndef XEN_SAFE_U64
#define XEN_SAFE_U64

#include "core/numdef.hpp"
#include "err/err.hpp"

namespace xen {

/// @class safe_u64
/// @brief A safe wrapper around unsigned 64-bit integers (u64).
/// @section Features:
/// - Wraps a raw `u64_t` value.
/// - Can perform arithmetic operations (+, -, *, /).
/// - Performs bounds checking on arithmetic operations
/// - Reports errors via the `err` enumeration:
/// --> `err::NumOverflow`  : Operation result exceeds maximum `u64_t` capacity.
/// --> `err::NumUnderflow` : Operation result goes below zero (not representable by `u64_t`).
/// --> `err::DivideByZero` : Division operation where the divisor is zero.
/// - Supports implicit/explicit conversion to `u64_t`.
/// - Can be compared like a regular integer (`==`, `<`, `>`, `!=`, `>=`, `<=`)
///
/// UNTESTED:
class safe_u64 {
private:
	u64_t _size{0};

#pragma region // Arithmetic helpers

	/// @returns `true` if the addition doesn't overflow `U64_MAX`.
	[[nodiscard]] static constexpr bool _is_safe_add_u64(u64_t a, u64_t b) noexcept {
		return a <= (U64_MAX - b);
	}

	/// @returns `true` if the subtraction doesn't underflow `U64_MIN`
	[[nodiscard]] static constexpr bool _is_safe_sub_u64(u64_t a, u64_t b) noexcept {
		return a >= b;
	}

	/// @returns `true` if the multiplication doesn't overflow `U64_MAX`
	[[nodiscard]] static constexpr bool _is_safe_mul_u64(u64_t a, u64_t b) noexcept {
		if (b == 0) return true;
		return a <= (U64_MAX / b);
	}

	/// @returns `true` if the divisior is not `0`
	[[nodiscard]] static constexpr bool _is_safe_div_u64(u64_t, u64_t b) noexcept {
		return b != 0;
	}

#pragma endregion // Arithmetic helpers

public:
	[[nodiscard]] constexpr safe_u64() noexcept = default;

	[[nodiscard]] constexpr safe_u64(u64_t val) noexcept : _size{val} {}

	[[nodiscard]] constexpr safe_u64(i32_t val) noexcept
	: _size{static_cast<u64_t>(val < 0 ? 0u : static_cast<u64_t>(val))} {}

	[[nodiscard]] constexpr operator u64_t() const noexcept { return _size; }

#pragma region /// (+) operation

	friend constexpr safe_u64& operator++(safe_u64& self) {
		if (self._size == U64_MAX) throw err::NumOverflow;
		++self._size;
		return self;
	}

	friend constexpr safe_u64 operator++(safe_u64& self, int) {
		safe_u64 tmp{self};
		++self;
		return tmp;
	}

	friend constexpr safe_u64& operator+=(safe_u64& lhs, u64_t rhs) {
		if (!_is_safe_add_u64(lhs._size, rhs)) throw err::NumOverflow;
		lhs._size += rhs;
		return lhs;
	}

	friend constexpr safe_u64 operator+(safe_u64 lhs, u64_t rhs) {
		lhs += rhs;
		return lhs;
	}

	friend constexpr safe_u64 operator+(u64_t lhs, const safe_u64& rhs) {
		return safe_u64{lhs} + rhs._size;
	}

	friend constexpr safe_u64 operator+(safe_u64 lhs, const safe_u64& rhs) {
		lhs += rhs._size;
		return lhs;
	}

	friend constexpr safe_u64& operator+=(safe_u64& lhs, i32_t rhs) {
		if (rhs < 0) {
			u64_t sub{static_cast<u64_t>(-static_cast<i64_t>(rhs))};
			if (!_is_safe_sub_u64(lhs._size, sub)) throw err::NumUnderflow;
			lhs._size -= sub;
		} else {
			u64_t add{static_cast<u64_t>(rhs)};
			if (!_is_safe_add_u64(lhs._size, add)) throw err::NumOverflow;
			lhs._size += add;
		}

		return lhs;
	}

	friend constexpr safe_u64 operator+(safe_u64 lhs, i32_t rhs) {
		lhs += rhs;
		return lhs;
	}

	friend constexpr safe_u64 operator+(i32_t lhs, const safe_u64& rhs) {
		safe_u64 tmp{rhs};
		tmp += lhs;
		return tmp;
	}

#pragma endregion /// (+) operation

#pragma region /// (-) operation

	friend constexpr safe_u64& operator--(safe_u64& self) {
		if (self._size == U64_MIN) throw err::NumUnderflow;
		--self._size;
		return self;
	}

	friend constexpr safe_u64 operator--(safe_u64& self, int) {
		safe_u64 tmp{self};
		--self;
		return tmp;
	}

	friend constexpr safe_u64& operator-=(safe_u64& lhs, u64_t rhs) {
		if (!_is_safe_sub_u64(lhs._size, rhs)) throw err::NumUnderflow;
		lhs._size -= rhs;
		return lhs;
	}

	friend constexpr safe_u64 operator-(safe_u64 lhs, u64_t rhs) {
		lhs -= rhs;
		return lhs;
	}

	friend constexpr safe_u64 operator-(u64_t lhs, const safe_u64& rhs) {
		if (!_is_safe_sub_u64(lhs, rhs._size)) throw err::NumUnderflow;
		return safe_u64{lhs - rhs._size};
	}

	friend constexpr safe_u64& operator-=(safe_u64& lhs, i32_t rhs) {
		if (rhs < 0) {
			u64_t add{static_cast<u64_t>(-static_cast<i64_t>(rhs))};
			if (!_is_safe_add_u64(lhs._size, add)) throw err::NumOverflow;
			lhs._size += add;
		} else {
			u64_t sub{static_cast<u64_t>(rhs)};
			if (!_is_safe_sub_u64(lhs._size, sub)) throw err::NumUnderflow;
			lhs._size -= sub;
		}

		return lhs;
	}

	friend constexpr safe_u64 operator-(safe_u64 lhs, i32_t rhs) {
		lhs -= rhs;
		return lhs;
	}

	friend constexpr safe_u64 operator-(i32_t lhs, const safe_u64& rhs) {
		if (lhs < 0) throw err::NumUnderflow;
		u64_t left{static_cast<u64_t>(lhs)};
		if (!_is_safe_sub_u64(left, rhs._size)) throw err::NumUnderflow;
		return safe_u64{left - rhs._size};
	}

#pragma endregion /// (-) operation

#pragma region /// (*) operation

	friend constexpr safe_u64& operator*=(safe_u64& lhs, u64_t rhs) {
		if (!_is_safe_mul_u64(lhs._size, rhs)) throw err::NumOverflow;
		lhs._size *= rhs;
		return lhs;
	}

	friend constexpr safe_u64 operator*(safe_u64 lhs, u64_t rhs) {
		lhs *= rhs;
		return lhs;
	}

	friend constexpr safe_u64 operator*(u64_t lhs, const safe_u64& rhs) {
		safe_u64 tmp{lhs};
		tmp *= rhs._size;
		return tmp;
	}

	friend constexpr safe_u64& operator*=(safe_u64& lhs, i32_t rhs) {
		if (rhs < 0) throw err::NumOverflow;
		return (lhs *= static_cast<u64_t>(rhs));
	}

	friend constexpr safe_u64 operator*(safe_u64 lhs, i32_t rhs) {
		lhs *= rhs;
		return lhs;
	}

	friend constexpr safe_u64 operator*(i32_t lhs, const safe_u64& rhs) {
		if (lhs < 0) throw err::NumOverflow;
		return rhs * static_cast<u64_t>(lhs);
	}

#pragma endregion /// (*) operation

#pragma region /// (/) operation

	friend constexpr safe_u64& operator/=(safe_u64& lhs, u64_t rhs) {
		if (rhs == 0) throw err::DivideByZero;
		lhs._size /= rhs;
		return lhs;
	}

	friend constexpr safe_u64 operator/(safe_u64 lhs, u64_t rhs) {
		lhs /= rhs;
		return lhs;
	}

	friend constexpr safe_u64 operator/(u64_t lhs, const safe_u64& rhs) {
		if (rhs._size == 0) throw err::DivideByZero;
		return safe_u64{lhs / rhs._size};
	}

#pragma endregion /// (/) operation

#pragma region /// Comparison overload

	friend constexpr bool operator==(const safe_u64& a, const safe_u64& b) noexcept { return a._size == b._size; }
	friend constexpr bool operator!=(const safe_u64& a, const safe_u64& b) noexcept { return a._size != b._size; }
	friend constexpr bool operator<(const safe_u64& a, const safe_u64& b) noexcept { return a._size < b._size; }
	friend constexpr bool operator>(const safe_u64& a, const safe_u64& b) noexcept { return a._size > b._size; }
	friend constexpr bool operator<=(const safe_u64& a, const safe_u64& b) noexcept { return a._size <= b._size; }
	friend constexpr bool operator>=(const safe_u64& a, const safe_u64& b) noexcept { return a._size >= b._size; }

	friend constexpr bool operator==(const safe_u64& a, const i32_t& b) noexcept { return a._size == b; }
	friend constexpr bool operator!=(const safe_u64& a, const i32_t& b) noexcept { return a._size != b; }
	friend constexpr bool operator<(const safe_u64& a, const i32_t& b) noexcept { return a._size < b; }
	friend constexpr bool operator>(const safe_u64& a, const i32_t& b) noexcept { return a._size > b; }
	friend constexpr bool operator<=(const safe_u64& a, const i32_t& b) noexcept { return a._size <= b; }
	friend constexpr bool operator>=(const safe_u64& a, const i32_t& b) noexcept { return a._size >= b; }

	friend constexpr bool operator==(const i32_t& a, const safe_u64& b) noexcept { return a == b._size; }
	friend constexpr bool operator!=(const i32_t& a, const safe_u64& b) noexcept { return a != b._size; }
	friend constexpr bool operator<(const i32_t& a, const safe_u64& b) noexcept { return a < b._size; }
	friend constexpr bool operator>(const i32_t& a, const safe_u64& b) noexcept { return a > b._size; }
	friend constexpr bool operator<=(const i32_t& a, const safe_u64& b) noexcept { return a <= b._size; }
	friend constexpr bool operator>=(const i32_t& a, const safe_u64& b) noexcept { return a >= b._size; }

#pragma endregion /// Comparison overload
};

typedef safe_u64 s_size;

} /// namespace xen

#endif /// XEN_SAFE_U64