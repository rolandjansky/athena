/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigInDetEventTPCnv/test/TrigTauTracksInfoCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for TrigTauTracksInfoCnv_p2.
 */


#undef NDEBUG
#include "TrigInDetEventTPCnv/TrigTauTracksInfoCnv_p2.h"
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


void compare (const TrigTauTracksInfo& p1,
              const TrigTauTracksInfo& p2)
{
  compare (static_cast<const P4PtEtaPhiM&>(p1),
           static_cast<const P4PtEtaPhiM&>(p2));
  assert (p1.roiId() == p2.roiId());
  assert (p1.nCoreTracks() == p2.nCoreTracks());
  assert (p1.nSlowTracks() == p2.nSlowTracks());
  assert (p1.nIsoTracks() == p2.nIsoTracks());
  assert (p1.charge() == p2.charge());
  assert (p1.leadingTrackPt() == p2.leadingTrackPt());
  assert (p1.scalarPtSumCore() == p2.scalarPtSumCore());
  assert (p1.scalarPtSumIso() == p2.scalarPtSumIso());
  assert (p1.ptBalance() == p2.ptBalance());

  compare (p1.threeFastestTracks(), p2.threeFastestTracks());
  assert (nullptr == p2.trackCollection());
}


void testit (const TrigTauTracksInfo& trans1)
{
  MsgStream log (0, "test");
  TrigTauTracksInfoCnv_p2 cnv;
  TrigTauTracksInfo_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigTauTracksInfo trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  TrigTauTracksInfo trans1;
  trans1.setPt (100000);
  trans1.setEta (1.7);
  trans1.setPhi (2.1);
  trans1.setM (2000);
  trans1.setRoiId (123);
  trans1.setNCoreTracks (2);
  trans1.setNSlowTracks (3);
  trans1.setNIsoTracks (4);
  trans1.setCharge (5.5);
  trans1.setLeadingTrackPt (30000);
  trans1.setScalarPtSumCore (20000);
  trans1.setScalarPtSumIso (15000);
  trans1.setPtBalance (40000);
  trans1.set3fastestPtEtaPhiM (80000, 1.4, 1.7, 20000);
  
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
