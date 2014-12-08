/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CxxUtils/test/ones_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2014
 * @brief Regression test for ones.
 */


#undef NDEBUG
#include "CxxUtils/ones.h"
#include <iostream>
#include <cassert>
#include <stdint.h>


template <class T>
inline
void testit (unsigned int n)
{
  T mask = CxxUtils::ones<T>(n);
  T comp = 0;
  for (unsigned int i=0; i < n; i++)
    comp = (comp<<1) | 1;
  assert (mask == comp);
}


void test1()
{
  std::cout << "test1\n";
  for (int i=0; i<=32; i++)
    testit<uint32_t> (i);

  for (int i=0; i<=64; i++)
    testit<uint64_t> (i);
}


int main()
{
  test1();
  return 0;
}

