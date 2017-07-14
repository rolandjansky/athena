/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CxxUtils/test/exctrace2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2009
 * @brief Test trace backs for caught exceptions --- without the collector.
 */


#include <cstdio>
#include <stdexcept>
#include "CxxUtils/exctrace.h"
#include "CxxUtils/checker_macros.h"

ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

void bar()
{
  throw std::logic_error ("yo mama");
}

void fee()
{
  bar();
}

void foo()
{
  try {
    fee();
  }
  catch (std::exception& e) {
    CxxUtils::exctrace (e);
  }
}


int main()
{
  foo();
  return 0;
}
