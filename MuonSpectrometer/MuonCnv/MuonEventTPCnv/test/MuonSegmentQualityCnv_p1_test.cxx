/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonEventTPCnv/test/MuonSegmentQualityCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "MuonEventTPCnv/MuonSegment/MuonSegmentQualityCnv_p1.h"
#include "TrkEventTPCnv/TrackCollectionCnv_tlp5.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const Trk::FitQuality& p1,
              const Trk::FitQuality& p2)
{
  assert (p1.chiSquared() == p2.chiSquared());
  assert (p1.numberDoF()  == p2.numberDoF());
}


void compare (const Muon::MuonSegmentQuality& p1,
              const Muon::MuonSegmentQuality& p2)
{
  compare (static_cast<const Trk::FitQuality&>(p1),
           static_cast<const Trk::FitQuality&>(p2));
  //assert (p1.isStrict() == p2.isStrict());
  assert (p1.channelsWithoutHit() == p2.channelsWithoutHit());
}


void testit (const Muon::MuonSegmentQuality& trans1)
{
  MsgStream log (0, "test");
  MuonSegmentQualityCnv_p1 cnv;
  TrackCollectionCnv_tlp5 tlcnv;
  cnv.setTopConverter (&tlcnv, TPObjRef::typeID_t());
  Muon::MuonSegmentQuality_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  Muon::MuonSegmentQuality trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  std::vector<Identifier> channels;
  channels.emplace_back (1234);
  channels.emplace_back (4321);
  channels.emplace_back (3333);
  
  Muon::MuonSegmentQuality trans1 (1.5, 5, channels, true);
  testit (trans1);
  Muon::MuonSegmentQuality trans2 (1.5, 5, channels, false);
  testit (trans2);
}


int main()
{
  test1();
  return 0;
}
