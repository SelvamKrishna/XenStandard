#pragma once

#ifndef XEN_NUMDEF
#define XEN_NUMDEF

namespace xen {

typedef signed char        i8_t;
typedef short              i16_t;
typedef int                i32_t;
typedef long long          i64_t;

typedef unsigned char      u8_t;
typedef unsigned short     u16_t;
typedef unsigned int       u32_t;
typedef unsigned long long u64_t;

typedef float  f32_t;
typedef double f64_t;

inline constexpr i8_t  I8_MIN  = -128;
inline constexpr i16_t I16_MIN = -32768;
inline constexpr i32_t I32_MIN = -2147483648;
inline constexpr i64_t I64_MIN = -9223372036854775808ull;

inline constexpr i8_t  I8_MAX  = 127;
inline constexpr i16_t I16_MAX = 32767;
inline constexpr i32_t I32_MAX = 2147483647;
inline constexpr i64_t I64_MAX = 9223372036854775807ll;

inline constexpr u8_t  U8_MIN  = 0;
inline constexpr u16_t U16_MIN = 0;
inline constexpr u32_t U32_MIN = 0;
inline constexpr u64_t U64_MIN = 0;

inline constexpr u8_t  U8_MAX  = 255;
inline constexpr u16_t U16_MAX = 65535;
inline constexpr u32_t U32_MAX = 4294967295;
inline constexpr u64_t U64_MAX = 18446744073709551615ull;

} /// namespace xen

#endif /// XEN_NUMDEF