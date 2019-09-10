/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CxxUtils/test/restrict_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2019
 * @brief Regression test for ATH_RESTRICT.  (Parse test only.)
 */

#undef NDEBUG
#include "CxxUtils/restrict.h"
#include <iostream>


int foo (const int* ATH_RESTRICT p)
{
  return *p;
}


int main()
{
  std::cout << "CxxUtils/restrict_test\n";
  return 0;
}
