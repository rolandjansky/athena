/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file eflowEventTPCnv/test/eflowObjectContainerCnv_p5_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief Tests for eflowObjectContainerCnv_p5.
 */


#undef NDEBUG
#include "eflowEventTPCnv/eflowObjectContainerCnv_p5.h"
#include "SGTools/TestStore.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <cassert>
#include <iostream>


void compare (const CaloRecoStatus& p1,
              const CaloRecoStatus& p2)
{
  assert (p1.getStatusWord() == p2.getStatusWord());
}


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

  compare (p1.getCaloRecoStatus(), p2.getCaloRecoStatus());
  assert (0 == p2.d0());
  assert (0 == p2.z0());
  assert (p1.eflowType() == p2.eflowType());
  assert (p1.charge() == p2.charge());
  assert (p1.isValid() == p2.isValid());
  //assert (p1.muonLink() == p2.muonLink());
  //assert (p1.conversionLink() == p2.conversionLink());
  assert (p1.getPi0MVA() == p2.getPi0MVA());
  assert (p1.getCenterMag() == p2.getCenterMag());
}


void compare (const eflowObjectContainer& p1,
              const eflowObjectContainer& p2)
{
  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++)
    compare (*p1[i], *p2[i]);

  assert (-999 == p2.missET());
  assert (-999 == p2.sumET());
  assert (-999 == p2.pTx());
  assert (-999 == p2.pTy());
  assert (-9 == p2.numEf());
  assert (-9 == p2.numPhot());
  assert (-9 == p2.numEle());
  assert (-9 == p2.numMuo());
  assert (-999 == p2.circularity());
  assert (-999 == p2.thrust());
  assert (-999 == p2.oblateness());
  assert (true == p2.isValid());
}


void testit (const eflowObjectContainer& trans1)
{
  MsgStream log (0, "test");
  eflowObjectContainerCnv_p5 cnv;
  eflowObjectContainer_p5 pers;
  cnv.transToPers (&trans1, &pers, log);
  eflowObjectContainer trans2;
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

  eflowObjectContainer trans1;
  for (int i=0; i < 10; i++) {
    int o = i*100;
    auto p = std::make_unique<eflowObject>();
    p->setE (100.5+o);
    p->setEta (2.5+o);
    p->setPhi (1.5+o);
    p->setM (50.5+o);

    p->setPassEOverPCheck (true);
    p->setIsSubtracted (false);
    p->setIsDuplicated (true);

    p->addClus (ElementLink<CaloClusterContainer> ("foo", 1+i));
    p->addClus (ElementLink<CaloClusterContainer> ("bar", 2+i));

    p->addTrack (ElementLink<Rec::TrackParticleContainer> ("aaa", 3+i));
    p->addTrack (ElementLink<Rec::TrackParticleContainer> ("bbb", 4+i));
    p->addTrack (ElementLink<Rec::TrackParticleContainer> ("ccc", 5+i));

    p->setCaloRecoStatus (CaloRecoStatus (CaloRecoStatus::CALIBRATED));
    p->set_d0 (12.5+o);
    p->set_z0 (13.5+o);
    p->set_type (14+o);
    p->set_charge (15+o);
    p->set_valid (true);
    p->addMuon (ElementLink<Analysis::MuonContainer> ("aa", 6+i));
    p->addConversion (ElementLink<VxContainer> ("bb", 7+i));
    p->setPi0MVA (16.5+o);
    p->setCenterMag(17.5+o);
    trans1.push_back (std::move(p));
  }

  trans1.setMissET (1011.5);
  trans1.setSumET (1012.5);
  trans1.setpTx (1013.5);
  trans1.setpTy (1013.5);
  trans1.setNumEf (1014.5);
  trans1.setNumPhot (1015.5);
  trans1.setNumEle (1016.5);
  trans1.setNumMuo (1017.5);
  trans1.setCircularity (1018.5);
  trans1.setThrust (1019.5);
  trans1.setOblateness (1020.5);
  trans1.set_valid (true);

  testit (trans1);
}


int main()
{
  SGTest::initTestStore();
  test1();
  return 0;
}
