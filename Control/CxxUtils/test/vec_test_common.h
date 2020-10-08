/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file CxxUtils/test/vec_test_common.h
 * @author scott snyder <snyder@bnl.gov>
 * @author Christos Anastopoulos 
 * @date Feb, 2020
 * @brief Common part for vec vec_fb unit tests.
 */

#ifndef CXXUTILS_VEC_TEST_COMMON
#define CXXUTILS_VEC_TEST_COMMON
#undef NDEBUG

#include "CxxUtils/no_sanitize_undefined.h"
#include "CxxUtils/vec.h"
#include "boost/preprocessor/list/first_n.hpp"
#include "boost/preprocessor/list/for_each.hpp"
#include "boost/preprocessor/variadic/to_list.hpp"
#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <type_traits>
#include <typeinfo>

#include <array>
#include <valarray>

template<class VEC, class T>
void
check(const VEC& v, const std::valarray<T>& a)
{
  const size_t N = CxxUtils::vec_size<VEC>();
  assert(a.size() == N);
  for (size_t i = 0; i < N; i++) {
    if (v[i] != a[i]) {
      std::cerr << "Mismatch " << typeid(VEC).name() << " "
                << typeid(std::valarray<T>).name() << " " << i << "\n";
      for (size_t j = 0; j < N; j++) {
        std::cerr << v[j] << " " << a[j] << "\n";
      }
      std::abort();
    }
  }
}

template<class VEC>
void
check(const VEC& v, const std::valarray<bool>& a)
{
  const size_t N = CxxUtils::vec_size<VEC>();
  assert(a.size() == N);
  for (size_t i = 0; i < N; i++) {
    assert(bool(v[i]) == a[i]);
  }
}

// Disable ubsan because we get some overflows here.
template<class VEC>
void test_arith
NO_SANITIZE_UNDEFINED(const VEC& v1)
{
  using T = CxxUtils::vec_type_t<VEC>;
  size_t N = CxxUtils::vec_size<VEC>();

  std::valarray<T> a1(N);
  for (size_t i = 0; i < N; i++)
    a1[i] = v1[i];

  check(v1, a1);

  VEC v2 = v1 * 2;
  std::valarray<T> a2 = a1 * static_cast<T>(2);
  check(v2, a2);

#define TEST(op)                                                               \
  do {                                                                         \
    VEC v3 = v2 op v1;                                                         \
    std::valarray<T> a3 = a2 op a1;                                            \
    check(v3, a3);                                                             \
                                                                               \
    VEC v4 = v3 op 5;                                                          \
    std::valarray<T> a4 = a3 op static_cast<T>(5);                             \
    check(v4, a4);                                                             \
                                                                               \
    v4 = 6 op v3;                                                              \
    a4 = static_cast<T>(6) op a3;                                              \
    check(v4, a4);                                                             \
                                                                               \
    v4 op## = v1;                                                              \
    a4 op## = a1;                                                              \
    check(v4, a4);                                                             \
                                                                               \
    v4 op## = 10;                                                              \
    a4 op## = static_cast<T>(10);                                              \
    check(v4, a4);                                                             \
  } while (0)

  TEST(+);
  TEST(-);
  TEST(*);
  TEST(/);

#undef TEST

  VEC v3 = -v1;
  std::valarray<T> a3 = -a1;
  check(v3, a3);
}

// Disable ubsan because we get some overflows here.
template<class VEC>
void test_int
NO_SANITIZE_UNDEFINED(const VEC& v1)
{
  using T = CxxUtils::vec_type_t<VEC>;
  size_t N = CxxUtils::vec_size<VEC>();

  std::valarray<T> a1(N);
  for (size_t i = 0; i < N; i++)
    a1[i] = v1[i];

  check(v1, a1);

  VEC v2 = v1 * 3;
  std::valarray<T> a2 = a1 * static_cast<T>(3);
  check(v2, a2);

#define TEST(op, rhs)                                                          \
  do {                                                                         \
    VEC v3 = v1 op rhs(v2);                                                    \
    std::valarray<T> a3 = a1 op rhs(a2);                                       \
    check(v3, a3);                                                             \
                                                                               \
    VEC v4 = v3 op rhs(5);                                                     \
    std::valarray<T> a4 = a3 op static_cast<T>(rhs(5));                        \
    check(v4, a4);                                                             \
                                                                               \
    v4 = 6 op rhs(v3);                                                         \
    a4 = static_cast<T>(6) op rhs(a3);                                         \
    check(v4, a4);                                                             \
                                                                               \
    v4 op## = rhs(v1);                                                         \
    a4 op## = rhs(a1);                                                         \
    check(v4, a4);                                                             \
                                                                               \
    v4 op## = rhs(3);                                                          \
    a4 op## = static_cast<T>(rhs(3));                                          \
    check(v4, a4);                                                             \
  } while (0)

#define _(x) (x)
// Ensure shift count is within defined range.
#define MOD(x)                                                                 \
  ((x & std::numeric_limits<T>::max()) %                                       \
   static_cast<T>(sizeof(T) * 8 - (std::is_signed_v<T> ? 1 : 0)))

  TEST(^, _);
  TEST(|, _);
  TEST(&, _);
  TEST(%, _);
  if (!std::is_signed_v<T>) {
    // cppcheck-suppress compareBoolExpressionWithInt  // false positive
    TEST(<<, MOD);
  }
  // cppcheck-suppress compareBoolExpressionWithInt  // false positive
  TEST(>>, MOD);

#undef TEST

  VEC v3 = ~v1;
  std::valarray<T> a3 = ~a1;
  check(v3, a3);
}

template<class VEC>
void
test_relops(const VEC& v1)
{
  using T = CxxUtils::vec_type_t<VEC>;
  size_t N = CxxUtils::vec_size<VEC>();

  std::valarray<T> a1(N);
  for (size_t i = 0; i < N; i++)
    a1[i] = v1[i];

  VEC v2;
  v2[0] = v1[0];
  for (size_t i = 1; i < N; i++)
    v2[i] = v1[N - i];

  std::valarray<T> a2(N);
  for (size_t i = 0; i < N; i++)
    a2[i] = v2[i];

#define TEST(op)                                                               \
  do {                                                                         \
    auto v3 = (v1 op v2);                                                      \
    std::valarray<bool> a3 = (a1 op a2);                                       \
    check(v3, a3);                                                             \
  } while (0)

  TEST(==);
  TEST(!=);
  TEST(>);
  TEST(>=);
  TEST(<);
  TEST(<=);

#undef TEST
}

template<class VEC>
void
test_logops(const VEC& v1)
{
  using T = CxxUtils::vec_type_t<VEC>;
  size_t N = CxxUtils::vec_size<VEC>();

  std::valarray<T> a1(N);
  for (size_t i = 0; i < N; i++)
    a1[i] = v1[i];

  VEC v2;
  for (size_t i = 0; i < N; i++)
    v2[i] = v1[N - 1 - i];

  std::valarray<T> a2(N);
  for (size_t i = 0; i < N; i++)
    a2[i] = v2[i];

  {
    auto v3 = v1 && v2;
    std::valarray<bool> a3 = a1 && a2;
    check(v3, a3);

    auto v4a = v1 && 1;
    std::valarray<bool> a4a = a1 && static_cast<T>(1);
    check(v4a, a4a);

    auto v4b = v1 && 0;
    std::valarray<bool> a4b = a1 && static_cast<T>(0);
    check(v4b, a4b);

    auto v5a = v1 && 1;
    std::valarray<bool> a5a = static_cast<T>(1) && a1;
    check(v5a, a5a);

    auto v5b = v1 && 0;
    std::valarray<bool> a5b = static_cast<T>(0) && a1;
    check(v5b, a5b);
  }

  {
    auto v3 = v1 || v2;
    std::valarray<bool> a3 = a1 || a2;
    check(v3, a3);
  }

// clang doesn't implement this.
#ifndef __clang__
  {
    auto v3 = !v1;
    std::valarray<bool> a3 = !a1;
    check(v3, a3);
  }
#endif
}

template<class VEC>
void
test_broadcast(const VEC& v1)
{
  VEC v2;
  CxxUtils::vbroadcast(v2, v1[0]);

  size_t N = CxxUtils::vec_size<VEC>();
  for (size_t i = 0; i < N; i++) {
    assert(v1[i] == v2[i]);
  }
}

template<class VEC>
void
test_storeload(const VEC& v1)
{

  std::array<CxxUtils::vec_type_t<VEC>, CxxUtils::vec_size<VEC>()> buffer{};
  CxxUtils::vec_type_t<VEC>* mem_addr = buffer.data();

  CxxUtils::vstore(mem_addr, v1);
  size_t N = CxxUtils::vec_size<VEC>();
  for (size_t i = 0; i < N; i++) {
    assert(v1[i] == mem_addr[i]);
  }

  VEC v2;
  CxxUtils::vload(v2, mem_addr);
  for (size_t i = 0; i < N; i++) {
    assert(v2[i] == mem_addr[i]);
  }
}

template<class VEC>
void
test_min(const VEC& v1)
{

  const VEC v2 = v1 + 1;
  VEC min;
  CxxUtils::vmin(min, v1, v2);
  constexpr size_t N = CxxUtils::vec_size<VEC>();
  for (size_t i = 0; i < N; i++) {
    assert(min[i] == v1[i]);
  }
}

template<class VEC>
void
test_max(const VEC& v1)
{

  const VEC v2 = v1 + 1;
  VEC max;
  CxxUtils::vmax(max, v1, v2);
  constexpr size_t N = CxxUtils::vec_size<VEC>();
  for (size_t i = 0; i < N; i++) {
    assert(max[i] == v2[i]);
  }
}

template<class VEC>
void
test_select(const VEC& v1)
{

  const VEC v2 = 2 * v1;
  CxxUtils::mask_type_t<VEC> greater;
  CxxUtils::mask_type_t<VEC> less;
  constexpr size_t N = CxxUtils::vec_size<VEC>();
  for (size_t i = 0; i < N; i++) {
    greater[i] = v1[i] > v2[i];
    less[i] = v1[i] < v2[i];
  }

  VEC selectGreater;
  CxxUtils::vselect(selectGreater, v1, v2, greater);
  VEC selectLess;
  CxxUtils::vselect(selectLess, v1, v2, less);

  for (size_t i = 0; i < N; i++) {
    assert(selectGreater[i] == v2[i]);
    assert(selectLess[i] == v1[i]);
  }
}

template<class VEC>
void
test_permute(const VEC& v1)
{
  VEC v2;
  // invert elements
  constexpr size_t N = CxxUtils::vec_size<VEC>();
  if constexpr (N == 2) {
    CxxUtils::vpermute<1, 0>(v2, v1);
  } else if constexpr (N == 4) {
    CxxUtils::vpermute<3, 2, 1, 0>(v2, v1);
  } else if constexpr (N == 8) {
    CxxUtils::vpermute<7, 6, 5, 4, 3, 2, 1, 0>(v2, v1);
  } else {
    // N==16
    CxxUtils::vpermute<15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0>(
      v2, v1);
  }
  for (size_t i = 0; i < N; i++) {
    assert(v2[i] == v1[(N-1)-i]);
  }
}
template<class VEC>
void
test_blend(const VEC& v1)
{
  // Add 10 to V1 (so as to be different)
  VEC v2 = v1 + 10;
  VEC v3;
  // take the bottom half of v1 and v2 and put them together
  constexpr size_t N = CxxUtils::vec_size<VEC>();
  if constexpr (N == 2) {
    CxxUtils::vblend<0, 2>(v3, v1, v2);
  } else if constexpr (N == 4) {
    CxxUtils::vblend<0, 1, 4, 5>(v3, v1, v2);
  } else if constexpr (N == 8) {
    CxxUtils::vblend<0, 1, 2, 3, 8, 9, 10, 11>(v3, v1, v2);
  } else {
    // N==16
    CxxUtils::vblend<0, 1, 2, 3, 4, 5, 6, 7, 16, 17, 18, 19, 20, 21, 22, 23>(
      v3, v1, v2);
  }
  for (size_t i = 0; i < N; i++) {
    if (i < N / 2) {
      assert(v3[i] == v1[i]);
    } else {
      assert(v3[i] == v2[i - N/2]);
    }
  }
}

template<template<class T, size_t N> class VEC>
void
testFloat1()
{
  static_assert(std::is_same_v<CxxUtils::vec_type_t<VEC<float, 4>>, float>);
  VEC<float, 4> v;
  assert((CxxUtils::vec_size<VEC<float, 4>>()) == 4);
  assert(CxxUtils::vec_size(v) == 4);

#define ELT(r, data, elem) elem,
#define INITN(N, ...)                                                          \
  {                                                                            \
    BOOST_PP_LIST_FOR_EACH(                                                    \
      ELT,                                                                     \
      _,                                                                       \
      BOOST_PP_LIST_FIRST_N(N, BOOST_PP_VARIADIC_TO_LIST(__VA_ARGS__)))        \
  }

#define TEST_FLOAT(T, N)                                                       \
  do {                                                                         \
    test_arith(VEC<T, N> INITN(N, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5, 8.5));    \
    test_relops(VEC<T, N> INITN(N, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5, 8.5));   \
    test_broadcast(                                                            \
      VEC<T, N> INITN(N, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0));             \
    test_storeload(                                                            \
      VEC<T, N> INITN(N, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5, 8.5));             \
    test_select(VEC<T, N> INITN(N, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5, 8.5));   \
    test_min(VEC<T, N> INITN(N, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5, 8.5));      \
    test_max(VEC<T, N> INITN(N, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5, 8.5));      \
    test_permute(VEC<T, N> INITN(N, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5, 8.5));  \
    test_blend(VEC<T, N> INITN(N, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5, 8.5));    \
  } while (0)

  TEST_FLOAT(float, 4); // 128 bit wide 4 floats
  TEST_FLOAT(float, 8); // 256 bit wide 8 floats

  TEST_FLOAT(double, 2); // 128 bit wide 2 double
  TEST_FLOAT(double, 4); // 256 bit wide 4 double

#undef TEST_FLOAT
#undef INITN
#undef ELT
}
template<template<class T, size_t N> class VEC>
void
testInt1()
{
  static_assert(std::is_same_v<CxxUtils::vec_type_t<VEC<int, 4>>, int>);
  VEC<int, 4> v;
  assert((CxxUtils::vec_size<VEC<int, 4>>()) == 4);
  assert(CxxUtils::vec_size(v) == 4);

#define ELT(r, data, elem) elem,
#define INITN(N, ...)                                                          \
  {                                                                            \
    BOOST_PP_LIST_FOR_EACH(                                                    \
      ELT,                                                                     \
      _,                                                                       \
      BOOST_PP_LIST_FIRST_N(N, BOOST_PP_VARIADIC_TO_LIST(__VA_ARGS__)))        \
  }

#define TEST_INT(T, N)                                                         \
  do {                                                                         \
    test_arith(VEC<T, N> INITN(                                                \
      N, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16));              \
    test_relops(VEC<T, N> INITN(                                               \
      N, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16));              \
    test_broadcast(                                                            \
      VEC<T, N> INITN(N, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));     \
    test_storeload(VEC<T, N> INITN(                                            \
      N, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16));              \
    test_select(VEC<T, N> INITN(                                               \
      N, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16));              \
    test_min(VEC<T, N> INITN(                                                  \
      N, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16));              \
    test_max(VEC<T, N> INITN(                                                  \
      N, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16));              \
    test_permute(VEC<T, N> INITN(                                              \
      N, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16));              \
    test_blend(VEC<T, N> INITN(                                                \
      N, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16));              \
    test_int(VEC<T, N> INITN(                                                  \
      N, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16));              \
    test_logops(                                                               \
      VEC<T, N> INITN(N, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 1));     \
  } while (0)

  TEST_INT(int8_t, 16); // 128 bit wide

  TEST_INT(uint8_t, 16); // 128 bit wide

  TEST_INT(int16_t, 8);  // 128 bit wide
  TEST_INT(int16_t, 16); // 256 bit wide

  TEST_INT(uint16_t, 8);  // 128 bit wide
  TEST_INT(uint16_t, 16); // 256 bit wide

  TEST_INT(int32_t, 4); // 128 bit wide
  TEST_INT(int32_t, 8); // 256 bit wide

  TEST_INT(uint32_t, 4); // 128 bit wide
  TEST_INT(uint32_t, 8); // 256 bit wide

  TEST_INT(int64_t, 2); // 128 bit wide
  TEST_INT(int64_t, 4); // 256 bit wide

  TEST_INT(uint64_t, 2); // 128 bit wide
  TEST_INT(uint64_t, 4); // 256 bit wide

#undef TEST_INT
#undef INITN
#undef ELT
}

#endif
