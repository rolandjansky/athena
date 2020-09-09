// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file CxxUtils/features.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2018
 * @brief Some additional feature test macros.
 */

#ifndef CXXUTILS_FEATURES_H
#define CXXUTILS_FEATURES_H

/// Do we have function multiversioning?  GCC and clang > 7 support
/// the target attribute
#if defined(__ELF__) && defined(__GNUC__) && !defined(__CLING__) &&            \
  !defined(__ICC) && !defined(__COVERITY__) && !defined(__CUDACC__) &&         \
  !defined(CL_SYCL_LANGUAGE_VERSION) && !defined(__HIP__)
# define HAVE_FUNCTION_MULTIVERSIONING 1
#else
# define HAVE_FUNCTION_MULTIVERSIONING 0
#endif

/// Do we have the target_clones attribute?  clang  does not support it
#if defined(__GNUC__) && !defined(__clang__) && !defined(__ICC) &&             \
  !defined(__COVERITY__) && !defined(__CUDACC__)
# define HAVE_TARGET_CLONES 1
#else
# define HAVE_TARGET_CLONES 0
#endif

/// Do we have support for all GCC intrinsics?
#if defined(__GNUC__) && !defined(__clang__) && !defined(__ICC) &&             \
  !defined(__COVERITY__) && !defined(__CUDACC__)
# define HAVE_GCC_INTRINSICS 1
#else
# define HAVE_GCC_INTRINSICS 0
#endif

/// Do we have the bit-counting intrinsics?
// __builtin_ctz
// __builtin_ctzl
// __builtin_ctzll
// __builtin_clz
// __builtin_clzl
// __builtin_clzll
// __builtin_popcount
// __builtin_popcountl
// __builtin_popcountll
#if defined(__GNUC__) || defined(__clang__)
# define HAVE_BITCOUNT_INTRINSICS 1
#else
# define HAVE_BITCOUNT_INTRINSICS 0
#endif

// Do we have the vector_size attribute for writing explicitly
// vectorized code?
#if (defined(__GNUC__) || defined(__clang__)) && !defined(__ICC) &&            \
  !defined(__COVERITY__) && !defined(__CUDACC__)
# define HAVE_VECTOR_SIZE_ATTRIBUTE 1
#else
# define HAVE_VECTOR_SIZE_ATTRIBUTE 0
#endif

// Do we additionally support the ternary operator for vectorizes types.
// GCC and llvm clang >=10
#if HAVE_VECTOR_SIZE_ATTRIBUTE &&                                              \
  !(defined(__clang__) && ((__clang_major__ < 10) || defined(__APPLE__)))
#define HAVE_VECTOR_TERNARY_OPERATOR 1
#else
#define HAVE_VECTOR_TERNARY_OPERATOR 0
#endif

#endif // not CXXUTILS_FEATURES_H
