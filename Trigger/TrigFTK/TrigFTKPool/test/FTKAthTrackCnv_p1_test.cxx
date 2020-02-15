/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrigFTKPool/test/FTKAthTrackCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2020
 * @brief Tests for FTKAthTrackCnv_p1.
 */


#undef NDEBUG
#include "../src/FTKAthTrackCnv_p1.h"
#include "TrigFTKPool/FTKAthTrack.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const FTKAthTrack& p1,
              const FTKAthTrack& p2)
{
  assert (p1.getBankID()      == p2.getBankID());
  assert (p1.getRoadID()      == p2.getRoadID());
  assert (p1.getPatternID()   == p2.getPatternID());
  assert (p1.getSectorID()    == p2.getSectorID());
  assert (p1.getTrackID()     == p2.getTrackID());
  assert (p1.getHalfInvPt()   == p2.getHalfInvPt());
  assert (p1.getIP()          == p2.getIP());
  assert (p1.getPhi()         == p2.getPhi());
  assert (p1.getZ0()          == p2.getZ0());
  assert (p1.getCotTheta()    == p2.getCotTheta());
  assert (p1.getChi2()        == p2.getChi2());
  assert (p1.getOrigChi2()    == p2.getOrigChi2());
  assert (p1.getNMissing()    == p2.getNMissing());
  assert (p1.getTypeMask()    == p2.getTypeMask());
  assert (p1.getBitmask()     == p2.getBitmask());
  assert (p1.getHFRejected()  == p2.getHFRejected());
  assert (p1.getHWRejected()  == p2.getHWRejected());
  assert (p1.getHWTrackID()   == p2.getHWTrackID());
  assert (p1.getEventIndex()  == p2.getEventIndex());
  assert (p1.getBarcode()     == p2.getBarcode());
  assert (p1.getBarcodeFrac() == p2.getBarcodeFrac());

  assert (p1.getNCoords()   == p2.getNCoords());
  for (int i = 0; i < p1.getNCoords(); i++) {
    assert (p1.getCoord(i) == p2.getCoord(i));
  }
}


void testit (const FTKAthTrack& trans1)
{
  MsgStream log (0, "test");
  FTKAthTrackCnv_p1 cnv;
  FTKAthTrack_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  FTKAthTrack trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  FTKAthTrack trans1 (3);
  trans1.setBankID (1);
  trans1.setRoadID (2);
  trans1.setPatternID (3);
  trans1.setSectorID (4);
  trans1.setTrackID (5);
  trans1.setHalfInvPt (6.5);
  trans1.setIP (7.5);
  trans1.setPhi (8.5);
  trans1.setZ0 (8.5);
  trans1.setCotTheta (9.5);
  trans1.setChi2 (10.5);
  trans1.setOrigChi2 (11.5);
  trans1.setNMissing (12);
  trans1.setTypeMask (13);
  trans1.setBitmask (14);
  trans1.setHFRejected (15);
  trans1.setHWRejected (16);
  trans1.setHWTrackID (17);
  trans1.setEventIndex (19);
  trans1.setBarcode (20);
  trans1.setBarcodeFrac (21.5);

  trans1.setCoord (0, 22.5);
  trans1.setCoord (1, 23.5);
  trans1.setCoord (2, 24.5);

  Athena_test::Leakcheck check;
  testit (trans1);
}


int main()
{
  std::cout << "TrigFTKPool/FTKAthTrackCnv_p1_test\n";
  test1();
  return 0;
}
