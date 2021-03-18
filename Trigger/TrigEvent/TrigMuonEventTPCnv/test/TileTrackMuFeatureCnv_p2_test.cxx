/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrigMuonEventTPCnv/test/TileTrackMuFeatureCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for TileTrackMuFeatureCnv_p2.
 */


#undef NDEBUG
#include "TrigMuonEventTPCnv/TileTrackMuFeatureCnv_p2.h"
#include "SGTools/TestStore.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <cassert>
#include <iostream>
#include <sstream>


void compare (const TileTrackMuFeature& p1,
              const TileTrackMuFeature& p2)
{
  assert (p1.PtTR_Trk() == p2.PtTR_Trk());
  assert (p1.EtaTR_Trk() == p2.EtaTR_Trk());
  assert (p1.PhiTR_Trk() == p2.PhiTR_Trk());
  assert (p1.Typ_IDTrk() == p2.Typ_IDTrk());
  assert (p1.TileMuLink() == p2.TileMuLink());
  assert (p1.IDScanLink() == p2.IDScanLink());
}


void testit (const TileTrackMuFeature& trans1)
{
  MsgStream log (0, "test");
  TileTrackMuFeatureCnv_p2 cnv;
  TileTrackMuFeature_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  TileTrackMuFeature trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  (void)Gaudi::Hive::currentContext();
  // Get proxies created outside of leak checking.
  ElementLink< TileMuFeatureContainer> foo ("foo", 10);
  ElementLink< TrigInDetTrackCollection> bar ("bar", 20);
  Athena_test::Leakcheck check;

  TileTrackMuFeature trans1 (2.5, 1.5, 3.5, 5,
                             ElementLink< TileMuFeatureContainer>("foo", 10),
                             ElementLink< TrigInDetTrackCollection>("bar", 20));
    
  testit (trans1);
}


int main()
{
  SGTest::initTestStore();
  test1();
  return 0;
}
