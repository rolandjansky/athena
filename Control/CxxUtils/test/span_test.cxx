/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file CxxUtils/tests/span_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2022
 * @brief Unit tests for span.
 */


#undef NDEBUG
#include "CxxUtils/span.h"
#include "CxxUtils/reverse_wrapper.h"
#include <cassert>
#include <iostream>


template <class ELT, class T>
void test1a(T& s2)
{
  assert (s2.size() == 8);
  assert (s2.size_bytes() == 8*sizeof(float));
  assert (!s2.empty());
  assert (s2.front() == 0);
  assert (s2.back() == 7);
  assert (s2[3] == 3);
  assert (s2.data()[4] == 4);
  assert (*s2.begin() == 0);
  assert (s2.end() - s2.begin() == 8);
  assert (s2.begin() != s2.end());
  assert (s2.begin()+8 == s2.end());
  assert (*s2.rbegin() == 7);
  assert (s2.rend() - s2.rbegin() == 8);
  assert (s2.rbegin() != s2.rend());
  assert (s2.rbegin()+8 == s2.rend());
  float acc = 0;
  for (float f : s2) {
    acc = 2*acc + f;
  }
  assert (acc == 247);
  acc = 0;
  for (float f : CxxUtils::make_reverse_wrapper (s2)) {
    acc = 2*acc + f;
  }
  assert (acc == 1538);

  CxxUtils::span<ELT> s3 = s2.first (3);
  assert (s3.size() == 3);
  assert (s3[1] == 1);

  CxxUtils::span<ELT> s4 = s2.last (3);
  assert (s4.size() == 3);
  assert (s4[1] == 6);

  CxxUtils::span<ELT> s5 = s2.subspan (4);
  assert (s5.size() == 4);
  assert (s5[1] == 5);

  CxxUtils::span<ELT> s6 = s2.subspan (2, 3);
  assert (s6.size() == 3);
  assert (s6[1] == 3);
}


void test1()
{
  std::cout << "test1\n";
  CxxUtils::span<float> s1;
  assert (s1.size() == 0);
  assert (s1.size_bytes() == 0);
  assert (s1.empty());

  float ff[8] = {0, 1, 2, 3, 4, 5, 6, 7};
  CxxUtils::span<float> s2 (ff, 8);
  test1a<float> (s2);
  const CxxUtils::span<float>& cs2 = s2;
  test1a<const float> (cs2);

  CxxUtils::span<float> s3 = s2;
  test1a<float> (s3);
  s1 = s2;
  test1a<float> (s1);

  CxxUtils::span<float> s4 (ff, ff+8);
  test1a<float> (s4);
}


int main()
{
  std::cout << "CxxUtils/span_test.cxx\n";
  test1();
  return 0;
}
