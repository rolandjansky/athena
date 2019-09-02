/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CxxUtils/test/vectorize_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2019
 * @brief Regression test for ATH_ENABLE_VECTORIZATION.  (Parse test only.)
 */

#undef NDEBUG
#include "CxxUtils/vectorize.h"
#include <iostream>


ATH_ENABLE_VECTORIZATION;


int main()
{
  std::cout << "CxxUtils/vectorize_test\n";
  return 0;
}
