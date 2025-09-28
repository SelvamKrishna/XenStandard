#pragma once

#ifndef XEN_STR
#define XEN_STR

#include "core/safe_u64.hpp"

#include <cstring>
#include <ostream>

namespace xen {

/// @class str
/// @brief A safe dynamic array of characters.
/// @section Features:
/// - Smart memory management of the character buffer.
/// - Copy: Deep copies inner content to prevent memory conflict.
/// - Move: Transfers ownership of underlying data to prevent memory conflicts.
/// - Supports implicit/explicit conversion from `const char*`.
/// - Supports ostream `<<` operator for displaying underlying string.
///
/// TODO: Implement `xen:owned_ptr` for memory handling
/// TODO: String manipulation support
/// UNTESTED:
class str {
private:
	char* _character_buf {nullptr};
	s_size _len;

public:
	[[nodiscard]] constexpr str() noexcept = default;

	[[nodiscard]] constexpr str(const char* text) noexcept {
		if (text == nullptr) { // Empty string
			_len = 0;
			_character_buf = new char[1];
			_character_buf[0] = '\0';
		} else { // Proper string
			_len = std::strlen(text);
			_character_buf = new char[_len + 1];
			strcpy_s(_character_buf, _len + 1, text);
		}
	}

	constexpr ~str() noexcept { delete[] _character_buf; }

#pragma region /// Copy semantics

	[[nodiscard]] constexpr str(const str& other) noexcept {
		if (&other != this) [[likely]] {
			_len = other._len;
			_character_buf = new char[_len + 1];
			strcpy_s(_character_buf, _len + 1, other._character_buf);
		}
	}
	
	[[nodiscard]] constexpr str& operator=(const str& other) noexcept {
		if (&other != this) [[likely]] {
			reset();
			_len = other._len;
			_character_buf = new char[_len + 1];
			strcpy_s(_character_buf, _len + 1, other._character_buf);
		}

		return *this;
	}

#pragma endregion /// Copy semantics

#pragma region /// Move semantics

	[[nodiscard]] constexpr str(str&& other) noexcept {
		_len = other._len;
		_character_buf = other._character_buf;

		other._len = 0;
		other._character_buf = nullptr;
	}

	[[nodiscard]] constexpr str& operator=(str&& other) noexcept {
		if (&other != this) [[likely]] {
			_len = other._len;
			_character_buf = other._character_buf;

			other._len = 0;
			other._character_buf = nullptr;
		}

		return *this;
	}

#pragma endregion /// Move semantics

	/// @details Console logging support
	friend std::ostream& operator<<(std::ostream& os, const str& str) noexcept {
		os << str._character_buf;
		return os;
	}
#pragma region /// String utils

	/// @returns Underlying `const char*`.
	[[nodiscard]] constexpr const char* c_str() const noexcept { return _character_buf; }

	/// @returns Total no.of characters in string.
	[[nodiscard]] constexpr s_size len() const noexcept { return _len; }

	/// @returns `true` if string is empty.
	[[nodiscard]] constexpr bool is_empty() const noexcept { return _len == 0; }

	/// @details Clears character buffer
	constexpr void reset() noexcept {
		_len = 0;
		delete[] _character_buf;
	}

#pragma endregion /// String utils
};

} /// namespace xen

#endif /// XEN_STR
