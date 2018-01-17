// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file CxxUtils/atomic_fetch_minmax.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2017
 * @brief Atomic min/max functions.
 *
 * These add atomic operations for finding the minimum or maximum.
 * These have been proposed for addition to the standard, but have not
 * yet made it in:
 *
 *   http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3696.htm
 *   http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0493r0.pdf
 */


#ifndef CXXUTILS_ATOMIC_FETCH_MINMAX_H
#define CXXUTILS_ATOMIC_FETCH_MINMAX_H


#include "CxxUtils/stall.h"
#include <algorithm>
#include <atomic>


namespace CxxUtils {


/**
 * @brief Atomically calculate maximum.
 * @param a Pointer to the atomic value.
 * @param v The other value.
 * @param memorder Memory ordering.
 *
 * Computes max(*a, v) and stores it in *a.  Returns the original value of *a.
 */
template <class T>
inline
T atomic_fetch_max (std::atomic<T>* a, T v,
                    std::memory_order memorder = std::memory_order_seq_cst)
{
  T orig = a->load (memorder);
  T max = std::max (orig, v);
  while (!a->compare_exchange_strong (orig, max, memorder)) {
    CxxUtils::stall();
    max = std::max (orig, v);
  }
  return orig;
}


/**
 * @brief Atomically calculate minimum.
 * @param a Pointer to the atomic value.
 * @param v The other value.
 * @param memorder Memory ordering.
 *
 * Computes min(*a, v) and stores it in *a.  Returns the original value of *a.
 */
template <class T>
inline
T atomic_fetch_min (std::atomic<T>* a, T v,
                    std::memory_order memorder = std::memory_order_seq_cst)
{
  T orig = a->load (memorder);
  T min = std::min (orig, v);
  while (!a->compare_exchange_strong (orig, min, memorder)) {
    CxxUtils::stall();
    min = std::min (orig, v);
  }
  return orig;
}


} // namespace CxxUtils


#endif // not CXXUTILS_ATOMIC_FETCH_MINMAX_H
