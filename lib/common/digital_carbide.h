#pragma once

// #define USE_XXH3 1

//#define XXH_NO_XXH32 1
//#define XXH_NO_XXH64 1
//#define XXH_NO_XXH3 1
//#define XXH_NO_XXH3_64 1
//#define XXH_NO_XXH3_128 1

#ifdef ZSTD_USE_XXH3
#	define XXH_NO_XXH32 1
#	define XXH_NO_XXH64 1
#endif

/*
#ifndef XXH_NO_XXH3
#	ifdef XXH_NO_XXH3_64
#		undef XXH_NO_XXH3_64
#	endif
#	define XXH_NO_XXH3_64 1
#	ifdef XXH_NO_XXH3_128
#		undef XXH_NO_XXH3_128
#	endif
#	define XXH_NO_XXH3_128 1
#endif
*/

#if !USE_XXH3
#	define _XXH_64(input, length, seed) XXH64((input), (length), (seed))
#else
#endif

// XXH_NO_XXH3_128
// XXH32_ENDJMP

//#define XXH_PUBLIC_API static
