/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthLinks/test/exceptions_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2014
 * @brief Regression tests for exceptions.
 */


#undef NDEBUG
#include "AthLinks/exceptions.h"
#include <iostream>


void test1()
{
  std::cout << "test1\n";

  std::cout << SG::ExcPointerNotInSG((char*)0x1234).what() << "\n";
  std::cout << SG::ExcCLIDMismatch(123, 456).what() << "\n";
  std::cout << SG::ExcInvalidLink (123, "key", 765).what() << "\n";
  std::cout << SG::ExcBadForwardLink (123, 345).what() << "\n";
  std::cout << SG::ExcElementNotFound ("test").what() << "\n";
  std::cout << SG::ExcInvalidIndex ("test").what() << "\n";
  std::cout << SG::ExcIndexNotFound ("test").what() << "\n";
  std::cout << SG::ExcIncomparableEL().what() << "\n";
}


int main()
{
  test1();
  return 0;
}
