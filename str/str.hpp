#pragma once

#ifndef XEN_STR
#define XEN_STR

#include "core/numdef.hpp"
#include "core/safe_u64.hpp"

namespace xen {

/// @return the length of the given text (w/o the `\0`)
[[nodiscard]] inline u_size get_text_len(const char* text) noexcept {
	const char* START = text;
	while (*text != '\0') { ++text; }

	return u_size{static_cast<u64_t>(text - START)};
}

/// @class `str`
/// @brief A safe dynamic array of characters.
/// @section Features:
/// - Smart memory management of the character buffer.
/// - Copy: Deep copies inner content to prevent memory conflict.
/// - Move: Transfers ownership of underlying data to prevent memory conflicts.
/// - Supports implicit/explicit conversion from `const char*`.
/// - Supports ostream `<<` operator for displaying underlying string.
/// - Comparison: conducts deep check of 2 `str` to verify similarity (==, !=)
/// - Joins 2 `str` together to create a new `str`. (concat, +=, +)
class str {
private:
	char* _char_buf {nullptr};
	u_size _len {0};

#pragma region /// Helpers

	/// @warning `_len` and `_char_buf` to be manually set before calling
	constexpr static void _raw_copy_text(char* dest, const char* src) noexcept {
		char* dest_it = dest;
		while ((*dest_it = *src) != '\0') {
			++dest_it;
			++src;
		}
	}

	/// @details copies raw c-style string to self
	void _copy_text(const char* text) noexcept {
		delete[] _char_buf;

		_len = get_text_len(text);
		_char_buf = new char[_len + 1];
		_raw_copy_text(_char_buf, text);
	}

#pragma endregion /// Helpers

public:
#pragma region /// Constrctors

	[[nodiscard]] constexpr str() noexcept = default;

	[[nodiscard]] str(const char* text) noexcept {
		_copy_text(text == nullptr ? "" : text);
	}

	constexpr ~str() noexcept { delete[] _char_buf; }

#pragma endregion /// Constrctors
#pragma region /// Copy semantics

	[[nodiscard]] constexpr str(const str& other) noexcept {
		_len = other._len;
		_char_buf = new char[_len + 1];
		_raw_copy_text(_char_buf, other.c_str());
	}
	
	constexpr str& operator=(const str& other) noexcept {
		if (&other != this) [[likely]] {
			reset();
			_len = other._len;
			_char_buf = new char[_len + 1];
			_raw_copy_text(_char_buf, other.c_str());
		}

		return *this;
	}

#pragma endregion /// Copy semantics
#pragma region /// Move semantics

	[[nodiscard]] constexpr str(str&& other) noexcept {
		_len = other._len;
		_char_buf = other._char_buf;

		other._len = 0;
		other._char_buf = nullptr;
	}

	constexpr str& operator=(str&& other) noexcept {
		if (&other != this) [[likely]] {
			_len = other._len;
			_char_buf = other._char_buf;

			other._len = 0;
			other._char_buf = nullptr;
		}

		return *this;
	}

#pragma endregion /// Move semantics
	#ifdef _OSTREAM_
	/// @details Console logging support
	friend std::ostream& operator<<(std::ostream& os, const str& str) noexcept {
		os << str._char_buf;
		return os;
	}
	#endif /// _OSTREAM_
#pragma region /// Iterator

	/// @returns iterator to the start of the `_char_buf`
	constexpr char* begin() noexcept { return _char_buf; }

	/// @returns iterator to the start of the `_char_buf`
	constexpr char* end() noexcept { return _char_buf + _len; }

	/// @returns const iterator to the start of the `_char_buf`
	constexpr const char* begin() const noexcept { return _char_buf; }

	/// @returns const iterator to the start of the `_char_buf`
	constexpr const char* end() const noexcept { return _char_buf + _len; }

	/// @returns const iterator to the start of the `_char_buf`
	constexpr const char* cbegin() const noexcept { return _char_buf; }

	/// @returns const iterator to the start of the `_char_buf`
	constexpr const char* cend() const noexcept { return _char_buf + _len; }

#pragma endregion /// Iterator
#pragma region /// String utils

	/// @returns Underlying `char*`.
	[[nodiscard]] constexpr const char* c_str() const noexcept { return _char_buf; }

	/// @returns Total no.of characters in string.
	[[nodiscard]] constexpr u_size len() const noexcept { return _len; }

	/// @returns `true` if string is empty.
	[[nodiscard]] constexpr bool is_empty() const noexcept { return _len == 0; }

	/// @details Clears character buffer
	constexpr void reset() noexcept {
		_len = 0;
		delete[] _char_buf;
		_char_buf = nullptr;
	}

#pragma endregion /// String utils
#pragma region /// Comparison operator

	friend bool operator==(const str& lhs, const str& rhs) {
		if (lhs.c_str() == rhs.c_str()) return true;
		if (lhs.len() != rhs.len()) return false;

		for (u_size i = 0; i < lhs.len(); i++) {
			if (lhs._char_buf[i] != rhs._char_buf[i]) return false;
		}

		return true;
	}

	friend bool operator!=(const str& lhs, const str& rhs) { return !(lhs == rhs); }

#pragma endregion /// Comparison operator
#pragma region /// Concatenation

	/// @returns Joins two strings into one and returns it
	constexpr static str concat(const str& lhs, const str& rhs) {
		u_size new_len {lhs.len() + rhs.len()};
		if (new_len == 0) return str {};

		str s {};
		s._len = new_len;
		s._char_buf = new char[new_len + 1];

		for (u_size i = 0; i < lhs.len(); i++) s._char_buf[i] = lhs._char_buf[i];
		for (u_size i = 0; i < rhs.len(); i++) s._char_buf[lhs.len() + i] = rhs._char_buf[i];

		s._char_buf[new_len] = '\0';
		return s;
	}

	friend str& operator+=(str& lhs, const str& rhs) {
		lhs = std::move(concat(lhs, rhs));
		return lhs;
	}

	friend str operator+(const str& lhs, const str& rhs) {
		return concat(lhs, rhs);
	}

#pragma endregion /// Concatenation
};

} /// namespace xen

#endif /// XEN_STR