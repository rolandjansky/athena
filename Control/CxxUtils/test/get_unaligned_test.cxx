/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CxxUtils/test/get_unaligned_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2019
 * @brief Regression tests for get_unaligned functions.
 */

#undef NDEBUG
#include "CxxUtils/get_unaligned.h"
#include "CxxUtils/restrict.h"
#include <iostream>
#include <cassert>
#include <stdint.h>


void test1()
{
  std::cout << "test1\n";
  uint8_t arr[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
  const uint8_t* p = arr+1;
  assert (CxxUtils::get_unaligned16 (p) == 0x201);
  assert (p == arr+3);

  assert (CxxUtils::get_unaligned32 (p) == 0x6050403);
  assert (p == arr+7);

  const uint8_t* ATH_RESTRICT q = arr+1;
  assert (CxxUtils::get_unaligned64 (q) == 0x807060504030201);
  assert (q == arr+9);

  uint8_t arr_f[] = { 0, 0, 0, 72, 64 };
  const uint8_t* pf = arr_f+1;
  assert (CxxUtils::get_unaligned_float (pf) == 3.125);
  assert (pf == arr_f+5);

  uint8_t arr_d[] = { 0, 0, 0, 0, 0, 0, 0, 9, 64 };
  const uint8_t* pd = arr_d+1;
  assert (CxxUtils::get_unaligned_double (pd) == 3.125);
  assert (pd == arr_d+9);
}


int main()
{
  std::cout << "CxxUtils/get_unaligned_test\n";
  test1();
  return 0;
}
