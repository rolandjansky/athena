/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrigFTKPool/test/FTKTrackFitterStatsCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2020
 * @brief Tests for FTKTrackFitterStatsCnv_p1.
 */


#undef NDEBUG
#include "../src/FTKTrackFitterStatsCnv_p1.h"
#include "TrigFTKPool/FTKTrackFitterStats.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const FTKTrackFitterStats& p1,
              const FTKTrackFitterStats& p2)
{
  assert (p1.getNCombs()                  == p2.getNCombs());
  assert (p1.getNFits()                   == p2.getNFits());
  assert (p1.getNFitsMajority()           == p2.getNFitsMajority());
  assert (p1.getNFitsMajority_pix()       == p2.getNFitsMajority_pix());
  assert (p1.getNFitsMajority_SCT()       == p2.getNFitsMajority_SCT());
  assert (p1.getNFitsRecovery()           == p2.getNFitsRecovery());
  assert (p1.getNAddFitsRecovery()        == p2.getNAddFitsRecovery());
  assert (p1.getNFitsBad()                == p2.getNFitsBad());
  assert (p1.getNFitsHWRejected()         == p2.getNFitsHWRejected());
  assert (p1.getNFitsBadMajority()        == p2.getNFitsBadMajority());
  assert (p1.getNFitsHWRejectedMajority() == p2.getNFitsHWRejectedMajority());
  //assert (p1.getNConn()                   == p2.getNConn()); // not saved
  //assert (p1.getNExtrapolatedTracks()     == p2.getNExtrapolatedTracks()); // not saved
}


void testit (const FTKTrackFitterStats& trans1)
{
  MsgStream log (0, "test");
  FTKTrackFitterStatsCnv_p1 cnv;
  FTKTrackFitterStats_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  FTKTrackFitterStats trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  FTKTrackFitterStats trans1;
  trans1.setNCombs (1);
  trans1.setNFits (2);
  trans1.setNFitsMajority (3);
  trans1.setNFitsMajority_pix (4);
  trans1.setNFitsMajority_SCT (5);
  trans1.setNFitsRecovery (6);
  trans1.setNAddFitsRecovery (6);
  trans1.setNFitsBad (7);
  trans1.setNFitsHWRejected (8);
  trans1.setNFitsBadMajority (9);
  trans1.setNFitsHWRejectedMajority (9);
  trans1.setNConn (10);
  trans1.setNExtrapolatedTracks (11);

  Athena_test::Leakcheck check;
  testit (trans1);
}


int main()
{
  std::cout << "TrigFTKPool/FTKTrackFitterStatsCnv_p1_test\n";
  test1();
  return 0;
}
