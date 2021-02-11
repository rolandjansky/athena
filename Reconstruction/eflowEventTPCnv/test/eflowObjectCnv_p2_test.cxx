/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file eflowEventTPCnv/test/eflowObjectCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief Tests for eflowObjectCnv_p2.
 */


#undef NDEBUG
#include "eflowEventTPCnv/eflowObjectCnv_p2.h"
#include "SGTools/TestStore.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <cassert>
#include <iostream>


#if 0
void compare (const CaloRecoStatus& p1,
              const CaloRecoStatus& p2)
{
  assert (p1.getStatusWord() == p2.getStatusWord());
}
#endif


void compare (const eflowObject& p1,
              const eflowObject& p2)
{
  assert (p1.e()   == p2.e());
  assert (p1.eta() == p2.eta());
  assert (p1.phi() == p2.phi());
  assert (p1.m()   == p2.m());

  assert (true == p2.getPassEOverPCheck());
  assert (false == p2.getIsSubtracted());
  assert (false == p2.getIsDuplicated());

  assert (p1.numClus() == p2.numClus());
  for (int i=0; i < p1.numClus(); i++)
    assert (p1.clusLink(i) == p2.clusLink(i));

  assert (p1.numTrack() == p2.numTrack());
  for (int i=0; i < p1.numTrack(); i++)
    assert (p1.trackLink(i) == p2.trackLink(i));

  //compare (p1.getCaloRecoStatus(), p2.getCaloRecoStatus());
  assert (p2.d0() == p2.d0());
  assert (p1.z0() == p2.z0());
  assert (p1.eflowType() == p2.eflowType());
  assert (p1.charge() == p2.charge());
  assert (p1.isValid() == p2.isValid());
  assert (p1.muonLink() == p2.muonLink());
  assert (p1.conversionLink() == p2.conversionLink());
  assert (0 == p2.getPi0MVA());
  assert (0 == p2.getCenterMag());
}


void testit (const eflowObject& trans1)
{
  MsgStream log (0, "test");
  eflowObjectCnv_p2 cnv;
  eflowObject_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  eflowObject trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  (void)Gaudi::Hive::currentContext();
  // Get proxies created outside of leak checking.
  ElementLink<CaloClusterContainer> foo ("foo", 0);
  ElementLink<CaloClusterContainer> bar ("bar", 0);
  ElementLink<Rec::TrackParticleContainer> aaa ("aaa", 0);
  ElementLink<Rec::TrackParticleContainer> bbb ("bbb", 0);
  ElementLink<Rec::TrackParticleContainer> ccc ("ccc", 0);
  ElementLink<Analysis::MuonContainer> aa ("aa", 0);
  ElementLink<VxContainer> bb ("bb", 0);
  Athena_test::Leakcheck check;

  eflowObject trans1;
  trans1.setE (100.5);
  trans1.setEta (2.5);
  trans1.setPhi (1.5);
  trans1.setM (50.5);

  trans1.setPassEOverPCheck (true);
  trans1.setIsSubtracted (false);
  trans1.setIsDuplicated (true);

  trans1.addClus (ElementLink<CaloClusterContainer> ("foo", 1));
  trans1.addClus (ElementLink<CaloClusterContainer> ("bar", 2));

  trans1.addTrack (ElementLink<Rec::TrackParticleContainer> ("aaa", 3));
  trans1.addTrack (ElementLink<Rec::TrackParticleContainer> ("bbb", 4));
  trans1.addTrack (ElementLink<Rec::TrackParticleContainer> ("ccc", 5));

  trans1.setCaloRecoStatus (CaloRecoStatus (CaloRecoStatus::CALIBRATED));
  trans1.set_d0 (12.5);
  trans1.set_z0 (13.5);
  trans1.set_type (14);
  trans1.set_charge (15);
  trans1.set_valid (true);
  trans1.addMuon (ElementLink<Analysis::MuonContainer> ("aa", 6));
  trans1.addConversion (ElementLink<VxContainer> ("bb", 7));
  trans1.setPi0MVA (16.5);
  trans1.setCenterMag(17.5);

  testit (trans1);
}


int main()
{
  SGTest::initTestStore();
  test1();
  return 0;
}
