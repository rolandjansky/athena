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

  assert (cache.thinningInfo ("foo")->m_vetoed.empty());
  CxxUtils::ConcurrentBitset bs (64);
  bs.set (13);
  cache.setVetoed ("foo", bs);
  assert (cache.thinningInfo ("foo")->m_vetoed == bs);
  assert (cache.thinningInfo ("foo")->vetoed(13));
  assert (!cache.thinningInfo ("foo")->vetoed(14));

  assert (cache.thinningInfo ("foo")->m_compression.empty());
  SG::ThinningInfo::compression_map_t compression1;
  CxxUtils::ConcurrentBitset cbs1 (64);
  cbs1.set (13);
  compression1[7] = cbs1;
  cache.setCompression("foo", compression1);
  assert (cache.thinningInfo ("foo")->m_compression == compression1);
  assert (cache.thinningInfo ("foo")->compression(13) == 7);
  assert (cache.thinningInfo ("foo")->compression(14) == 0);

  assert (cache.thinningInfo ("bar")->m_compression.empty());
  SG::ThinningInfo::compression_map_t compression2;
  CxxUtils::ConcurrentBitset cbs2 (42);
  cbs2.set (8);
  compression2[7] = cbs2;
  compression2[15] = cbs2;
  cache.setCompression("bar", compression2);
  assert (cache.thinningInfo ("bar")->m_compression == compression2);
  assert (cache.thinningInfo ("bar")->compression(8) == 7);
  assert (cache.thinningInfo ("bar")->compression(8) != 15);

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
