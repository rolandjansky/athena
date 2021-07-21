/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file EventTPCnv/test/AtlasMcWeight_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2016
 * @brief Regression tests.
 */

#undef NDEBUG
#include "EventTPCnv/AtlasMcWeight.h"
#include "TestTools/leakcheck.h"
#include "TestTools/FLOATassert.h"
#include "CxxUtils/checker_macros.h"
#include <cstdlib>
#include <cassert>
#include <iostream>


void testit (double w, unsigned int n, double w2)
{
  unsigned int nn = AtlasMcWeight::encode (w);
  if (nn != n) {
    std::cout << "encode for " << w << " fails; " << n << " " << nn << "\n";
    std::abort();
  }
  assert (nn == n);
  assert (Athena_test::isEqual (AtlasMcWeight::decode (nn), w2));
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;
  
  testit (123.5, 312350000, 123.5);
  testit (-123.5, 1312350000, -123.5);
  testit (0.125, 2012500000, 0.125);
  testit (-0.125, 3012500000, -0.125);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}
