/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "TestTools/expect_exception.h"
#include <cassert>
#include <iostream>
#include <stdexcept>


void test1()
{
  std::cout << "test1\n";
  SG::ThinningCache cache;
  assert (cache.empty());
  assert (!cache.thinning ("foo"));

  SG::ThinningDecisionBase b1;
  cache.addThinning ("foo", std::vector<SG::sgkey_t> {101, 102}, &b1);

  SG::ThinningDecisionBase b2;
  cache.addThinning ("bar", std::vector<SG::sgkey_t> {201, 202}, &b2);

  assert (!cache.empty());
  assert (cache.thinning ("foo") == &b1);
  assert (cache.thinning (102) == &b1);
  assert (cache.thinning ("bar") == &b2);
  assert (cache.thinning (201) == &b2);

  EXPECT_EXCEPTION (std::runtime_error, cache.addThinning ("foo", std::vector<SG::sgkey_t> {101, 102}, &b1));

  cache.clear();
  assert (cache.empty());
}


int main()
{
  std::cout << "AthenaKernel/ThinningCache_test\n";
  test1();
  return 0;
}
