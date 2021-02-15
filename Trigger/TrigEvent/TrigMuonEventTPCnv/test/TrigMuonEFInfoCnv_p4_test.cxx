/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrigMuonEventTPCnv/test/TrigMuonEFInfoCnv_p4_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for TrigMuonEFInfoCnv_p4.
 */


#undef NDEBUG
#include "TrigMuonEventTPCnv/TrigMuonEFInfoCnv_p4.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfoContainerCnv_tlp2.h"
#include "SGTools/TestStore.h"
#include "TestTools/FLOATassert.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <cassert>
#include <iostream>


void compare (const P4IPtCotThPhiM& p1,
              const P4IPtCotThPhiM& p2)
{
  assert (Athena_test::isEqual (p1.iPt(), p2.iPt()));
  assert (Athena_test::isEqual (p1.cotTh(), p2.cotTh()));
  assert (Athena_test::isEqual (p1.phi(), p2.phi()));
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


void compare (const TrigMuonEFInfoTrack& p1,
              const TrigMuonEFInfoTrack& p2)
{
  assert (p1.MuonType() == p2.MuonType());

  if (p1.SpectrometerTrack())
    compare (*p1.SpectrometerTrack(), *p2.SpectrometerTrack());
  else
    assert (nullptr == p2.SpectrometerTrack());

  if (p1.ExtrapolatedTrack())
    compare (*p1.ExtrapolatedTrack(), *p2.ExtrapolatedTrack());
  else
    assert (nullptr == p2.ExtrapolatedTrack());

  if (p1.CombinedTrack())
    compare (*p1.CombinedTrack(), *p2.CombinedTrack());
  else
    assert (nullptr == p2.CombinedTrack());
}


void compare (const TrigMuonEFInfoTrackContainer& p1,
              const TrigMuonEFInfoTrackContainer& p2)
{
  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++)
    compare (*p1[i], *p2[i]);
}


void compare (const TrigMuonEFInfo& p1,
              const TrigMuonEFInfo& p2)
{
  assert (p1.RoINum() == p2.RoINum());
  assert (p1.NSegments() == p2.NSegments());
  assert (p1.NMdtHits() == p2.NMdtHits());
  assert (p1.NRpcHits() == p2.NRpcHits());
  assert (p1.NTgcHits() == p2.NTgcHits());
  assert (p1.NCscHits() == p2.NCscHits());
  assert (p1.EtaPreviousLevel() == p2.EtaPreviousLevel());
  assert (p1.PhiPreviousLevel() == p2.PhiPreviousLevel());

  if (p1.TrackContainer())
    compare (*p1.TrackContainer(), *p2.TrackContainer());
  else
    assert (nullptr == p2.TrackContainer());

#if 0
  if (p1.legacySpectrometerTrack())
    compare (*p1.legacySpectrometerTrack(), *p2.legacySpectrometerTrack());
  else
    assert (nullptr == p2.legacySpectrometerTrack());

  if (p1.legacyExtrapolatedTrack())
    compare (*p1.legacyExtrapolatedTrack(), *p2.legacyExtrapolatedTrack());
  else
    assert (nullptr == p2.legacyExtrapolatedTrack());

  if (p1.legacyCombinedTrack())
    compare (*p1.legacyCombinedTrack(), *p2.legacyCombinedTrack());
  else
    assert (nullptr == p2.legacyCombinedTrack());
#endif
}


void testit (const TrigMuonEFInfo& trans1)
{
  MsgStream log (0, "test");
  TrigMuonEFInfoCnv_p4 cnv;
  TrigMuonEFInfoContainerCnv_tlp2 tlcnv;
  cnv.setRuntimeTopConverter (&tlcnv);
  TrigMuonEFInfo_p4 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigMuonEFInfo trans2;
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

  TrigMuonEFInfo trans1 (123);
  trans1.setRoINum (234);
  trans1.setNSegments (1);
  trans1.setNMdtHits (2);
  trans1.setNRpcHits (2);
  trans1.setNTgcHits (3);
  trans1.setNCscHits (4);
  trans1.setEtaPreviousLevel (4.5);
  trans1.setPhiPreviousLevel (5.5);

  auto tk = std::make_unique<TrigMuonEFInfoTrack>();
  tk->setMuonType (123);

  {
    auto cbtrack = std::make_unique<TrigMuonEFCbTrack> (1./80000, 1.5, 2.5, 5000);
    cbtrack->setCharge (3.5);
    cbtrack->setD0 (4.5);
    cbtrack->setZ0 (5.5);
    cbtrack->setChi2 (6.5);
    cbtrack->setChi2prob (7.5);
    cbtrack->setPosX (8.5);
    cbtrack->setPosY (9.5);
    cbtrack->setPosZ (10.5);
    cbtrack->setNRpcHitsPhi (11);
    cbtrack->setNTgcHitsPhi (12);
    cbtrack->setNCscHitsPhi (13);
    cbtrack->setNRpcHitsEta (15);
    cbtrack->setNTgcHitsEta (16);
    cbtrack->setNCscHitsEta (17);
    cbtrack->setNMdtHits (19);

    cbtrack->setMatchChi2 (20.5);
    cbtrack->setIDTrackElementLink (ElementLink<Rec::TrackParticleContainer>("foo", 10));
    cbtrack->setNIdSctHits (21);
    cbtrack->setNIdPixelHits (22);
    cbtrack->setNTrtHits (23);

    tk->setCombinedTrack (std::move(cbtrack));
  }

  {
    auto sptrack = std::make_unique<TrigMuonEFTrack> (1./85000, 1.4, 2.4, 6000);
    sptrack->setCharge (3.5 + 20);
    sptrack->setD0 (4.5 + 20);
    sptrack->setZ0 (5.5 + 20);
    sptrack->setChi2 (6.5 + 20);
    sptrack->setChi2prob (7.5 + 20);
    sptrack->setPosX (8.5 + 20);
    sptrack->setPosY (9.5 + 20);
    sptrack->setPosZ (10.5 + 20);
    sptrack->setNRpcHitsPhi (11 + 20);
    sptrack->setNTgcHitsPhi (12 + 20);
    sptrack->setNCscHitsPhi (13 + 20);
    sptrack->setNRpcHitsEta (15 + 20);
    sptrack->setNTgcHitsEta (16 + 20);
    sptrack->setNCscHitsEta (17 + 20);
    sptrack->setNMdtHits (19 + 20);

    tk->setSpectrometerTrack (std::move(sptrack));
  }

  {
    auto extrack = std::make_unique<TrigMuonEFTrack> (1./87000, 1.3, 2.3, 7000);
    extrack->setCharge (3.5 + 40);
    extrack->setD0 (4.5 + 40);
    extrack->setZ0 (5.5 + 40);
    extrack->setChi2 (6.5 + 40);
    extrack->setChi2prob (7.5 + 40);
    extrack->setPosX (8.5 + 40);
    extrack->setPosY (9.5 + 40);
    extrack->setPosZ (10.5 + 40);
    extrack->setNRpcHitsPhi (11 + 40);
    extrack->setNTgcHitsPhi (12 + 40);
    extrack->setNCscHitsPhi (13 + 40);
    extrack->setNRpcHitsEta (15 + 40);
    extrack->setNTgcHitsEta (16 + 40);
    extrack->setNCscHitsEta (17 + 40);
    extrack->setNMdtHits (19 + 40);

    tk->setExtrapolatedTrack (std::move(extrack));
  }

  trans1.addTrack (tk.release());
    
  testit (trans1);
}


int main()
{
  SGTest::initTestStore();
  test1();
  return 0;
}
