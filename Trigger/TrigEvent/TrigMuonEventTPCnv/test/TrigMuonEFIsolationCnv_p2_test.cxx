/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrigMuonEventTPCnv/test/TrigMuonEFIsolationCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for TrigMuonEFIsolationCnv_p2.
 */


#undef NDEBUG
#include "TrigMuonEventTPCnv/TrigMuonEFIsolationCnv_p2.h"
#include "SGTools/TestStore.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <cassert>
#include <iostream>


void compare (const TrigMuonEFIsolation& p1,
              const TrigMuonEFIsolation& p2)
{
  assert (p1.sumTrkPtCone02() == p2.sumTrkPtCone02());
  assert (p1.sumTrkPtCone03() == p2.sumTrkPtCone03());
  assert (p1.sumEtCone01() == p2.sumEtCone01());
  assert (p1.sumEtCone02() == p2.sumEtCone02());
  assert (p1.sumEtCone03() == p2.sumEtCone03());
  assert (p1.sumEtCone04() == p2.sumEtCone04());
  assert (p1.trackPosition() == p2.trackPosition());
  assert (p1.getMuonInfoLink() == p2.getMuonInfoLink());
}


void testit (const TrigMuonEFIsolation& trans1)
{
  MsgStream log (0, "test");
  TrigMuonEFIsolationCnv_p2 cnv;
  TrigMuonEFIsolation_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigMuonEFIsolation trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  (void)Gaudi::Hive::currentContext();
  // Get proxy created outside of leak checking.
  ElementLink<TrigMuonEFInfoContainer> foo ("foo", 10);
  Athena_test::Leakcheck check;

  TrigMuonEFIsolation trans1;
  trans1.setSumTrkPtCone02 (1.5);
  trans1.setSumTrkPtCone03 (2.5);
  trans1.setSumEtCone01 (3.5);
  trans1.setSumEtCone02 (4.5);
  trans1.setSumEtCone03 (5.5);
  trans1.setSumEtCone04 (6.5);
  trans1.setEFMuonInfoTrackLink (7);
  trans1.setEFMuonInfoElementLink(ElementLink<TrigMuonEFInfoContainer> ("foo", 10));
    
  testit (trans1);
}


int main()
{
  SGTest::initTestStore();
  test1();
  return 0;
}
