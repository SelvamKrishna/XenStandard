#pragma once

#ifndef XEN_UNIQUE_REF
#define XEN_UNIQUE_REF

#include <utility>

namespace xen {

/// @class `unique_ref`
/// @brief A class which takes full ownership of the underlying data
/// @warning Try to construct with `xen::unique_ref<T_>::make(...)`
/// @section Features:
/// - Wraps a raw pointer with RAII principles
/// - Automatically frees memory when the pointer goes out of unique
/// - Underlying data cannot be copied, but can be moved
/// - Operator overloaded to behave just like a raw pointer
///
/// UNTESTED:
template <typename T_>
class unique_ref {
private:
	T_* _ptr{nullptr};

public:
#pragma region /// Constructors & Destructors

	[[nodiscard]] constexpr explicit unique_ref() noexcept = default;

	[[nodiscard]] constexpr explicit unique_ref(T_* ptr) noexcept : _ptr{ptr} {}

	constexpr ~unique_ref() noexcept { delete _ptr; }

#pragma endregion /// Constructors & Destructors

#pragma region /// Copy semantics

	[[nodiscard]] constexpr unique_ref(const unique_ref&) noexcept = delete;
	constexpr unique_ref& operator=(const unique_ref&) noexcept = delete;

#pragma endregion /// Copy semantics

#pragma region /// Move semantics

	[[nodiscard]] constexpr unique_ref(unique_ref&& other) noexcept : _ptr{other.release()} {}

	constexpr unique_ref& operator=(unique_ref&& other) noexcept {
		if (&other != this) [[likely]] {
			delete _ptr;
			_ptr = other.release();
		}

		return *this;
	}

#pragma endregion /// Move semantics

#pragma region /// Ownership utils

	/// @details Releases ownership of underlying data
	/// @returns pointer to the underlying data
	[[nodiscard]] constexpr T_* release() noexcept {
		T_* temp = _ptr;
		_ptr = nullptr;
		return temp;
	}

	/// @details Destroys underlying data
	constexpr void reset(T_* ptr = nullptr) noexcept {
		if (ptr != _ptr) [[likely]] {
			delete _ptr;
			_ptr = ptr;
		}
	}

	/// @warning Unsafe operation
	/// @returns pointer to the underlying data
	[[nodiscard]] constexpr T_* get() const noexcept { return _ptr; }

#pragma endregion /// Ownership utils

#pragma region /// Operator overload

	constexpr T_& operator*() const noexcept { return *_ptr; }
	constexpr T_* operator->() const noexcept { return _ptr; }

	constexpr operator bool() const noexcept { return _ptr != nullptr; }

#pragma endregion /// Operator overload
};

/// @details constructs `T_` on heap and wraps it in a `unique_ref`
template <typename T_, typename... Args>
[[nodiscard]] constexpr unique_ref<T_> build_unique(Args&&... args) {
	return unique_ref{new T_{std::forward<Args>(args)...}};
}

} /// namespace xen

#endif /// XEN_UNIQUE_REF