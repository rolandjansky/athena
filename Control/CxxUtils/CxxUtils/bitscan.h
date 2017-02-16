/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CxxUtils/bitscan.h
 * @author Frank Winklmeier
 * @brief Bit scanning functions
 *
 * Fast helper functions to count number of leading/traling zeros.
 * Supports 32 and 64 bit input types.
 *
 * Inspired by boost/multiprecision/detail/bitscan.hpp
 *             https://en.wikipedia.org/wiki/Find_first_set
 */

#ifndef CXXUTILS_BITSCAN_H
#define CXXUTILS_BITSCAN_H

#include <stdint.h>

namespace CxxUtils {

  /**
   * Count number of trailing zeros
   *
   * @param x Number to check
   * @return Number of trailing zeros, 0 if x==0
   */
  inline unsigned count_trailing_zeros(unsigned x) {
#if defined (__GNUC__) || defined(__clang__)
    return (x!=0 ? __builtin_ctz(x) : 0);
#else
    return detail::ctz_portable(x);
#endif
  }

  inline unsigned count_trailing_zeros(unsigned long x) {
#if defined (__GNUC__) || defined(__clang__)
    return (x!=0 ? __builtin_ctzl(x) : 0);
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
  inline unsigned count_leading_zeros(uint32_t x) {
#if defined (__GNUC__) || defined(__clang__)
    return (x!=0 ? __builtin_clz(x) : 32);
#else
    return detail::clz_portable(x);
#endif
  }

  inline unsigned count_leading_zeros(uint64_t x) {
#if defined (__GNUC__) || defined(__clang__)
    return (x!=0 ? __builtin_clzl(x) : 64);
#else
    return detail::clz_portable(x);
#endif
  }

  /**
   * Portable implementations
   *
   * These could be replaced by faster algorithms if needed
   */
  namespace detail {

    template <typename T>
    inline unsigned ctz_portable(T x) {
      unsigned n(0);
      if (x!=0) {
        for (;(x & 0x1) == 0; ++n, x >>= 1);
      }
      return n;
    }    

    template <typename T>
    inline unsigned clz_portable(T x) {
      if (x==0) return sizeof(T)*8;
      unsigned n;
      T msb = static_cast<T>(1) << (sizeof(T)*8-1);
      for (n = 0; (x & msb) == 0; ++n, x <<= 1);
      return n;
    }    
  }


}
#endif // CXXUTILS_BITSCAN_H
