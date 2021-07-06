/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrigMuonEventTPCnv/test/CombinedMuonFeatureCnv_p4_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for CombinedMuonFeatureCnv_p4.
 */


#undef NDEBUG
#include "TrigMuonEventTPCnv/CombinedMuonFeatureCnv_p4.h"
#include "SGTools/TestStore.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <cassert>
#include <iostream>
#include <sstream>


void compare (const CombinedMuonFeature& p1,
              const CombinedMuonFeature& p2)
{
  assert (p1.pt() == p2.pt());
  assert (p1.charge() == p2.charge());
  assert (p1.ptq() == p2.ptq());
  assert (p1.sigma_pt_raw() == p2.sigma_pt_raw());
  assert (p1.getFlag() == p2.getFlag());
  assert (p1.muFastTrackLink() == p2.muFastTrackLink());
  assert (p1.IDTrackLink() == p2.IDTrackLink());
}


void testit (const CombinedMuonFeature& trans1)
{
  MsgStream log (0, "test");
  CombinedMuonFeatureCnv_p4 cnv;
  CombinedMuonFeature_p4 pers;
  cnv.transToPers (&trans1, &pers, log);
  CombinedMuonFeature trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  (void)Gaudi::Hive::currentContext();
  // Get proxies created outside of leak checking.
  ElementLink<MuonFeatureContainer> foo ("foo", 1);
  ElementLink<TrigInDetTrackCollection> bar ("bar", 2);
  Athena_test::Leakcheck check;

  CombinedMuonFeature trans1 (80000, 8000, 800, 3, 4, 5,
                              ElementLink<MuonFeatureContainer> ("foo", 1),
                              ElementLink<TrigInDetTrackCollection> ("bar", 2));
    
  testit (trans1);
}


int main()
{
  SGTest::initTestStore();
  test1();
  return 0;
}
