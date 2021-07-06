/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrigMuonEventTPCnv/test/TrigMuonEFCbTrackCnv_p7_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for TrigMuonEFCbTrackCnv_p7.
 */


#undef NDEBUG
#include "TrigMuonEventTPCnv/TrigMuonEFCbTrackCnv_p7.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfoContainerCnv_tlp2.h"
#include "TestTools/FLOATassert.h"
#include "SGTools/TestStore.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <cassert>
#include <iostream>


void compare (const P4IPtCotThPhiM& p1,
              const P4IPtCotThPhiM& p2)
{
  assert (Athena_test::isEqual (p1.iPt(), p2.iPt()));
  assert (p1.cotTh() == p2.cotTh());
  assert (p1.phi() == p2.phi());
  assert (p1.m() == p2.m());
}


void compare (const TrigMuonEFTrack& p1,
              const TrigMuonEFTrack& p2)
{
  compare (static_cast<const P4IPtCotThPhiM&>(p1),
           static_cast<const P4IPtCotThPhiM&>(p2));

  assert (p1.charge() == p2.charge());
  assert (p1.d0() == p2.d0());
  assert (p1.z0() == p2.z0());
  assert (p1.chi2() == p2.chi2());
  assert (p1.chi2prob() == p2.chi2prob());
  assert (p1.posX() == p2.posX());
  assert (p1.posY() == p2.posY());
  assert (p1.posZ() == p2.posZ());
  assert (p1.NRpcHitsPhi() == p2.NRpcHitsPhi());
  assert (p1.NTgcHitsPhi() == p2.NTgcHitsPhi());
  assert (p1.NCscHitsPhi() == p2.NCscHitsPhi());
  assert (p1.NRpcHitsEta() == p2.NRpcHitsEta());
  assert (p1.NTgcHitsEta() == p2.NTgcHitsEta());
  assert (p1.NCscHitsEta() == p2.NCscHitsEta());
  assert (p1.NMdtHits() == p2.NMdtHits());
}


void compare (const TrigMuonEFCbTrack& p1,
              const TrigMuonEFCbTrack& p2)
{
  compare (static_cast<const TrigMuonEFTrack&>(p1),
           static_cast<const TrigMuonEFTrack&>(p2));

  assert (p1.matchChi2() == p2.matchChi2());
  assert (p1.NIdSctHits() == p2.NIdSctHits());
  assert (p1.NIdPixelHits() == p2.NIdPixelHits());
  assert (p1.NTrtHits() == p2.NTrtHits());
  assert (p1.getIDTrackParticleLink() == p2.getIDTrackParticleLink());
}


void testit (const TrigMuonEFCbTrack& trans1)
{
  MsgStream log (0, "test");
  TrigMuonEFCbTrackCnv_p7 cnv;
  TrigMuonEFInfoContainerCnv_tlp2 tlcnv;
  cnv.setRuntimeTopConverter (&tlcnv);
  TrigMuonEFCbTrack_p7 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigMuonEFCbTrack trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  (void)Gaudi::Hive::currentContext();
  // Get proxy created outside of leak checking.
  ElementLink<Rec::TrackParticleContainer> foo ("foo", 10);
  Athena_test::Leakcheck check;

  TrigMuonEFCbTrack trans1 (1./80000, 1.5, 2.5, 5000);
  trans1.setCharge (3.5);
  trans1.setD0 (4.5);
  trans1.setZ0 (5.5);
  trans1.setChi2 (6.5);
  trans1.setChi2prob (7.5);
  trans1.setPosX (8.5);
  trans1.setPosY (9.5);
  trans1.setPosZ (10.5);
  trans1.setNRpcHitsPhi (11);
  trans1.setNTgcHitsPhi (12);
  trans1.setNCscHitsPhi (13);
  trans1.setNRpcHitsEta (15);
  trans1.setNTgcHitsEta (16);
  trans1.setNCscHitsEta (17);
  trans1.setNMdtHits (19);

  trans1.setMatchChi2 (20.5);
  trans1.setIDTrackElementLink (ElementLink<Rec::TrackParticleContainer>("foo", 10));
  trans1.setNIdSctHits (21);
  trans1.setNIdPixelHits (22);
  trans1.setNTrtHits (23);
    
  testit (trans1);
}


int main()
{
  SGTest::initTestStore();
  test1();
  return 0;
}
