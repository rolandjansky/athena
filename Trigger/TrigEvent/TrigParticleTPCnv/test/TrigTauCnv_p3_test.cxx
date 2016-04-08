/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigParticleTPCnv/test/TrigTauCnv_p3_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for TrigTauCnv_p3.
 */


#undef NDEBUG
#include "TrigParticleTPCnv/TrigTauCnv_p3.h"
#include "TestTools/FLOATassert.h"
#include "GaudiKernel/MsgStream.h"
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


void compare (const TrigTau& p1,
              const TrigTau& p2)
{
  compare (static_cast<const P4PtEtaPhiM&>(p1),
           static_cast<const P4PtEtaPhiM&>(p2));

  assert (p1.roiId() == p2.roiId());
  assert (p1.Zvtx() == p2.Zvtx());
  assert (p1.err_Zvtx() == p2.err_Zvtx());
  assert (p1.etCalibCluster() == p2.etCalibCluster());
  assert (p1.simpleEtFlow() == p2.simpleEtFlow());
  assert (p1.nMatchedTracks() == p2.nMatchedTracks());
  assert (p1.trkAvgDist() == p2.trkAvgDist());
  assert (p1.etOverPtLeadTrk() == p2.etOverPtLeadTrk());
  assert (p2.tauCluster() == nullptr);
  assert (p2.trackCollection() == nullptr);
  assert (p2.tracksInfo() == nullptr);
}


void testit (const TrigTau& trans1)
{
  MsgStream log (0, "test");
  TrigTauCnv_p3 cnv;
  TrigTau_p3 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigTau trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  TrigTau trans1 (123, //roi
                  1.5, 2.5, 8.5, 80000,
                  10.5, 11.5, 12.5, 13,
                  nullptr, nullptr, nullptr);
  trans1.setTrkAvgDist (14.5);
  trans1.setEtOverPtLeadTrk (15.5);
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
