#pragma once

#ifndef XEN_OBSERVED_REF
#define XEN_OBSERVED_REF

#include <utility>

#include "core/safe_u64.hpp"

namespace xen {

/// @struct `ref_counter_t`
/// @brief
/// A tool to help keep track of the number of strong and weak references of a
/// heap allocated data.
/// @warning
// `ref_counter_t` is only a tool to keep track of the references, All underlying logic must be implemented:
/// - A reference which holds shared ownership to a data is a strong reference
/// - A reference which simply points to a data without owning it is a weak reference
/// - As long as there are strong references the data must live
/// - As long as there are weak references tehe `ref_counter_t` must live
struct ref_counter_t {
private:
	u_size _strong_ref_count{1};
	u_size _weak_ref_count{0};

public:
#pragma region /// Constructor

	[[nodiscard]] explicit constexpr ref_counter_t() noexcept = default;

#pragma endregion /// Constructor
#pragma region /// Getters

	/// @returns The number of strong references
	[[nodiscard]] constexpr u_size get_strong_count() const noexcept { return _strong_ref_count; }

	/// @returns The number of weak references
	[[nodiscard]] constexpr u_size get_weak_count() const noexcept { return _weak_ref_count; }

	/// @returns The number of references altogether
	[[nodiscard]] constexpr u_size get_total_count() const noexcept { return _strong_ref_count + _weak_ref_count; }

	/// @returns `true` if there are no strong references
	[[nodiscard]] constexpr bool has_no_strong_ref() const noexcept { return _strong_ref_count == 0; }

	/// @returns `true` if there are no weak references
	[[nodiscard]] constexpr bool has_no_weak_ref() const noexcept { return _weak_ref_count == 0; }

	/// @returns `true` if there are no references altogether
	[[nodiscard]] constexpr bool has_no_reference() const noexcept { return get_total_count() == 0; }

#pragma endregion /// Getters
#pragma region /// Setters

	/// @details Adds a strong reference
	constexpr void inc_strong_ref() { ++_strong_ref_count; }

	/// @details Removes a strong reference
	constexpr void dec_strong_ref() { --_strong_ref_count; }

	/// @details Adds a weak reference
	constexpr void inc_weak_ref() { ++_weak_ref_count; }

	/// @details Removes a weak reference
	constexpr void dec_weak_ref() { --_weak_ref_count; }

#pragma endregion /// Setters
};

/// @class `observed_ref`
/// @brief A strong reference to a heap allocated data
/// @section Features:
/// - Imlements RAII principles
/// - Properly handles data lifetime and reference counting
/// - Underlying data can be safely copied and moved
/// - Operator overloaded to behave just like a raw pointer
/// @note `observed_ref` is a replica of `shared_ref` but with `weak_ref` support
///s
/// TODO: `weak_ref` support
template <typename T_>
class observed_ref {
private:
	T_* _ptr{nullptr};
	ref_counter_t* _ref_counter{nullptr};

#pragma region /// Helpers

	/// @details Increments the `_strong_ref_count`
	/// @warning `_ptr` must be properly initialized before calling fn
	constexpr void _add_owner() noexcept {
		if (_ptr != nullptr && _ref_counter != nullptr) [[likely]] _ref_counter->inc_strong_ref();
	}

	/// @details
	/// Decrements the `_strong_ref_count`, destroys `_ptr` if no strong references,
	/// destroys `_ref_counter` if no strong and weak references
	constexpr void _remove_owner() noexcept {
		if (_ptr == nullptr || _ref_counter == nullptr) [[unlikely]] return;

		_ref_counter->dec_strong_ref();

		if (_ref_counter->has_no_strong_ref()) [[unlikely]] delete _ptr;
		if (_ref_counter->has_no_reference()) [[unlikely]] delete _ref_counter;

		_ptr = nullptr;
		_ref_counter = nullptr;
	}

#pragma endregion /// Helpers

public:
#pragma region /// Constructors & Destructors

	[[nodiscard]] constexpr explicit observed_ref() noexcept = default;

	[[nodiscard]] constexpr explicit observed_ref(T_* ptr) noexcept
	: _ptr{ptr}
	, _ref_counter{ptr != nullptr ? new ref_counter_t{} : nullptr}
	{}

	constexpr ~observed_ref() noexcept { _remove_owner(); }

#pragma endregion /// Constructors & Destructors
#pragma region /// Copy semantics

	[[nodiscard]] constexpr observed_ref(const observed_ref& other) noexcept
	: _ptr{other._ptr}, _ref_counter{other._ref_counter} { _add_owner(); }

	constexpr observed_ref& operator=(const observed_ref& other) noexcept {
		if (this != &other) {
            _remove_owner();
            _ptr = other._ptr;
            _ref_counter = other._ref_counter;
            _add_owner();
        }

        return *this;
	}

#pragma endregion /// Copy semantics
#pragma region /// Move semantics

	[[nodiscard]] constexpr observed_ref(observed_ref&& other) noexcept
	: _ptr{other._ptr}, _ref_counter{other._ref_counter} {
		other._ptr = nullptr;
		other._ref_counter = nullptr;
    }

	constexpr observed_ref& operator=(observed_ref&& other) noexcept {
		if (&other != this) [[likely]] {
			_remove_owner();
			std::swap(_ptr, other._ptr);
			std::swap(_ref_counter, other._ref_counter);
		}

		return *this;
	}

#pragma endregion /// Move semantics
#pragma region /// Ownership utils

	/// @details Removes observed_ref from holding shared ownership to data
	constexpr void reset(T_* ptr = nullptr) noexcept {
		_remove_owner();

		if (ptr != nullptr) {
			_ptr = ptr;
			_ref_counter = new ref_counter_t{};
		}
	}

	/// @warning Unsafe operation
	/// @returns pointer to the underlying data
	[[nodiscard]] constexpr T_* get_ptr() const noexcept { return _ptr; }

	/// @warning Unsafe operation
	/// @returns A constant reference counter
	[[nodiscard]] constexpr const ref_counter_t* get_ref_counter() const noexcept { return _ref_counter; }

/// TODO:
#ifdef XEN_WEAK_REF
	/// @returns A `weak_ref<T_>` of the underlying data
	[[nodiscard]] constexpr weak_ref<T_> get_weak_ref() const noexcept { return weak_ref<T_>{*this}; }
#endif /// XEN_WEAK_REF

#pragma endregion /// Ownership utils
#pragma region /// Operator overload

	constexpr T_& operator*() const noexcept { return *_ptr; }
	constexpr T_* operator->() const noexcept { return _ptr; }

	constexpr operator bool() const noexcept { return _ptr != nullptr; }

	friend constexpr bool operator==(const observed_ref& lhs, const observed_ref& rhs) noexcept { return lhs._ptr == rhs._ptr; }
	friend constexpr bool operator!=(const observed_ref& lhs, const observed_ref& rhs) noexcept { return lhs._ptr != rhs._ptr; }

/// TODO:
#ifdef XEN_WEAK_REF
	friend constexpr bool operator==(const observed_ref& lhs, const weak_ref<T_>& rhs) noexcept { return lhs._ptr == rhs._ptr; }
	friend constexpr bool operator==(const weak_ref<T_>& lhs, const observed_ref& rhs) noexcept { return lhs._ptr == rhs._ptr; }
	friend constexpr bool operator!=(const observed_ref& lhs, const weak_ref<T_>& rhs) noexcept { return lhs._ptr != rhs._ptr; }
	friend constexpr bool operator!=(const weak_ref<T_>& lhs, const observed_ref& rhs) noexcept { return lhs._ptr != rhs._ptr; }
#endif /// XEN_WEAK_REF

#pragma endregion /// Operator overload
};

/// @details constructs `T_` on heap and wraps it in a `observed_ref`
template <typename T_, typename... Args>
[[nodiscard]] constexpr observed_ref<T_> build_observed(Args&&... args) {
	return observed_ref<T_>{new T_{std::forward<Args>(args)...}};
}

} /// namespace xen

#endif /// XEN_OBSERVED_REF