// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CxxUtils/vec_fb.h
 * @author scott snyder <snyder@bnl.gov>
 *
 * @brief Fallback vectorized class.
 *
 * This is intended to be (mostly) functionally equivalent to the
 * built-in vectorized types.  (One difference is that we don't
 * support ?:, as that can't be overloaded.)
 */

#ifndef CXXUTILS_VEC_FB_H
#define CXXUTILS_VEC_FB_H

#include "boost/integer.hpp"
#include <initializer_list>
#include <algorithm>

namespace CxxUtils {
/**
 */
template<typename T, size_t N>
class vec_fb
{
public:
  vec_fb() = default;
  vec_fb(const vec_fb&) = default;
  vec_fb& operator=(const vec_fb&) = default;
  vec_fb(std::initializer_list<T> init)
  {
    std::copy(init.begin(), init.end(), m_arr);
    std::fill(m_arr + init.size(), m_arr + N, T());
  }

  T operator[](size_t n) const { return m_arr[n]; }
  T& operator[](size_t n) { return m_arr[n]; }

  T m_arr[N];
};

// Helper: Given a vectorized class, find another vectorized class
// that uses integers of the same size as the original class.
template<typename T, size_t N>
using ivec = vec_fb<typename boost::int_t<sizeof(T) * 8>::exact, N>;

// Define binary operations.
// For each operation, define
//   V1 OP V2
//   V OP S
//   S OP V
//   V1 OP= V2
//   V  OP= S

#define BINOP(op)                                                              \
  template<typename T, size_t N>                                               \
  inline vec_fb<T, N> operator op(const vec_fb<T, N>& a,                       \
                                  const vec_fb<T, N>& b)                       \
  {                                                                            \
    vec_fb<T, N> c;                                                            \
    for (size_t i = 0; i < N; ++i)                                             \
      c.m_arr[i] = a.m_arr[i] op b.m_arr[i];                                   \
    return c;                                                                  \
  }                                                                            \
  template<typename T, size_t N, typename U>                                   \
  inline vec_fb<T, N> operator op(const vec_fb<T, N>& a, U b)                  \
  {                                                                            \
    vec_fb<T, N> c;                                                            \
    for (size_t i = 0; i < N; ++i)                                             \
      c.m_arr[i] = a.m_arr[i] op b;                                            \
    return c;                                                                  \
  }                                                                            \
  template<typename T, size_t N, typename U>                                   \
  inline vec_fb<T, N> operator op(U a, const vec_fb<T, N>& b)                  \
  {                                                                            \
    vec_fb<T, N> c;                                                            \
    for (size_t i = 0; i < N; ++i)                                             \
      c.m_arr[i] = a op b.m_arr[i];                                            \
    return c;                                                                  \
  }                                                                            \
  template<typename T, size_t N>                                               \
  inline vec_fb<T, N>& operator op##=(vec_fb<T, N>& a, const vec_fb<T, N>& b)  \
  {                                                                            \
    for (size_t i = 0; i < N; ++i)                                             \
      a.m_arr[i] op## = b.m_arr[i];                                            \
    return a;                                                                  \
  }                                                                            \
  template<typename T, size_t N, typename U>                                   \
  inline vec_fb<T, N>& operator op##=(vec_fb<T, N>& a, U b)                    \
  {                                                                            \
    for (size_t i = 0; i < N; ++i)                                             \
      a.m_arr[i] op## = b;                                                     \
    return a;                                                                  \
  }

BINOP(+)
BINOP(-)
BINOP(*)
BINOP(/)
BINOP(^)
BINOP(|)
BINOP(&)
BINOP(%)
BINOP(>>)
BINOP(<<)

#undef BINOP

// Define unary operations.

#define UNOP(op)                                                               \
  template<typename T, size_t N>                                               \
  inline vec_fb<T, N> operator op(const vec_fb<T, N>& a)                       \
  {                                                                            \
    vec_fb<T, N> c;                                                            \
    for (size_t i = 0; i < N; ++i)                                             \
      c.m_arr[i] = op a.m_arr[i];                                              \
    return c;                                                                  \
  }

UNOP(-)
UNOP(~)

#undef UNOP

// Define relational operations.

#define RELOP(op)                                                              \
  template<typename T, size_t N>                                               \
  inline ivec<T, N> operator op(const vec_fb<T, N>& a, const vec_fb<T, N>& b)  \
  {                                                                            \
    ivec<T, N> c;                                                              \
    for (size_t i = 0; i < N; ++i)                                             \
      c.m_arr[i] = a.m_arr[i] op b.m_arr[i];                                   \
    return c;                                                                  \
  }

RELOP(==)
RELOP(!=)
RELOP(<)
RELOP(<=)
RELOP(>)
RELOP(>=)

#undef RELOP

/// Negation.
template<typename T, size_t N>
inline ivec<T, N>
operator!(const vec_fb<T, N>& a)
{
  ivec<T, N> c;
  for (size_t i = 0; i < N; ++i)
    c.m_arr[i] = a.m_arr[i] == 0;
  return c;
}

/// V1 && V2
template<typename T, size_t N>
inline ivec<T, N>
operator&&(const vec_fb<T, N>& a, const vec_fb<T, N>& b)
{
  ivec<T, N> c;
  for (size_t i = 0; i < N; ++i)
    c.m_arr[i] = (a.m_arr[i] != 0) & (b.m_arr[i] != 0);
  return c;
}

/// S && V
template<typename T, size_t N, class U>
inline ivec<T, N>
operator&&(U a, const vec_fb<T, N>& b)
{
  ivec<T, N> c;
  for (size_t i = 0; i < N; ++i)
    c.m_arr[i] = a ? b.m_arr[i] != 0 : 0;
  return c;
}

/// V && S
template<typename T, size_t N, class U>
inline ivec<T, N>
operator&&(const vec_fb<T, N>& a, U b)
{
  ivec<T, N> c;
  for (size_t i = 0; i < N; ++i)
    c.m_arr[i] = (a.m_arr[i] != 0) & (b ? -1 : 0);
  return c;
}

/// V1 || V2
template<typename T, size_t N>
inline ivec<T, N>
operator||(const vec_fb<T, N>& a, const vec_fb<T, N>& b)
{
  ivec<T, N> c;
  for (size_t i = 0; i < N; ++i)
    c.m_arr[i] = (a.m_arr[i] != 0) | (b.m_arr[i] != 0);
  return c;
}

} // namespace CxxUtils

#endif // CXXUTILS_VEC_FB_H
