/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AthenaKernel/test/ThinningCache_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2019
 * @brief Regression tests for ThinningCache.
 */

#undef NDEBUG

#include "AthenaKernel/ThinningCache.h"
#include "AthenaKernel/ThinningDecisionBase.h"
#include "AthenaKernel/ITrigNavigationThinningSvc.h"
#include "TestTools/expect_exception.h"
#include <cassert>
#include <iostream>
#include <stdexcept>


class TestTrigNavigationThinningSvc : public ITrigNavigationThinningSvc
{
public:
  virtual
  StatusCode doSlimming (const EventContext& /*ctx*/,
                         std::vector<uint32_t>& /*slimmed_and_serialized*/) const override
  {
    return StatusCode::SUCCESS;
  }
  virtual unsigned long addRef() override { return 0; }
  virtual unsigned long release() override { return 0; }
  virtual StatusCode queryInterface( const InterfaceID& , void** ) override
  {
    return StatusCode::FAILURE;
  }

};



void test1()
{
  std::cout << "test1\n";
  SG::ThinningCache cache;
  assert (cache.empty());
  assert (!cache.thinning ("foo"));

  SG::ThinningDecisionBase b1 (5);
  b1.thin (0);
  b1.thin (1);
  b1.thin (3);
  b1.buildIndexMap();
  cache.addThinning ("foo", std::vector<SG::sgkey_t> {101, 102}, &b1, true);

  SG::ThinningDecisionBase b2 (5);
  b2.thin (1);
  b2.thin (2);
  b2.thin (3);
  b2.buildIndexMap();
  cache.addThinning ("bar", std::vector<SG::sgkey_t> {201, 202}, &b2, true);

  assert (!cache.empty());
  assert (cache.thinning ("foo") == &b1);
  assert (cache.thinning (102) == &b1);
  assert (cache.thinning ("bar") == &b2);
  assert (cache.thinning (201) == &b2);

  EXPECT_EXCEPTION (std::runtime_error, cache.addThinning ("foo", std::vector<SG::sgkey_t> {101, 102}, &b1, true));

  cache.addThinning ("foo", std::vector<SG::sgkey_t> {101, 102}, &b2, false);
  const SG::ThinningDecisionBase* ba = cache.thinning ("foo");
  assert (ba != &b1);
  assert (ba != &b2);
  assert (cache.thinning (101) == ba);
  assert (cache.thinning (102) == ba);
  assert (!ba->thinned (0));
  assert ( ba->thinned (1));
  assert (!ba->thinned (2));
  assert ( ba->thinned (3));
  assert (!ba->thinned (4));

  SG::ThinningDecisionBase b3 (5);
  b3.thin (2);
  b3.thin (3);
  b3.thin (4);
  b3.buildIndexMap();

  cache.addThinning ("foo", std::vector<SG::sgkey_t> {101, 102}, &b3, false);
  cache.lockOwned();
  ba = cache.thinning ("foo");
  assert (ba != &b1);
  assert (ba != &b2);
  assert (cache.thinning (101) == ba);
  assert (cache.thinning (102) == ba);
  assert (!ba->thinned (0));
  assert (!ba->thinned (1));
  assert (!ba->thinned (2));
  assert ( ba->thinned (3));
  assert (!ba->thinned (4));

  assert (ba->index (1) == 1);
  assert (ba->index (3) == SG::ThinningDecisionBase::RemovedIdx);
  assert (ba->index (4) == 3);

  assert (cache.trigNavigationThinningSvc() == nullptr);
  TestTrigNavigationThinningSvc tt;
  cache.setTrigNavigationThinningSvc (&tt);
  assert (cache.trigNavigationThinningSvc() == &tt);

  cache.clear();
  assert (cache.empty());
  assert (cache.trigNavigationThinningSvc() == nullptr);
}


int main()
{
  std::cout << "AthenaKernel/ThinningCache_test\n";
  test1();
  return 0;
}
