/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file LUCID_EventTPCnv/test/LUCID_SimHitCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for LUCID_SimHitCnv_p1.
 */


#undef NDEBUG
#include "LUCID_EventTPCnv/LUCID_SimHitCnv_p1.h"
#include <cassert>
#include <iostream>


void compare (const LUCID_SimHit& p1,
              const LUCID_SimHit& p2)
{
  assert (p1.GetTubeID() == p2.GetTubeID());
  assert (p1.GetPdgCode() == p2.GetPdgCode());
  assert (p1.GetTrack() == p2.GetTrack());
  assert (p1.particleLink() == p2.particleLink());
  assert (p1.GetGenVolume() == p2.GetGenVolume());
  assert (p1.GetX() == p2.GetX());
  assert (p1.GetY() == p2.GetY());
  assert (p1.GetZ() == p2.GetZ());
  assert (p1.GetEPX() == p2.GetEPX());
  assert (p1.GetEPY() == p2.GetEPY());
  assert (p1.GetEPZ() == p2.GetEPZ());
  assert (p1.GetPreStepTime() == p2.GetPreStepTime());
  assert (p1.GetPostStepTime() == p2.GetPostStepTime());
  assert (p1.GetWavelength() == p2.GetWavelength());
  assert (p1.GetEnergy() == p2.GetEnergy());
}


void testit (const LUCID_SimHit& trans1)
{
  MsgStream log (0, "test");
  LUCID_SimHitCnv_p1 cnv;
  LUCID_SimHit_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  LUCID_SimHit trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  LUCID_SimHit trans1 (1, 2, 3, 4,
                       5.5, 6.5, 7.5,
                       8.5, 9.5, 10.5,
                       11.5, 12.5, 13.5, 14.5);
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
