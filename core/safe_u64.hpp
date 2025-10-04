#pragma once

#ifndef XEN_SAFE_U64
#define XEN_SAFE_U64

#include <type_traits>

#include "core/numdef.hpp"
#include "err/err.hpp"

namespace xen {

/// @class `safe_u64`
/// @brief A safe wrapper around unsigned 64-bit integers (u64).
/// @section Features:
/// - Performs arithmetic operations (+, -, *, /, +=, -=, *=, /=, ++, --) with bounds checking.
/// - Reports errors via the `err` enumeration:
/// --> `err::NumOverflow`  : Operation result exceeds maximum `u64_t` capacity.
/// --> `err::NumUnderflow` : Operation result goes below zero (not representable by `u64_t`).
/// --> `err::DivideByZero` : Division operation where the divisor is zero.
/// - Can be compared like a regular integer (`==`, `<`, `>`, `!=`, `>=`, `<=`)
class safe_u64 {
private:
	u64_t _size{0};

#pragma region // Arithmetic helpers

	/// @returns `true` if the addition doesn't overflow `U64_MAX`.
	[[nodiscard]] static constexpr bool _is_safe_add(u64_t a, u64_t b) noexcept {
		return a <= (U64_MAX - b);
	}

	/// @returns `true` if the subtraction doesn't underflow `U64_MIN`
	[[nodiscard]] static constexpr bool _is_safe_sub(u64_t a, u64_t b) noexcept {
		return a >= b;
	}

	/// @returns `true` if the multiplication doesn't overflow `U64_MAX`
	[[nodiscard]] static constexpr bool _is_safe_mul(u64_t a, u64_t b) noexcept {
		if (b == 0) return true;
		return a <= (U64_MAX / b);
	}

	/// @returns `true` if the divisior is not `0`
	[[nodiscard]] static constexpr bool _is_safe_div(u64_t, u64_t b) noexcept {
		return b != 0;
	}

#pragma endregion // Arithmetic helpers

public:
#pragma region /// Constructors

	[[nodiscard]] constexpr safe_u64() noexcept = default;

	template <typename T_>
		requires std::is_integral_v<T_>
	[[nodiscard]] constexpr safe_u64(T_ val) noexcept {
		if constexpr (std::is_signed_v<T_>) {
			if (val < 0) {
				_size = 0;
				return;
			}
		}

		_size = static_cast<u64_t>(val);
	}

	[[nodiscard]] constexpr operator u64_t() const noexcept { return _size; }

#pragma endregion /// Constructors
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

	template <typename T_>
		requires std::is_integral_v<T_>
	friend constexpr safe_u64& operator+=(safe_u64& lhs, T_ rhs) {
		if constexpr (std::is_signed_v<T_>) {
			if (rhs < 0) {
				u64_t sub = static_cast<u64_t>(-static_cast<i64_t>(rhs));
				if (!_is_safe_sub(lhs._size, sub)) throw err::NumUnderflow;

				lhs._size -= sub;
				return lhs;
			}
		}

		u64_t add = static_cast<u64_t>(rhs);
		if (!_is_safe_add(lhs._size, add)) throw err::NumOverflow;

		lhs._size += add;
		return lhs;
	}

	template <typename T_>
		requires std::is_integral_v<T_>
	friend constexpr safe_u64 operator+(safe_u64 lhs, T_ rhs) {
		lhs += rhs; 
		return lhs;
	}

	template<typename T_>
		requires std::is_integral_v<T_>
	friend constexpr safe_u64 operator+(T_ lhs, safe_u64 rhs) {
		rhs += lhs; 
		return rhs;
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

	template<typename T_>
		requires std::is_integral_v<T_>
	friend constexpr safe_u64& operator-=(safe_u64& lhs, T_ rhs) {
		if constexpr (std::is_signed_v<T_>) {
			if (rhs < 0) {
				u64_t add = static_cast<u64_t>(-static_cast<i64_t>(rhs));
				if (!_is_safe_add(lhs._size, add)) throw err::NumOverflow;
				lhs._size += add;
				return lhs;
			}
		}

		u64_t sub = static_cast<u64_t>(rhs);
		if (!_is_safe_sub(lhs._size, sub)) throw err::NumUnderflow;
		lhs._size -= sub;
		return lhs;
	}

	template<typename T_>
		requires std::is_integral_v<T_>
	friend constexpr safe_u64 operator-(safe_u64 lhs, T_ rhs) {
		lhs -= rhs;
		return lhs;
	}
	
	template<typename T_>
		requires std::is_integral_v<T_>
	friend constexpr safe_u64 operator-(T_ lhs, safe_u64 rhs) {
		safe_u64 tmp{lhs};
		tmp -= rhs._size;
		return tmp;
	}

#pragma endregion /// (-) operation
#pragma region /// (*) operation

	template<typename T_>
		requires std::is_integral_v<T_>
	friend constexpr safe_u64& operator*=(safe_u64& lhs, T_ rhs) {
		if constexpr (std::is_signed_v<T_>) {
			if (rhs < 0) throw err::NumUnderflow;
		}

		u64_t mul = static_cast<u64_t>(rhs);
		if (!_is_safe_mul(lhs._size, mul)) throw err::NumOverflow;

		lhs._size *= mul;
		return lhs;
	}

	template<typename T_>
		requires std::is_integral_v<T_>
	friend constexpr safe_u64 operator*(safe_u64 lhs, T_ rhs) {
		lhs *= rhs;
		return lhs;
	}

	template<typename T_>
		requires std::is_integral_v<T_>
	friend constexpr safe_u64 operator*(T_ lhs, safe_u64 rhs) {
		rhs *= lhs;
		return rhs;
	}

#pragma endregion /// (*) operation
#pragma region /// (/) operation

	template<typename T_>
		requires std::is_integral_v<T_>
	friend constexpr safe_u64& operator/=(safe_u64& lhs, T_ rhs) {
		if constexpr (std::is_signed_v<T_>) {
			if (rhs <= 0) throw err::DivideByZero;
		}

		u64_t div = static_cast<u64_t>(rhs);
		if (!_is_safe_div(lhs._size, div)) throw err::DivideByZero;

		lhs._size /= div;
		return lhs;
	}

	template<typename T_>
		requires std::is_integral_v<T_>
	friend constexpr safe_u64 operator/(safe_u64 lhs, T_ rhs) {
		lhs /= rhs;
		return lhs;
	}

	template<typename T_>
		requires std::is_integral_v<T_>
	friend constexpr safe_u64 operator/(T_ lhs, safe_u64 rhs) {
		if (rhs._size == 0) throw err::DivideByZero;
		return safe_u64{static_cast<u64_t>(lhs) / rhs._size};
	}

#pragma endregion /// (/) operation
#pragma region /// Comparison overload

	#define XEN_SAFE_U64_COMPARISON_OP(op) \
		friend constexpr bool operator op(const safe_u64& lhs, const safe_u64& rhs) noexcept { return lhs._size op rhs._size; } \
		template<typename T_> requires std::is_integral_v<T_> \
		friend constexpr bool operator op(const safe_u64& lhs, T_ rhs) noexcept { return lhs._size op static_cast<u64_t>(rhs); } \
		template<typename T_> requires std::is_integral_v<T_> \
		friend constexpr bool operator op(T_ lhs, const safe_u64& rhs) noexcept { return static_cast<u64_t>(lhs) op rhs._size; } \

	XEN_SAFE_U64_COMPARISON_OP(==)
	XEN_SAFE_U64_COMPARISON_OP(!=)
	XEN_SAFE_U64_COMPARISON_OP(<)
	XEN_SAFE_U64_COMPARISON_OP(>)
	XEN_SAFE_U64_COMPARISON_OP(<=)
	XEN_SAFE_U64_COMPARISON_OP(>=)

	#undef XEN_SAFE_U64_COMPARISON_OP_T

#pragma endregion /// Comparison overload
};

typedef safe_u64 u_size;

} /// namespace xen

#endif /// XEN_SAFE_U64