/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file CxxUtils/test/vec_int_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2020
 * @brief Unit test for vec for int types.
 */


#include "vec_test_common.h"
#include "CxxUtils/vec.h"
#include <iostream>
void test1()
{
  using CxxUtils::vec;
  std::cout << "test1 vec for int\n";
  testInt1<CxxUtils::vec>();
}


int main()
{
  std::cout << "CxxUtils/vec_test\n";
  test1();
  return 0;
}
