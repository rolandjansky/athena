/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloCellPackerUtils_test.cxx,v 1.1 2007-11-08 18:14:23 ssnyder Exp $
/**
 * @file  CaloCellPackerUtils_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2007
 * @brief Component test for CaloCellPackerUtils.
 */

#undef NDEBUG


#include "../src/CaloCellPackerUtils.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>


void compare_float (float x1, float x2, float thresh=1e-9)
{
  float den = std::abs(x1) + std::abs(x2);
  if (den == 0) den = 1;
  if (std::abs(x1-x2) / den > thresh) {
    printf ("Float miscompare: %f %f\n", x1, x2);
    std::abort();
  }
}


void test1()
{
  CaloCellPackerUtils::Bitfield bf (0x00f0);
  assert (bf.in (5) == 0x0050);
  assert (bf.in (0x123) == 0x0030);
  assert (bf.out (0x1234) == 3);

  int underflow;
  CaloCellPackerUtils::Floatfield ff (0x00f0, 2, 4);
  assert (ff.in (2.5) == 0x0040);
  compare_float (ff.out (0x0040, underflow), 2.5625);
  assert (underflow == 0);
  assert (ff.in (0) == 0);
  compare_float (ff.out (0, underflow), 2);
  assert (underflow == 1);
  assert (ff.in (10) == 0x00f0);
  compare_float (ff.out (0x00f0, underflow), 3.9375);
  assert (underflow == 0);

  CaloCellPackerUtils::Floatfield2 ff2 (0x00f0, 4);
  assert (ff2.in (2.5) == 0x00a0);
  compare_float (ff2.out (0x00a0), 2.625);
  assert (ff2.in (0) == 0);
  compare_float (ff2.out (0), 0);
  assert (ff2.in (10) == 0x00f0);
  compare_float (ff2.out (0x00f0), 3.875);
}


int main()
{
  test1();
  return 0;
}


// This package builds a component library, so we don't normally
// link against the library.  So just include this here.
#include "../src/CaloCellPackerUtils.cxx"
