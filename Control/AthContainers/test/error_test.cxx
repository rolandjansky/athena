/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/test/error_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2013
 * @brief Regression tests for error.
 */


#undef NDEBUG
#include "AthContainers/tools/error.h"


void test1()
{
  ATHCONTAINERS_ERROR("context", "message");
  ATHCONTAINERS_ERROR("context2",
                      AthContainers_detail::typeinfoName (typeid(int)));
}


int main()
{
  test1();
  return 0;
}
