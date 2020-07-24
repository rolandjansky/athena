/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AthenaKernel/test/getThinningCache_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2019
 * @brief Regression tests for getThinningCache.
 */

#undef NDEBUG

#include "AthenaKernel/getThinningCache.h"
#include "AthenaKernel/ThinningDecisionBase.h"
#include "AthenaKernel/ThinningCache.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <cassert>
#include <iostream>
#include <stdexcept>


void test1()
{
  std::cout << "test1\n";

  EventContext ctx;

  assert (SG::getThinningCache() == nullptr);
  assert (SG::getThinningCache(ctx) == nullptr);
  assert (SG::getThinningInfo("foo") == nullptr);
  assert (SG::getThinningInfo(ctx, "foo") == nullptr);
  assert (SG::getThinningDecision("foo") == nullptr);
  assert (SG::getThinningDecision(ctx, "foo") == nullptr);
  assert (SG::getThinningDecision(101) == nullptr);
  assert (SG::getThinningDecision(ctx, 101) == nullptr);

  ctx.emplaceExtension<Atlas::ExtendedEventContext>();
  assert (SG::getThinningCache(ctx) == nullptr);

  SG::ThinningCache cache;

  ctx.getExtension<Atlas::ExtendedEventContext>().setThinningCache (&cache);
  assert (SG::getThinningCache() == nullptr);
  assert (SG::getThinningCache(ctx) == &cache);
  assert (SG::getThinningInfo("foo") == nullptr);
  assert (SG::getThinningInfo(ctx, "foo") == nullptr);
  assert (SG::getThinningDecision("foo") == nullptr);
  assert (SG::getThinningDecision(ctx, "foo") == nullptr);
  assert (SG::getThinningDecision(101) == nullptr);
  assert (SG::getThinningDecision(ctx, 101) == nullptr);

  SG::ThinningDecisionBase dec;
  cache.addThinning ("foo", {101}, &dec);

  assert (SG::getThinningCache() == nullptr);
  assert (SG::getThinningCache(ctx) == &cache);
  assert (SG::getThinningInfo("foo") == nullptr);
  assert (SG::getThinningInfo(ctx, "foo")->m_decision == &dec);
  assert (SG::getThinningDecision("foo") == nullptr);
  assert (SG::getThinningDecision(ctx, "foo") == &dec);
  assert (SG::getThinningDecision(101) == nullptr);
  assert (SG::getThinningDecision(ctx, 101) == &dec);

  Gaudi::Hive::setCurrentContext (ctx);

  assert (SG::getThinningCache() == &cache);
  assert (SG::getThinningCache(ctx) == &cache);
  assert (SG::getThinningInfo("foo")->m_decision == &dec);
  assert (SG::getThinningInfo(ctx, "foo")->m_decision == &dec);
  assert (SG::getThinningDecision("foo") == &dec);
  assert (SG::getThinningDecision(ctx, "foo") == &dec);
  assert (SG::getThinningDecision(101) == &dec);
  assert (SG::getThinningDecision(ctx, 101) == &dec);
}


int main()
{
  std::cout << "AthenaKernel/getThinningCache_test\n";
  test1();
  return 0;
}
