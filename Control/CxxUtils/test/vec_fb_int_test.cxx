/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file CxxUtils/test/vec_fb_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2020
 * @brief Unit test for vec_fb for int types.
 */


#define WANT_VECTOR_FALLBACK 1
#include "vec_test_common.h"
#include "CxxUtils/vec.h"
#include <iostream>

void
test1()
{
  std::cout << "test1 vec_fb for int\n";
  testInt1<CxxUtils::vec_fb>();
}

int
main()
{
  std::cout << "CxxUtils/vec_test\n";
  test1();
  return 0;
}
