/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TileTPCnv/test/TileCosmicMuonCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Tests for TileCosmicMuonCnv_p1.
 */


#undef NDEBUG
#include "TileTPCnv/TileCosmicMuonCnv_p1.h"
#include "TestTools/initGaudi.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TileCosmicMuon& p1,
              const TileCosmicMuon& p2)
{
  assert (p1.GetTime() == p2.GetTime());
  assert (p1.GetPositionX() == p2.GetPositionX());
  assert (p1.GetPositionY() == p2.GetPositionY());
  assert (p1.GetPositionZ() == p2.GetPositionZ());
  assert (p1.GetDirectionPhi() == p2.GetDirectionPhi());
  assert (p1.GetDirectionTheta() == p2.GetDirectionTheta());
  assert (p1.GetFitQuality() == p2.GetFitQuality());
  assert (p1.GetFitNCells() == p2.GetFitNCells());
  assert (p1.GetNSamples() == p2.GetNSamples());
  for (int i=0; i < p1.GetNSamples(); i++) {
    assert (p1.GetPathTop(i) == p2.GetPathTop(i));
    assert (p1.GetPathBottom(i) == p2.GetPathBottom(i));
    assert (p1.GetEnergyTop(i) == p2.GetEnergyTop(i));
    assert (p1.GetEnergyBottom(i) == p2.GetEnergyBottom(i));
  }
  assert (p1.GetTrackNCells() == p2.GetTrackNCells());
  for (int i = 0; i < p1.GetTrackNCells(); i++) {
    assert (p1.GetTrackCellHash(i) == p2.GetTrackCellHash(i));
  }
  assert (p2.GetNSegments() == 0);
}



void testit (const TileCosmicMuon& trans1)
{
  MsgStream log (0, "test");
  TileCosmicMuonCnv_p1 cnv;
  TileCosmicMuon_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TileCosmicMuon trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  TileCosmicMuon trans1;
  trans1.SetTime (1.5);
  trans1.SetPositionX (2.5);
  trans1.SetPositionY (3.5);
  trans1.SetPositionZ (4.5);
  trans1.SetDirectionPhi (5.5);
  trans1.SetDirectionTheta (6.5);
  trans1.SetFitQuality (7.5);
  trans1.SetFitNCells (8);

  trans1.SetPathTop      (std::vector<double> { 10.5, 11.5, 12.5 });
  trans1.SetPathBottom   (std::vector<double> { 20.5, 21.5, 22.5 });
  trans1.SetEnergyTop    (std::vector<double> { 30.5, 31.5, 32.5 });
  trans1.SetEnergyBottom (std::vector<double> { 40.5, 41.5, 42.5 });

  trans1.SetTrackCellHash (std::vector<IdentifierHash> { 11, 12, 13, 14 });

  trans1.SetSegmentPath (std::vector<double> { 50.5, 51.5 } );
  trans1.SetSegmentPartition (std::vector<int> { 60, 61 } );
  trans1.SetSegmentModule    (std::vector<int> { 70, 71 } );
  trans1.SetSegmentSampling  (std::vector<int> { 80, 81 } );

  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
