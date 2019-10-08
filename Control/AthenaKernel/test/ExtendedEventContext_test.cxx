/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AthenaKernel/test/ExtendedEventContext_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2019
 * @brief Regression tests for ExtendedEventContext (partial).
 */

#undef NDEBUG

#include "AthenaKernel/ExtendedEventContext.h"
#include "GaudiKernel/EventContext.h"
#include <cassert>
#include <iostream>
#include <stdexcept>


void test1()
{
  std::cout << "test1\n";

  EventContext ctx;

  Atlas::ExtendedEventContext ectx;

  assert (!Atlas::hasExtendedEventContext (ctx));
  ectx.setConditionsRun (42);
  Atlas::setExtendedEventContext (ctx, std::move (ectx));
  assert (Atlas::hasExtendedEventContext (ctx));
  assert (Atlas::getExtendedEventContext (ctx).conditionsRun() == 42);
}


int main()
{
  std::cout << "AthenaKernel/ExtendedEventContext_test\n";
  test1();
  return 0;
}
