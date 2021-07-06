/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrigCaloEventTPCnv/test/TrigEMClusterCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Tests for TrigEMClusterCnv_p2.
 */


#undef NDEBUG
#include "TrigCaloEventTPCnv/TrigEMClusterCnv_p2.h"
#include "TrigCaloEventTPCnv/TrigEMClusterCnv_tlp2.h"
#include "SGTools/TestStore.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <cassert>
#include <iostream>


void compare (const TrigCaloCluster& p1,
              const TrigCaloCluster& p2)
{
  assert (p1.rawEnergy() == p2.rawEnergy());
  assert (p1.rawEt() == p2.rawEt());
  for (int i=0; i < TrigCaloCluster_p1::MAXSIZE_P; i++) {
    CaloSampling::CaloSample s = static_cast<CaloSampling::CaloSample>(i);
    assert (p1.rawEnergy(s) == p2.rawEnergy(s));
  }
  assert (p1.rawEta() == p2.rawEta());
  assert (p1.rawPhi() == p2.rawPhi());
  assert (p1.RoIword() == p2.RoIword());
  assert (p1.nCells() == p2.nCells());
  assert (p1.quality() == p2.quality());
}


void compare (const TrigEMCluster& p1,
              const TrigEMCluster& p2)
{
  compare (static_cast<const TrigCaloCluster&>(p1),
           static_cast<const TrigCaloCluster&>(p2));
  assert (p1.energy() == p2.energy());
  assert (p1.et() == p2.et());
  for (int i=0; i < TrigCaloCluster_p1::MAXSIZE_P; i++) {
    CaloSampling::CaloSample s = static_cast<CaloSampling::CaloSample>(i);
    assert (p1.energy(s) == p2.energy(s));
  }
  assert (p1.eta() == p2.eta());
  assert (p1.phi() == p2.phi());
  assert (p1.e237() == p2.e237());
  assert (p1.e277() == p2.e277());
  assert (p1.fracs1() == p2.fracs1());
  assert (p1.weta2() == p2.weta2());
  assert (p1.ehad1() == p2.ehad1());
  assert (p1.Eta1() == p2.Eta1());
  assert (p1.emaxs1() == p2.emaxs1());
  assert (p1.e2tsts1() == p2.e2tsts1());
  assert (0 == p2.e233());
  assert (0 == p2.wstot());
  assert (p1.ringsLink() == p2.ringsLink());
}


void testit (const TrigEMCluster& trans1)
{
  MsgStream log (0, "test");
  TrigEMClusterCnv_p2 cnv;
  TrigEMClusterCnv_tlp2 tlcnv;
  cnv.setRuntimeTopConverter (&tlcnv);
  TrigEMCluster_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigEMCluster trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  (void)Gaudi::Hive::currentContext();
  // Get proxy created outside of leak check.
  ElementLink<RingerRingsContainer> foo ("foofoo", 10);
  Athena_test::Leakcheck check;

  TrigEMCluster trans1 (100000, 2.2, 1.5, 12345);
  trans1.setRawEnergy (90000);
  trans1.setRawEt (80000);
  for (int i=0; i < CaloSampling::CaloSample::Unknown; i++) {
    CaloSampling::CaloSample s = static_cast<CaloSampling::CaloSample>(i);
    trans1.setRawEnergy (s, i*1000);
    trans1.setEnergy (s, i*1000 + 500);
  }
  trans1.setRawEta (2.1);
  trans1.setRawPhi (1.6);
  trans1.setNCells (100);
  trans1.setquality (10);

  trans1.setEt (81000);
  trans1.set_e237 (50000);
  trans1.set_e277 (51000);
  trans1.set_fracs1 (0.4);
  trans1.set_weta2 (0.1);
  trans1.set_ehad1 (10000);
  trans1.set_Eta1 (2.5);
  trans1.set_emaxs1 (7000);
  trans1.set_e2tsts1 (5000);
  trans1.set_e233 (40000);
  trans1.set_wstot (0.7);
  trans1.setRings (ElementLink<RingerRingsContainer> ("foofoo", 10));
  
  testit (trans1);
}


int main()
{
  SGTest::initTestStore();
  test1();
  return 0;
}
