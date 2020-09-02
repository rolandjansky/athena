/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AthenaKernel/test/ThinningInfo_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2020
 * @brief Regression tests for ThinningInfo.
 */

#undef NDEBUG

#include "AthenaKernel/ThinningInfo.h"
#include <cassert>
#include <iostream>
#include <stdexcept>


void test1()
{
  std::cout << "test1\n";
}


int main()
{
  std::cout << "AthenaKernel/ThinningInfo_test\n";
  test1();
  return 0;
}
