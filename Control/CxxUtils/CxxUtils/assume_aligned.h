/*
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 */

/**
 * @file CxxUtils/assume_aligned.h
 * @author Christos Anastopoulos
 * @date Jan, 2022
 * @brief @c assumed_aligned for C++17
 *
 *
 * C++20 introduces std::assumed_aligned.
 * From the relevant paper
 * http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1007r1.pdf
 *
 * This is to allow using it already in C++17
 */

#ifndef CXXUTILS_ASSUME_ALIGNED_H
#define CXXUTILS_ASSUME_ALIGNED_H

#include <cstddef>

namespace CxxUtils {
template<size_t Align, typename T>
[[nodiscard]]
#if (defined(__GNUC__) || defined(__clang__))
[[gnu::always_inline]]
#endif
inline T*
assume_aligned(T* ptr) noexcept
{
  static_assert((Align != 0 && (Align & (Align - 1)) == 0),
                "Align not a power of 2");

#if (defined(__GNUC__) || defined(__clang__))
  return static_cast<T*>(__builtin_assume_aligned(ptr, Align));
#else
  return ptr;
#endif
}
} // namespace CxxUtils

#endif
