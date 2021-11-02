/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file CxxUtils/test/LockedPointer_test.cxx
 * @author scott snyder
 * @date Sep 2021
 * @brief Regression tests for LockedPointer.
 */


#undef NDEBUG
#include "CxxUtils/LockedPointer.h"
#include <mutex>
#include <iostream>
#include <cassert>


void test1a (CxxUtils::LockedPointer<int> p)
{
  assert (*p == 42);
}

void test1()
{
  std::cout << "test1\n";

  std::recursive_mutex m;
  int x = 42;

  {
    std::unique_lock l (m);
    CxxUtils::LockedPointer<int> p (x, std::move(l));
    assert (*p == 42);
    assert (*p.get() == 42);
    test1a (std::move (p));
  }
}


int main()
{
  std::cout << "CxxUtils/LockedPointer_test\n";
  test1();
  return 0;
}
