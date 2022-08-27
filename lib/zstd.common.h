#pragma once
#ifndef ZSTD_COMMON_H
#define ZSTD_COMMON_H

#ifdef ZSTD_EXPORT_API
#	warn ZSTD_EXPORT_API already defined
#	undef ZSTD_EXPORT_API
#endif
#ifdef ZSTD_EXPORT_ABI
#	warn ZSTD_EXPORT_ABI already defined
#	undef ZSTD_EPXORT_ABI
#endif
#ifdef ZSTD_EXPORT_VISIBILITY
#	warn ZSTD_EXPORT_VISIBILITY already defined
#	undef ZSTD_EXPORT_VISIBILITY
#endif

#if defined(__clang__) || (defined(__GNUC__) && (__GNUC__ >= 4))
#	define ZSTD_VISIBLE __attribute__((visibility("default")))
#	define ZSTDLIB_HIDDEN __attribute__((visibility("hidden")))
#else
#	define ZSTD_VISIBLE
#	define ZSTDLIB_HIDDEN
#endif

#if WIN32
#	if ZSTD_DLL_EXPORT == 1
#		if !defined(__clang__) && defined(__GNUC__)
#			define ZSTD_EXPORT_API __attribute__((dllexport)) ZSTD_VISIBLE
#			define ZSTD_EXPORT_ABI __cdecl
#		elif defined(_MSC_VER) || defined(__clang__)
#			define ZSTD_EXPORT_API __declspec(dllexport) ZSTD_VISIBLE
#			define ZSTD_EXPORT_ABI __cdecl
#		else
#			error Unknown Toolchain
#		endif
#	elif ZSTD_DLL_IMPORT == 1
#		if !defined(__clang__) && defined(__GNUC__)
#			define ZSTD_EXPORT_API __attribute__((dllimport)) ZSTD_VISIBLE
#			define ZSTD_EXPORT_ABI __cdecl
#		elif defined(_MSC_VER) || defined(__clang__)
#			define ZSTD_EXPORT_API __declspec(dllimport) ZSTD_VISIBLE
#			define ZSTD_EXPORT_ABI __cdecl
#		else
#			error Unknown Toolchain
#		endif
#	else
#		define ZSTD_EXPORT_API
#		define ZSTD_EXPORT_ABI
#	endif
#else
#	define ZSTD_EXPORT_API ZSTD_VISIBLE
#	define ZSTD_EXPORT_ABI
#endif

#if defined(__GNUC__)
// __attribute__((target(...))) // https://gcc.gnu.org/onlinedocs/gcc-12.1.0/gcc/x86-Function-Attributes.html#x86-Function-Attributes
#	define ZSTD_UNUSED __attribute__((unused))
#	define ZSTD_CONST __attribute__((const))
#	define ZSTD_PURE __attribute__((pure))
#	define ZSTD_FLATTEN __attribute__((flatten))
#	define ZSTD_LEAF __attribute__((leaf))
#	define ZSTD_ALLOC __attribute__((malloc)) // returns_nonnull
#	define ZSTD_NORETURN __attribute__((noreturn))
#	define ZSTD_LIKELY(expr) (__builtin_expect(expr, 1))
#	define ZSTD_UNLIKELY(expr) (__builtin_expect(expr, 0))
#	define ZSTD_UNREACHABLE() __builtin_unreachable()
#	if defined(__clang__)
#		define ZSTD_HOT
#		define ZSTD_COLD
#		define ZSTD_UNPREDICTABLE(expr) (__builtin_unpredictable(expr))
#		define ZSTD_ASSUME(expr) __builtin_assume(expr);
#	else
#		define ZSTD_HOT __attribute__((hot))
#		define ZSTD_COLD __attribute__((cold))
#		define ZSTD_UNPREDICTABLE(expr) (__builtin_expect_with_probability(expr, 1, 0.5))
#		define ZSTD_ASSUME(expr) ({ if (!(expr)) __builtin_unreachable(); })
#	endif
#elif defined(_MSC_VER)
#	define ZSTD_UNUSED
#	define ZSTD_CONST __declspec(noalias)
#	define ZSTD_PURE __declspec(noalias)
#	define ZSTD_FLATTEN
#	define ZSTD_LEAF
#	define ZSTD_ALLOC __declspec(restrict)
#	define ZSTD_NORETURN __declspec(noreturn)
#	define ZSTD_HOT
#	define ZSTD_COLD
#	define ZSTD_LIKELY(expr) (expr)
#	define ZSTD_UNLIKELY(expr) (expr)
#	define ZSTD_UNPREDICTABLE(expr) (expr)
#	define ZSTD_ASSUME(expr) __assume(expr)
#	define ZSTD_UNREACHABLE() __assume(0)
#else
#	define ZSTD_UNUSED
#	define ZSTD_CONST
#	define ZSTD_PURE
#	define ZSTD_FLATTEN
#	define ZSTD_LEAF
#	define ZSTD_ALLOC
#	define ZSTD_NORETURN
#	define ZSTD_HOT
#	define ZSTD_COLD
#	define ZSTD_LIKELY(expr) (expr)
#	define ZSTD_UNLIKELY(expr) (expr)
#	define ZSTD_UNPREDICTABLE(expr) (expr)
#	define ZSTD_ASSUME(expr)
#	define ZSTD_UNREACHABLE()
#endif

#endif // ZSTD_COMMON_H
