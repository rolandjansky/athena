/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrigParticleTPCnv/test/TrigElectronCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for TrigElectronCnv_p2.
 */


#undef NDEBUG
#include "TrigParticleTPCnv/TrigElectronCnv_p2.h"
#include "TrigParticleTPCnv/TrigElectronContainerCnv_tlp2.h"
#include "SGTools/TestStore.h"
#include "TestTools/FLOATassert.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <cassert>
#include <iostream>


void compare (const P4PtEtaPhiM& p1,
              const P4PtEtaPhiM& p2)
{
  assert (Athena_test::isEqual (p1.pt(), p2.pt()));
  assert (Athena_test::isEqual (p1.eta(), p2.eta()));
  assert (Athena_test::isEqual (p1.phi(), p2.phi()));
  assert (Athena_test::isEqual (p1.m(), p2.m()));
}


void compare (const TrigElectron& p1,
              const TrigElectron& p2)
{
  compare (static_cast<const P4PtEtaPhiM&>(p1),
           static_cast<const P4PtEtaPhiM&>(p2));

  assert (p1.roiId() == p2.roiId());
  assert (p1.isValid() == p2.isValid());
  assert (p1.trackAlgo() == p2.trackAlgo());
  assert (p1.Zvtx() == p2.Zvtx());
  assert (p1.nTRTHits() == p2.nTRTHits());
  assert (p1.nTRTHiThresholdHits() == p2.nTRTHiThresholdHits());
  assert (p1.trkEtaAtCalo() == p2.trkEtaAtCalo());
  assert (p1.trkPhiAtCalo() == p2.trkPhiAtCalo());
  assert (p1.EtOverPt() == p2.EtOverPt());
  assert (p1.caloEta() == p2.caloEta());
  assert (p1.caloPhi() == p2.caloPhi());
  assert (p1.Rcore() == p2.Rcore());
  assert (p1.Eratio() == p2.Eratio());
  assert (p1.EThad() == p2.EThad());
  assert (p1.F0() == p2.F0());
  assert (p1.F1() == p2.F1());
  assert (p1.F2() == p2.F2());
  assert (p1.F3() == p2.F3());
  assert (p1.clusterLink() == p2.clusterLink());
  assert (p1.trackLink() == p2.trackLink());
}


void testit (const TrigElectron& trans1)
{
  MsgStream log (0, "test");
  TrigElectronCnv_p2 cnv;
  TrigElectronContainerCnv_tlp2 tlcnv;
  cnv.setRuntimeTopConverter (&tlcnv);
  TrigElectron_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigElectron trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  (void)Gaudi::Hive::currentContext();
  // Get proxies created outside of leak check.
  ElementLink< TrigEMClusterContainer > foo ("foo", 10);
  ElementLink< TrigInDetTrackCollection > bar ("bar", 20);
  Athena_test::Leakcheck check;

  TrigElectron trans1 (80000, 1.5, 2.5,
                       123, //roi
                       true,
                       3.5, 4.5, 5.5,
                       ElementLink< TrigEMClusterContainer > ("foo", 10),
                       6.5, 7.5, 8.5, 9.5, 10.5, 11.5, 12.5, 13.5, 14.5,
                       ElementLink< TrigInDetTrackCollection > ("bar", 20),
                       TrigInDetTrack::IDSCANID,
                       17.5, 15, 16);
    
  testit (trans1);
}


int main()
{
  SGTest::initTestStore();
  test1();
  return 0;
}
