/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CxxUtils/test/make_unique.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2014
 * @brief Regression tests for make_unique.
 */


#undef NDEBUG

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

#include "CxxUtils/make_unique.h"
#include <memory>
#include <iostream>
#include <cassert>


int count = 0;
struct C
{
  C(int x) : m_x (x) { count += x; }
  ~C() { count -= m_x; }
  int m_x;
};


void test1()
{
  std::cout << "test1\n";
#if __cplusplus > 201100
  {
    assert (count == 0);
    std::unique_ptr<C> c = CxxUtils::make_unique<C>(10);
    assert (count == 10);
  }
    assert (count == 0);
#endif
}


int main()
{
  test1();
  return 0;
}
