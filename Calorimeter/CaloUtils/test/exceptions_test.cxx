/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file CaloUtils/test/exceptions_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2020
 * @brief Regression tests for exceptions.
 */

#undef NDEBUG
#include "CaloUtils/exceptions.h"
#include <iostream>


void test1()
{
  std::cout << "test1\n";

  std::cout << CaloUtils::ExcBadToolConstantVersion ("tool", "const", 1, 2).what() << "\n";

  std::cout << CaloUtils::ExcConstantNotSet ("tool", "const").what() << "\n";

  std::cout << CaloUtils::ExcConstantReset ("tool", "const").what() << "\n";
  std::cout << CaloUtils::ExcBadContextlessRetrieve ("tool", "const").what() << "\n";
}


int main()
{
  std::cout << "CaloUtils/exceptions_test\n";
  test1();
  return 0;
}
