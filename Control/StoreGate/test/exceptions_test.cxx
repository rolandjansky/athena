/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file StoreGate/test/exceptions_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Regression tests for exceptions.
 */


#undef NDEBUG
#include "StoreGate/exceptions.h"
#include <iostream>


void test1()
{
  std::cout << "test1\n";

  std::cout << SG::ExcNullHandleKey().what() << "\n";
  std::cout << SG::ExcBadHandleKey("xkey").what() << "\n";
  std::cout << SG::ExcForbiddenMethod("meth").what() << "\n";
  std::cout << SG::ExcHandleInitError(123, "foo", "FooSvc").what() << "\n";
  std::cout << SG::ExcUninitKey (123, "foo", "FooSvc").what() << "\n";
  std::cout << SG::ExcConstObject(123, "foo", "FooSvc").what() << "\n";
  std::cout << SG::ExcNullWriteHandle(123, "foo", "FooSvc").what() << "\n";
  std::cout << SG::ExcNullReadHandle(123, "foo", "FooSvc").what() << "\n";
  std::cout << SG::ExcNullUpdateHandle(123, "foo", "FooSvc").what() << "\n";
  std::cout << SG::ExcUpdatedObjectFailure (123, "foo", "FooSvc").what() << "\n";
  std::cout << SG::ExcNonConstHandleKey (123, "foo", "FooSvc").what() << "\n";
}


int main()
{
  test1();
  return 0;
}
