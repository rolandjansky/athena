/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthAllocators/test/exceptions_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2016
 * @brief Regression tests for exceptions.
 */


#undef NDEBUG
#include "AthAllocators/exceptions.h"
#include <iostream>


void test1()
{
  std::cout << "test1\n";

  std::cout << SG::ExcDifferentArenas().what() << "\n";
}


int main()
{
  test1();
  return 0;
}
