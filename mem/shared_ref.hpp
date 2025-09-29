#pragma once

#ifndef XEN_SHARED_REF
#define XEN_SHARED_REF

#include "core/safe_u64.hpp"

#include <utility>

namespace xen {

/// @class `shared_ref`
/// @brief A class which takes shared ownership of the underlying data
/// @warning Try to construct with `xen::shared_ref<T_>::make(...)` for object creation
/// @section Features:
/// - Wraps a raw pointer with RAII principles
/// - Automatically frees memory when there are no more shared owners
/// - Underlying data can be copied and moved safely
/// - Operator overloaded to behave just like a raw pointer
/// - Can be compared with other `shared_ref` (`==`, `!=`)
///
/// UNTESTED:
template <typename T_>
class shared_ref {
private:
	T_* _ptr{nullptr};
	s_size* _share_count{nullptr};

	/// @details Increments the `_share_count`
	constexpr void _add_owner() noexcept { if (_share_count != nullptr) [[likely]] ++(*_share_count); }

	/// @details Decrements the `_share_count`, resets members and frees them if `_share_count == 0`
	constexpr void _remove_owner() noexcept {
		if (_share_count == nullptr) [[unlikely]] return;

		if (--(*_share_count) == 0) {
			delete _ptr;
			delete _share_count;
		}

		_ptr = nullptr;
		_share_count = nullptr;
	}

public:
#pragma region /// Constructors & Destructors

	[[nodiscard]] constexpr explicit shared_ref() noexcept = default;

	[[nodiscard]] constexpr explicit shared_ref(T_* ptr) noexcept 
	: _ptr{ptr}
	, _share_count{ptr != nullptr ? new s_size{1} : nullptr} 
	{}

	constexpr ~shared_ref() noexcept {
		_ptr = nullptr;
		_remove_owner(); 
	}

#pragma endregion /// Constructors & Destructors

#pragma region /// Copy semantics

	[[nodiscard]] constexpr shared_ref(const shared_ref& other) noexcept 
	: _ptr{other._ptr}, _share_count{other._share_count} {
		if (other._ptr != nullptr) [[likely]] _add_owner();
	}

	constexpr shared_ref& operator=(const shared_ref& other) noexcept {
		if (this != &other) {
            _remove_owner();
            _ptr = other._ptr;
            _share_count = other._share_count;
            _add_owner();
        }

        return *this;
	}

#pragma endregion /// Copy semantics

#pragma region /// Move semantics

	[[nodiscard]] constexpr shared_ref(shared_ref&& other) noexcept
	: _ptr{other._ptr}, _share_count{other._share_count} {
        other._ptr = other._share_count = nullptr;
    }

	constexpr shared_ref& operator=(shared_ref&& other) noexcept {
		if (&other != this) [[likely]] {
			_remove_owner();
			_ptr = other._ptr;
			_share_count = other._share_count;
			other._ptr = other._share_count = nullptr;
		}

		return *this;
	}

#pragma endregion /// Move semantics

#pragma region /// Ownership utils

	/// @details Removes shared_ref from holding ownership to data
	constexpr void reset(T_* ptr = nullptr) noexcept {
		_remove_owner();

		if (ptr != nullptr) {
			_ptr = ptr;
			_share_count = new s_size{1};
		}
	}

	/// @warning Unsafe operation
	/// @returns pointer to the underlying data
	[[nodiscard]] constexpr T_* get() const noexcept { return _ptr; }

	/// @returns the total number of shared owners
	[[nodiscard]] constexpr s_size get_count() const noexcept {
		return _share_count == nullptr ? s_size{0} : (*_share_count);
	}

#pragma endregion /// Ownership utils

#pragma region /// Operator overload

	constexpr T_& operator*() const noexcept { return *_ptr; }
	constexpr T_* operator->() const noexcept { return _ptr; }

	constexpr operator bool() const noexcept { return _ptr != nullptr; }

	friend constexpr bool operator==(const shared_ref& a, const shared_ref& b) noexcept { return a._ptr == b._ptr; }
	friend constexpr bool operator!=(const shared_ref& a, const shared_ref& b) noexcept { return a._ptr != b._ptr; }

#pragma endregion /// Operator overload
};

/// @details constructs `T_` on heap and wraps it in a `shared_ref`
template <typename T_, typename... Args>
[[nodiscard]] constexpr shared_ref<T_> build_shared(Args&&... args) {
	return shared_ref{new T_{std::forward<Args>(args)...}};
}

} /// namespace xen

#endif /// XEN_SHARED_REF