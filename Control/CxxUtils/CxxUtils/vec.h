// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CxxUtils/vec.h
 * @author scott snyder <snyder@bnl.gov>
 * @author Christos Anastopoulos (additional helper methods)
 * @date Mar, 2020
 * @brief Vectorization helpers.
 *
 * gcc and clang provide built-in types for writing vectorized code,
 * using the vector_size attribute.  This usually results in code
 * that is much easier to read and more portable than one would get
 * using intrinsics directly.  However, it is still non-standard,
 * and there are some operations which are kind of awkward.
 *
 * This file provides some helpers for writing vectorized code
 * in C++, as well as a standard-compliant fallback that can be used
 * if the vector types are not available.
 *
 * A vectorized type may be named as @c CxxUtils::vec<T, N>.  Here @c T is the
 * element type, which should be an elementary integer or floating-point type.
 * @c N is the number of elements in the vector; it should be a power of 2.
 * This will either be a built-in vector type if the @c vector_size
 * attribute is supported or a fallback C++ class intended to be
 * (mostly) functionally equivalent.
 *
 *
 * The GCC, clang and fallback vector types support:
 * ++, --, +,-,*,/,%, =, &,|,^,~, >>,<<, !, &&, ||,
 * ==, !=, >, <, >=, <=, =, sizeof and Initialization from brace-enclosed lists
 *
 * Furthemore the GCC and clang>=10 vector types support the ternary operator.
 *
 * We also support some additional operations.
 *
 * Deducing useful types:
 *
 *  - @c CxxUtils::vec_type_t<VEC> is the element type of @c VEC.
 *  - @c CxxUtils::mask_type_t<VEC> is the vector type return by relational
 *                                  operations.
 *
 * Deducing the num of elements in a vectorized type:
 *
 *  - @c CxxUtils::vec_size<VEC>() is the number of elements in @c VEC.
 *  - @c CxxUtils::vec_size(const VEC&) is the number of elements in @c VEC.
 *
 * Additional Helpers for common SIMD operations:
 *
 *  - @c CxxUtils::vbroadcast (VEC& v, T x) initializes each element of
 *                                          @c v with @c x.
 *  - @c CxxUtils::vload (VEC& dst, const vec_type_t<VEC>* src)
 *                                          loads elements from @c src
 *                                          to @c dst
 *  - @c CxxUtils::vstore (vec_type_t<VEC>* dst, const VEC& src)
 *                                          stores elements from @c src
 *                                          to @c dst
 *  - @c CxxUtils::vselect (VEC& dst, const VEC& a, const VEC& b, const
 *                          mask_type_t<VEC>& mask) copies elements
 *                          from @c a or @c b, depending
 *                          on the value of @c  mask to @c dst.
 *                          dst[i] = mask[i] ? a[i] : b[i]
 *  - @c CxxUtils::vmin     (VEC& dst, const VEC& a, const VEC& b)
 *                         copies to @c dst[i]  the min(a[i],b[i])
 *  - @c CxxUtils::vmax    (VEC& dst, const VEC& a, const VEC& b)
 *                         copies to @c dst[i]  the max(a[i],b[i])
 *  - @c CxxUtils::vpermute<mask> (VEC& dst, const VEC& src)
 *                          Fills dst with permutation of src
 *                          according to mask.
 *                          Mask is a list of integers that specifies the elements
 *                          that should be extracted and returned in src.
 *                          dst[i] = src[mask[i]] where mask[i] is the ith integer
 *                          is the mask.
 *
 *
 * In terms of expected performance it might be  advantageous to
 * use vector types that fit the size of the ISA.
 * e.g 128 bit wide for SSE, 256 wide for AVX.
 *
 * Specifying a combination that is not valid for the current architecture
 * causes the compiler to synthesize the instructions using a narrower mode.
 *
 * Consider using Function Multiversioning (CxxUtils/features.h)
 * if you really need to target efficiently multiple ISAs.
 *
 * Obviously, one should also consider if the same result can be achieved via
 * the auto-vectorization compiler optimizations 
 * (e.g see vectorize.h for GCC -O2)
 */

#ifndef CXXUTILS_VEC_H
#define CXXUTILS_VEC_H


#include "CxxUtils/features.h"
#include "boost/integer.hpp"
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <algorithm>
#include <type_traits>


namespace CxxUtils {


// Define @c WANT_VECTOR_FALLBACK prior to including this file to always
// make the fallback class @c vec_fb visible, even if we support the
// built-in type.  Intended for testing.
#ifndef WANT_VECTOR_FALLBACK
# define WANT_VECTOR_FALLBACK 0
#endif

#if (!HAVE_VECTOR_SIZE_ATTRIBUTE) || WANT_VECTOR_FALLBACK!=0

/**
 * @brief Fallback vectorized class.
 *
 * This is intended to be (mostly) functionally equivalent to the
 * built-in vectorized types.  (One difference is that we don't
 * support ?:, as that can't be overloaded.)
 */
template <typename T, size_t N>
class vec_fb
{
public:
  vec_fb() = default;
  vec_fb(const vec_fb&) = default;
  vec_fb(std::initializer_list<T> init)
  {
    std::copy (init.begin(), init.end(), m_arr);
    std::fill (m_arr + init.size(), m_arr + N, T());
  }

  T operator[] (size_t n) const { return m_arr[n]; }
  T& operator[] (size_t n) { return m_arr[n]; }

  T m_arr[N];
};


// Helper: Given a vectorized class, find another vectorized class
// that uses integers of the same size as the original class.
template <typename T, size_t N>
using ivec = vec_fb<typename boost::int_t<sizeof(T)*8>::exact, N>;

// Define binary operations.
// For each operation, define
//   V1 OP V2
//   V OP S
//   S OP V
//   V1 OP= V2
//   V  OP= S

#define BINOP(op)                                                         \
  template <typename T, size_t N>                                         \
  inline                                                                  \
  vec_fb<T, N> operator op (const vec_fb<T, N>& a, const vec_fb<T, N>& b) \
  {                                                                       \
    vec_fb<T, N> c;                                                       \
    for (size_t i = 0; i < N; i++)                                        \
      c.m_arr[i] = a.m_arr[i] op b.m_arr[i];                              \
    return c;                                                             \
  }                                                                       \
  template <typename T, size_t N, typename U>                             \
  inline                                                                  \
  vec_fb<T, N> operator op (const vec_fb<T, N>& a, U b)                   \
  {                                                                       \
    vec_fb<T, N> c;                                                       \
    for (size_t i = 0; i < N; i++)                                        \
      c.m_arr[i] = a.m_arr[i] op b;                                       \
    return c;                                                             \
  }                                                                       \
  template <typename T, size_t N, typename U>                             \
  inline                                                                  \
  vec_fb<T, N> operator op (U a, const vec_fb<T, N>& b)                   \
  {                                                                       \
    vec_fb<T, N> c;                                                       \
    for (size_t i = 0; i < N; i++)                                        \
      c.m_arr[i] = a op b.m_arr[i];                                       \
    return c;                                                             \
  }                                                                       \
  template <typename T, size_t N>                                         \
  inline                                                                  \
  vec_fb<T, N>& operator op ## = (vec_fb<T, N>& a, const vec_fb<T, N>& b) \
  {                                                                       \
     for (size_t i = 0; i < N; i++)                                       \
      a.m_arr[i] op ## = b.m_arr[i];                                      \
    return a;                                                             \
  }                                                                       \
  template <typename T, size_t N, typename U>                             \
  inline                                                                  \
  vec_fb<T, N>& operator op ## = (vec_fb<T, N>& a, U b)                   \
  {                                                                       \
    for (size_t i = 0; i < N; i++)                                        \
      a.m_arr[i] op ## = b;                                               \
    return a;                                                             \
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


#define UNOP(op)                                                        \
  template <typename T, size_t N>                                       \
  inline                                                                \
  vec_fb<T, N> operator op (const vec_fb<T, N>& a)                      \
  {                                                                     \
    vec_fb<T, N> c;                                                     \
    for (size_t i = 0; i < N; i++)                                      \
      c.m_arr[i] = op a.m_arr[i];                                       \
    return c;                                                           \
  }                                                                     \


UNOP(-)
UNOP(~)


#undef UNOP


// Define relational operations.


#define RELOP(op)                                                       \
  template <typename T, size_t N>                                       \
  inline                                                                \
  ivec<T, N> operator op (const vec_fb<T, N>& a, const vec_fb<T, N>& b) \
  {                                                                     \
    ivec<T, N> c;                                                       \
    for (size_t i = 0; i < N; i++)                                      \
      c.m_arr[i] = a.m_arr[i] op b.m_arr[i];                            \
    return c;                                                           \
  }


RELOP(==)
RELOP(!=)
RELOP(<)
RELOP(<=)
RELOP(>)
RELOP(>=)

#undef RELOP


/// Negation.
template <typename T, size_t N>
inline
ivec<T, N> operator! (const vec_fb<T, N>& a)
{
  ivec<T, N> c;
  for (size_t i = 0; i < N; i++)
    c.m_arr[i] = a.m_arr[i] == 0;
  return c;
}


/// V1 && V2
template <typename T, size_t N>
inline
ivec<T, N> operator&& (const vec_fb<T, N>& a, const vec_fb<T, N>& b)
{
  ivec<T, N> c;
  for (size_t i = 0; i < N; i++)
    c.m_arr[i] = (a.m_arr[i]!=0) & (b.m_arr[i]!=0);
  return c;
}


/// S && V
template <typename T, size_t N, class U>
inline
ivec<T, N> operator&& (U a, const vec_fb<T, N>& b)
{
  ivec<T, N> c;
  for (size_t i = 0; i < N; i++)
    c.m_arr[i] = a ? b.m_arr[i] != 0 : 0;
  return c;
}


/// V && S
template <typename T, size_t N, class U>
inline
ivec<T, N> operator&& (const vec_fb<T, N>& a, U b)
{
  ivec<T, N> c;
  for (size_t i = 0; i < N; i++)
    c.m_arr[i] = (a.m_arr[i]!=0) & (b ? -1 : 0);
  return c;
}


/// V1 || V2
template <typename T, size_t N>
inline
ivec<T, N> operator|| (const vec_fb<T, N>& a, const vec_fb<T, N>& b)
{
  ivec<T, N> c;
  for (size_t i = 0; i < N; i++)
    c.m_arr[i] = (a.m_arr[i]!=0) | (b.m_arr[i]!=0);
  return c;
}

#endif // !HAVE_VECTOR_SIZE_ATTRIBUTE || WANT_VECTOR_FALLBACK



#if HAVE_VECTOR_SIZE_ATTRIBUTE

/// Define a nice alias for a built-in vectorized type.
template <typename T, size_t N>
using vec  __attribute__ ((vector_size(N*sizeof(T)))) = T;

#else

/// Define alias for the vectorized fallback type.
template <typename T, size_t N>
using vec = vec_fb<T, N>;

#endif



/**
 * @brief Deduce the element type from a vectorized type.
 */
template <class VEC>
struct vec_type
{
  // Requires c++20.
  //typedef typename std::invoke_result< decltype([](const VEC& v){return v[0];}), VEC& >::type type;

  // Works in c++17.
  static auto elt (const VEC& v) -> decltype( v[0] );
  typedef typename std::invoke_result< decltype(elt), const VEC& >::type type1;
  typedef std::remove_cv_t<std::remove_reference_t<type1> > type;
};

/// Deduce the element type from a vectorized type.
template<class VEC>
using vec_type_t = typename vec_type<VEC>::type;

/**
 * brief Deduce the type of a mask , type returned by relational operations,
 * for a vectorized type.
 */
template<class VEC>
struct mask_type
{
  static auto maskt(const VEC& v1, const VEC& v2) -> decltype(v1 < v2);
  typedef
    typename std::invoke_result<decltype(maskt), const VEC&, const VEC&>::type
      type1;
  typedef std::remove_cv_t<std::remove_reference_t<type1>> type;
};
/// Deduce the mask type for a vectorized type.
template<class VEC>
using mask_type_t = typename mask_type<VEC>::type;

/**
 * @brief Return the number of elements in a vectorized type.
 */
template<class VEC>
inline constexpr size_t
vec_size()
{
  typedef vec_type_t<VEC> ELT;
  return sizeof(VEC) / sizeof(ELT);
}

/**
 * @brief Return the number of elements in a vectorized type.
 */
template<class VEC>
inline constexpr size_t
vec_size(const VEC&)
{
  typedef vec_type_t<VEC> ELT;
  return sizeof(VEC) / sizeof(ELT);
}

/**
 * brief Copy a scalar to each element of a vectorized type.
 * Similar functionality to _mm_set/_mm_broadcast x86-64 intrinsics.
 */
template<typename VEC, typename T>
inline void
vbroadcast(VEC& v, T x)
{
#if !HAVE_VECTOR_SIZE_ATTRIBUTE || WANT_VECTOR_FALLBACK
  // This may look inefficient, but the loop goes away when we
  // compile with optimization.
  constexpr size_t N = CxxUtils::vec_size<VEC>();
  for (size_t i = 0; i < N; i++) {
    v[i] = x;
  }
#else
  // using  - to avoid sign conversions.
  // using + adds  extra instructions due to float arithmetic.
  v = x - VEC{ 0 };
#endif
}

/*
 * @brief load elements from  memory address src (C-array)
 * to a vectorized type dst.
 * Used memcpy to avoid alignment issues
 */
template<typename VEC>
inline void
vload(VEC& dst, vec_type_t<VEC> const* src)
{
  std::memcpy(&dst, src, sizeof(VEC));
}

/*
 * @brief store elements from a vectorized type src to
 * to a memory address dst (C-array).
 * Uses memcpy to avoid alignment issue
 */
template<typename VEC>
inline void
vstore(vec_type_t<VEC>* dst, const VEC& src)
{
  std::memcpy(dst, &src, sizeof(VEC));
}

/*
 * @brief select/blend function.
 * Fill dst according to
 * dst[i] = mask[i] ? a[i] : b[i]
 */
template<typename VEC>
inline void
vselect(VEC& dst, const VEC& a, const VEC& b, const mask_type_t<VEC>& mask)
{
#if !HAVE_VECTOR_TERNARY_OPERATOR || WANT_VECTOR_FALLBACK
  constexpr size_t N = vec_size<VEC>();
  for (size_t i = 0; i < N; i++) {
    dst[i] = mask[i] ? a[i] : b[i];
  }
#else
  dst = mask ? a : b;
#endif
}

/*
 * @brief vectorized min.
 * copies to @c dst[i]  the min(a[i],b[i])
 */
template<typename VEC>
inline void
vmin(VEC& dst, const VEC& a, const VEC& b)
{
#if !HAVE_VECTOR_TERNARY_OPERATOR || WANT_VECTOR_FALLBACK
  constexpr size_t N = vec_size<VEC>();
  for (size_t i = 0; i < N; i++) {
    dst[i] = a[i] < b[i] ? a[i] : b[i];
  }
#else
  dst = a < b ? a : b;
#endif
}

/*
 * @brief vectorized max.
 * copies to @c dst[i]  the max(a[i],b[i])
 */
template<typename VEC>
inline void
vmax(VEC& dst, const VEC& a, const VEC& b)
{
#if !HAVE_VECTOR_TERNARY_OPERATOR || WANT_VECTOR_FALLBACK
  constexpr size_t N = vec_size<VEC>();
  for (size_t i = 0; i < N; i++) {
    dst[i] = a[i] > b[i] ? a[i] : b[i];
  }
#else
  dst = a > b ? a : b;
#endif
}

/*
 * Need for static asserts on argument
 * packs
 */
namespace bool_pack_helper {
template<bool...>
struct bool_pack;
template<bool... bs>
using all_true = std::is_same<bool_pack<bs..., true>, bool_pack<true, bs...>>;
}

/**
 * @brief vpermute function.
 * move any element of a vector src
 * into any or multiple position inside dst.
 *
 * We try to wrap both 
 * gcc's __builtin_shuffle
 * and 
 * clang's __builtin_shufflevector
 *
 */
template<size_t... Indices, typename VEC>
inline void
vpermute(VEC& dst, const VEC& src)
{

  constexpr size_t N = vec_size<VEC>();
  static_assert((sizeof...(Indices) == N),
                "Number of indices different than vector size");
  static_assert(
    bool_pack_helper::all_true<(Indices >= 0 && Indices < N)...>::value,
    "permute indices outside allowed range");

#if !HAVE_VECTOR_SIZE_ATTRIBUTE || WANT_VECTOR_FALLBACK
  dst = VEC{ src[Indices]... };
#elif defined(__clang__)
  dst = __builtin_shufflevector(src, src, Indices...);
#else // gcc
  dst = __builtin_shuffle(src, mask_type_t<VEC>{ Indices... });
#endif
}

} // namespace CxxUtils

#endif // not CXXUTILS_VEC_H
