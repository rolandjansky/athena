/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file SGTools/test/exceptions_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2014
 * @brief Regression tests for exceptions.
 */

#undef NDEBUG
#include "SGTools/exceptions.h"
#include <iostream>


void test1()
{
  std::cout << "test1\n";

  std::cout << SG::ExcBadDataProxyCast (123, typeid(int)).what() << "\n";

  std::cout << SG::ExcProxyCollision (123, "abc", 456, "def").what() << "\n";
}


int main()
{
  test1();
  return 0;
}
