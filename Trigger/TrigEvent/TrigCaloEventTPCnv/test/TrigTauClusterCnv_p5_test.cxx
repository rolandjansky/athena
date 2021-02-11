/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrigCaloEventTPCnv/test/TrigTauClusterCnv_p5_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Tests for TrigTauClusterCnv_p5.
 */


#undef NDEBUG
#include "TrigCaloEventTPCnv/TrigTauClusterCnv_p5.h"
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
  for (int i=0; i < TrigCaloCluster_p2::MAXSIZE_P; i++) {
    CaloSampling::CaloSample s = static_cast<CaloSampling::CaloSample>(i);
    assert (p1.rawEnergy(s) == p2.rawEnergy(s));
  }
  assert (p1.rawEta() == p2.rawEta());
  assert (p1.rawPhi() == p2.rawPhi());
  assert (p1.RoIword() == p2.RoIword());
  assert (p1.nCells() == p2.nCells());
  assert (p1.quality() == p2.quality());
}


void compare (const TrigTauCluster& p1,
              const TrigTauCluster& p2)
{
  compare (static_cast<const TrigCaloCluster&>(p1),
           static_cast<const TrigCaloCluster&>(p2));

  assert (p1.EMenergy() == p2.EMenergy());
  assert (p1.HADenergy() == p2.HADenergy());
  assert (p1.eCalib() == p2.eCalib());
  assert (p1.EMRadius2() == p2.EMRadius2());
  assert (p1.CaloRadius() == p2.CaloRadius());
  assert (p1.IsoFrac() == p2.IsoFrac());
  assert (p1.numStripCells() == p2.numStripCells());
  assert (p1.numTotCells() == p2.numTotCells());
  assert (p1.stripWidth() == p2.stripWidth());
  assert (p1.stripWidthOffline() == p2.stripWidthOffline());
  assert (p1.EMRadius3S() == p2.EMRadius3S());
  assert (p1.CoreFrac() == p2.CoreFrac());
  assert (p1.EMFrac() == p2.EMFrac());
  assert (p1.HadRadius() == p2.HadRadius());
  assert (p1.clusterDetailsLink() == p2.clusterDetailsLink());
}


void testit (const TrigTauCluster& trans1)
{
  MsgStream log (0, "test");
  TrigTauClusterCnv_p5 cnv;
  TrigTauCluster_p5 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigTauCluster trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  (void)Gaudi::Hive::currentContext();
  // Get proxy created outside of leak check.
  ElementLink<TrigTauClusterDetailsContainer> foo ("foofoo", 10);
  Athena_test::Leakcheck check;

  TrigTauCluster trans1 (100000, 2.2, 1.5, 12345);
  trans1.setRawEnergy (90000);
  trans1.setRawEt (80000);
  for (int i=0; i < CaloSampling::CaloSample::Unknown; i++) {
    CaloSampling::CaloSample s = static_cast<CaloSampling::CaloSample>(i);
    trans1.setRawEnergy (s, i*1000);
  }
  trans1.setRawEta (2.1);
  trans1.setRawPhi (1.6);
  trans1.setNCells (100);
  trans1.setquality (10);

  trans1.setEMenergy (80000);
  trans1.setHADenergy (80000);
  trans1.setEnergyCalib (110000);
  trans1.setEMRadius2 (0.5);
  trans1.setCaloRadius (0.6);
  trans1.setIsoFrac (0.7);
  trans1.setNumStripCells (30);
  trans1.setNumTotCells (90);
  trans1.setStripWidth (0.3);
  trans1.setStripWidthOffline (0.4);
  trans1.setEMRadius3S (0.8);
  trans1.setCoreFrac (0.5);
  trans1.setEMFrac (0.45);
  trans1.setHadRadius (0.55);
  
  trans1.setClusterDetails (ElementLink<TrigTauClusterDetailsContainer> ("foofoo", 10));
  
  testit (trans1);
}


int main()
{
  SGTest::initTestStore();
  test1();
  return 0;
}
