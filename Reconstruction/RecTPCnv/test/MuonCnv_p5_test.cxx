/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file RecTPCnv/test/MuonCnv_p5_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2019
 * @brief Regression tests.
 */

#undef NDEBUG
#include "RecTPCnv/MuonCnv_p5.h"
#include "muonEvent/Muon.h"
#include "SGTools/TestStore.h"
#include "TestTools/leakcheck.h"
#include "TestTools/FLOATassert.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <cassert>
#include <iostream>


void compare (const ParticleEvent::Base& p1,
              const ParticleEvent::Base& p2)
{
  assert (p1.dataType() == p2.dataType());
  assert (p1.hasCharge() == p2.hasCharge());
  assert (p1.charge() == p2.charge());
  assert (p1.hasPdgId() == p2.hasPdgId());
  assert (p1.pdgId() == p2.pdgId());
  assert (p1.getAthenaBarCodeImpl().getAthenaBarCode() == p2.getAthenaBarCodeImpl().getAthenaBarCode());
  assert (p1.originLink() == p2.originLink());
}


void compare (const Trk::FitQuality& p1,
              const Trk::FitQuality& p2)
{
  assert (p1.chiSquared() == p2.chiSquared());
  assert (p1.numberDoF()  == p2.numberDoF());
}


void compare (const Analysis::Muon& p1,
              const Analysis::Muon& p2)
{
  compare (p1.particleBase(), p2.particleBase());

  assert (Athena_test::isEqual (p1.iPt(), p2.iPt()));
  assert (p1.cotTh() == p2.cotTh());
  assert (p1.phi() == p2.phi());
  assert (p1.m() == p2.m());

  assert (p1.author() == p2.author());
  assert (p1.matchChi2() == p2.matchChi2());
  assert (p1.matchNumberDoF() == p2.matchNumberDoF());
  //assert (p1.outerMatchNumberDoF() == p2.outerMatchNumberDoF()); // in p6
  assert (p1.segmentChi2OverDoF() == p2.segmentChi2OverDoF());
  //compare (*p1.outerMatchChi2(), *p2.outerMatchChi2()); // in p6
  assert (p1.bestMatch() == p2.bestMatch());
  assert (p1.associatedEtaDigits() == p2.associatedEtaDigits());
  assert (p1.associatedPhiDigits() == p2.associatedPhiDigits());
  assert (p1.hasMuonExtrapolatedTrackParticleFlag() == p2.hasMuonExtrapolatedTrackParticleFlag());
  assert (p1.muonExtrapTrackLink() == p2.muonExtrapTrackLink());
  assert (p1.hasCombinedMuonTrackParticleFlag() == p2.hasCombinedMuonTrackParticleFlag());
  assert (p1.combinedTrackLink() == p2.combinedTrackLink());
  assert (p1.inDetTrackLink() == p2.inDetTrackLink());
  assert (p1.muonSegmentLink() == p2.muonSegmentLink());
  assert (p1.innerExtrapTrackLink() == p2.innerExtrapTrackLink());
  //assert (p1.statCombinedTrackLink() == p2.statCombinedTrackLink()); // in p6
  assert (p1.clusterLink() == p2.clusterLink());
  assert (p1.muonSpectrometerTrackLink() == p2.muonSpectrometerTrackLink());
  assert (p1.caloEnergyLink() == p2.caloEnergyLink());
  assert (p1.alsoFoundByLowPt() == p2.alsoFoundByLowPt());
  assert (p1.alsoFoundByCaloMuonId() == p2.alsoFoundByCaloMuonId());
  assert (p1.caloMuonAlsoFoundByMuonReco() == p2.caloMuonAlsoFoundByMuonReco());
  assert (p1.isCorrected() == p2.isCorrected());
  assert (p1.allAuthors() == p2.allAuthors());
  assert (p1.isMuonLikelihood() == p2.isMuonLikelihood());
  assert (p1.isMuon().qualityWord() == p2.isMuon().qualityWord());
  //assert (p1.spectrometerFieldIntegral() == p2.spectrometerFieldIntegral()); // in p6
  //assert (p1.scatteringCurvatureSignificance() == p2.scatteringCurvatureSignificance()); // in p6
  //assert (p1.scatteringNeighbourSignificance() == p2.scatteringNeighbourSignificance()); // in p6
  //assert (p1.momentumBalanceSignificance() == p2.momentumBalanceSignificance()); // in p6
  assert (p1.segmentDeltaEta() == p2.segmentDeltaEta());
  assert (p1.segmentDeltaPhi() == p2.segmentDeltaPhi());
  assert (p1.annBarrel() == p2.annBarrel());
  assert (p1.annEndCap() == p2.annEndCap());
  assert (p1.innAngle() == p2.innAngle());
  assert (p1.midAngle() == p2.midAngle());
  assert (p1.t0() == p2.t0());
  assert (p1.beta() == p2.beta());
}


void testit (const Analysis::Muon& trans1)
{
  MsgStream log (0, "test");
  MuonCnv_p5 cnv;
  Muon_p5 pers;
  cnv.transToPers (&trans1, &pers, log);
  Analysis::Muon trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  {
    Analysis::Muon xx (MuonParameters::highPt);
  }
  (void)Gaudi::Hive::currentContext();
  ElementLink<Rec::TrackParticleContainer> el1 ("cont1", 1);
  ElementLink<Rec::TrackParticleContainer> el2 ("cont2", 2);
  ElementLink<Rec::TrackParticleContainer> el3 ("cont3", 3);
  ElementLinkVector<Trk::SegmentCollection> elv4;
  elv4.push_back (ElementLink<Trk::SegmentCollection> ("cont4", 33));
  elv4.push_back (ElementLink<Trk::SegmentCollection> ("cont4", 34));
  ElementLink<Rec::TrackParticleContainer> el5 ("cont5", 5);
  ElementLink<Rec::TrackParticleContainer> el6 ("cont6", 6);
  ElementLink<CaloClusterContainer> el7 ("cont7", 7);
  ElementLink<Rec::TrackParticleContainer> el8 ("cont8", 8);
  ElementLink<MuonCaloEnergyContainer> el9 ("cont9", 9);
  ElementLink<VxContainer> el10 ("cont10", 10);
  Athena_test::Leakcheck check;

  Analysis::Muon trans1 (MuonParameters::highPt);

  trans1.momentumBase().setIPt (0.05);
  trans1.momentumBase().setCotTh (-0.5);
  trans1.momentumBase().setPhi (2.5);
  trans1.momentumBase().setM (200.5);

  trans1.set_dataType (ParticleDataType::Full);
  trans1.set_charge (0.5);
  trans1.set_pdgId (PDG::mu_plus);
  trans1.setAthenaBarCode (444);
  trans1.set_origin (el10);
  
  trans1.set_matchChi2 (1.5);
  trans1.set_matchNumberDoF (2);
  trans1.set_outerMatchChi2 (4.5, 4);
  trans1.set_bestMatch (true);
  trans1.set_numberOfAssociatedEtaDigits (10, 11, 12, 13);
  trans1.set_numberOfAssociatedPhiDigits (5, 6, 7, 8);

  trans1.setMuonExtrapTrackLink (el1, true);
  trans1.setCombinedTrackLink (el2, false);
  trans1.setInDetTrackLink (el3);
  trans1.setMuonSegmentLink (elv4);
  trans1.setInnerExtrapTrackLink (el5);
  trans1.setStatCombinedTrackLink (el6);
  trans1.setClusterLink (el7);
  trans1.setMuonSpectrometerTrackLink (el8);
  trans1.setCaloEnergyLink (el9);

  trans1.set_isAlsoFoundByLowPt (true);
  trans1.set_isAlsoFoundByCaloMuonId (false);
  trans1.set_caloMuonAlsoFoundByMuonReco (2);
  trans1.set_isCorrected (true);
  trans1.set_allAuthors (123);
  trans1.set_isMuonLikelihood (2.5);
  trans1.set_isMuonBits (342);

  trans1.set_spectrometerFieldIntegral (30.5);
  trans1.set_scatteringCurvatureSignificance (31.5);
  trans1.set_scatteringNeighbourSignificance (32.5);
  trans1.set_momentumBalanceSignificance (33.5);
  trans1.set_segmentDeltaEta (34.5);
  trans1.set_segmentDeltaPhi (35.5);
  trans1.set_annBarrel (37.5);
  trans1.set_annEndCap (38.5);
  trans1.set_innAngle (39.5);
  trans1.set_midAngle (40.5);
  trans1.set_parameter (MuonParameters::t0, 41.5);
  trans1.set_parameter (MuonParameters::beta, 42.5);

  testit (trans1);
}


int main()
{
  std::cout << "RecTPCnv/MuonCnv_p5\n";
  SGTest::initTestStore();
  test1();
  return 0;
}
