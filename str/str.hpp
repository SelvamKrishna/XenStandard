#pragma once

#ifndef XEN_STR
#define XEN_STR

#include "core/numdef.hpp"
#include "core/safe_u64.hpp"

namespace xen {

/// @return the length of the given text (w/o the `\0`)
[[nodiscard]] inline s_size get_text_len(const char* text) noexcept {
	const char* START = text;
	while (*text != '\0') { ++text; }

	return s_size{static_cast<u64_t>(START - text)};
}

/// @class `str`
/// @brief A safe dynamic array of characters.
/// @section Features:
/// - Smart memory management of the character buffer.
/// - Copy: Deep copies inner content to prevent memory conflict.
/// - Move: Transfers ownership of underlying data to prevent memory conflicts.
/// - Supports implicit/explicit conversion from `const char*`.
/// - Supports ostream `<<` operator for displaying underlying string.
///
/// TODO: String manipulation support
/// UNTESTED:
class str {
private:
	char* _character_buf {nullptr};
	s_size _len;

	/// @warning `_len` and `_character_buf` to be manually set before calling
	constexpr void _raw_copy_text(const char* text) noexcept {
		char* dest = _character_buf;
		while ((*dest = *text) != '\0') {
			++dest;
			++text;
		}
	}

	/// @details copies raw c-style string to self
	void _copy_text(const char* text) noexcept {
		delete[] _character_buf;

		_len = get_text_len(text);
		_character_buf = new char[_len + 1];
		_raw_copy_text(text);
	}

public:
	[[nodiscard]] constexpr str() noexcept = default;

	[[nodiscard]] str(const char* text) noexcept {
		_copy_text(text == nullptr ? "" : text);
	}

	constexpr ~str() noexcept { delete[] _character_buf; }

#pragma region /// Copy semantics

	[[nodiscard]] constexpr str(const str& other) noexcept {
		if (&other != this) [[likely]] {
			_len = other._len;
			_character_buf = new char[_len + 1];
			_raw_copy_text(other.c_str());
		}
	}
	
	[[nodiscard]] constexpr str& operator=(const str& other) noexcept {
		if (&other != this) [[likely]] {
			reset();
			_len = other._len;
			_character_buf = new char[_len + 1];
			_raw_copy_text(other.c_str());
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

#ifdef _OSTREAM_
	/// @details Console logging support
	friend std::ostream& operator<<(std::ostream& os, const str& str) noexcept {
		os << str._character_buf;
		return os;
	}
#endif

#pragma region /// String utils

	/// @returns Underlying `char*`.
	[[nodiscard]] constexpr char* c_str() const noexcept { return _character_buf; }

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
