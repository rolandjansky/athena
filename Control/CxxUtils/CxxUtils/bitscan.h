/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CxxUtils/bitscan.h
 * @author Frank Winklmeier
 * @brief Bit scanning functions
 *
 * Fast helper functions to count number of leading/trailing zeros.
 * Supports 32 and 64 bit input types.
 *
 * Inspired by boost/multiprecision/detail/bitscan.hpp
 *             https://en.wikipedia.org/wiki/Find_first_set
 */

#ifndef CXXUTILS_BITSCAN_H
#define CXXUTILS_BITSCAN_H

#include "CxxUtils/features.h"
#include <climits>
#include <cstdint>
#include <type_traits>

namespace CxxUtils {

  /**
   * Portable implementations
   *
   * These could be replaced by faster algorithms if needed
   */
  namespace detail {

    template <typename T>
    inline unsigned ctz_portable(T x) {
      static_assert(std::is_integral<T>::value, "An integer type is required.");
      unsigned n(0);
      if (x!=0) {
        for (;(x & 0x1) == 0; ++n, x >>= 1);
      }
      return n;
    }    

    template <typename T>
    inline unsigned clz_portable(T x) {
      static_assert(std::is_integral<T>::value, "An integer type is required.");
      if (x==0) return sizeof(T)*8;
      unsigned n(0);
      T msb = static_cast<T>(1) << (sizeof(T)*8-1);
      for (n = 0; (x & msb) == 0; ++n, x <<= 1);
      return n;
    }    

    template <typename T>
    inline unsigned popcount_portable(T x) {
      static_assert(std::is_integral<T>::value, "An integer type is required.");
      unsigned n = 0;
      while (x != 0) {
        n += (x&1);
        x >>= 1;
      }
      return n;
    }    
  }

  /**
   * Count number of trailing zeros
   *
   * @param x Number to check
   * @return Number of trailing zeros, 0 if x==0
   */
  inline unsigned count_trailing_zeros(unsigned x) {
#if HAVE_BITCOUNT_INTRINSICS
    return (x!=0 ? __builtin_ctz(x) : 0);
#else
    return detail::ctz_portable(x);
#endif
  }

  inline unsigned count_trailing_zeros(unsigned long x) {
#if HAVE_BITCOUNT_INTRINSICS
    return (x!=0 ? __builtin_ctzl(x) : 0);
#else
    return detail::ctz_portable(x);
#endif
  }

  inline unsigned count_trailing_zeros(unsigned long long x) {
#if HAVE_BITCOUNT_INTRINSICS
    return (x!=0 ? __builtin_ctzll(x) : 0);
#else
    return detail::ctz_portable(x);
#endif
  }

  /**
   * Count number of leading zeros
   *
   * @param x Number to check
   * @return Number of leading zeros, input size in bits if x==0
   */
  inline unsigned count_leading_zeros(unsigned x) {
#if HAVE_BITCOUNT_INTRINSICS
    return (x!=0 ? __builtin_clz(x) : sizeof(x)*CHAR_BIT);
#else
    return detail::clz_portable(x);
#endif
  }

  inline unsigned count_leading_zeros(unsigned long x) {
#if HAVE_BITCOUNT_INTRINSICS
    return (x!=0 ? __builtin_clzl(x) : sizeof(x)*CHAR_BIT);
#else
    return detail::clz_portable(x);
#endif
  }

  inline unsigned count_leading_zeros(unsigned long long x) {
#if HAVE_BITCOUNT_INTRINSICS
    return (x!=0 ? __builtin_clzll(x) : sizeof(x)*CHAR_BIT);
#else
    return detail::clz_portable(x);
#endif
  }


  /**
   * Count number of set bits.
   *
   * @param x Number to check
   * @return Number of bits set in x.
   */
#if defined(__x86_64__) && HAVE_TARGET_CLONES
  // We want to use the popcnt instruction for this if it's available.
  // However, we're still compiling for a Model-T x86_64 by default.
  // Use the target attribute and function multiversioning to use
  // this instruction if it is in fact available.
  // clang has intrinsics for popcount but not target_clones. 
  __attribute__ ((target_clones ("popcnt,default")))
#endif
  inline unsigned count_ones(unsigned x) {
#if HAVE_BITCOUNT_INTRINSICS
    return __builtin_popcount(x);
#else
    return detail::popcount_portable(x);
#endif
  }

#if defined(__x86_64__) && HAVE_TARGET_CLONES
  __attribute__ ((target_clones ("popcnt,default")))
#endif
  inline unsigned count_ones(unsigned long x) {
#if HAVE_BITCOUNT_INTRINSICS
    return __builtin_popcountl(x);
#else
    return detail::popcount_portable(x);
#endif
  }


#if defined(__x86_64__) && HAVE_TARGET_CLONES
  __attribute__ ((target_clones ("popcnt,default")))
#endif
  inline unsigned count_ones(unsigned long long x) {
#if HAVE_BITCOUNT_INTRINSICS
    return __builtin_popcountll(x);
#else
    return detail::popcount_portable(x);
#endif
  }
}
#endif // CXXUTILS_BITSCAN_H
