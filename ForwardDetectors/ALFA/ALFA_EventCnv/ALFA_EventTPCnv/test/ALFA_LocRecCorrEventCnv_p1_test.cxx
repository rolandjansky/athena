/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file ALFA_EventTPCnv/test/ALFA_LocRecCorrEventCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief Tests for ALFA_LocRecCorrEventCnv_p1.
 */


#undef NDEBUG
#include "ALFA_EventTPCnv/ALFA_LocRecCorrEventCnv_p1.h"
#include <cassert>
#include <iostream>


void compare (const ALFA_LocRecCorrEvent& p1,
              const ALFA_LocRecCorrEvent& p2)
{
  assert (p1.getAlgoNum() == p2.getAlgoNum());
  assert (p1.getPotNum() == p2.getPotNum());
  assert (p1.getXpositionLHC() == p2.getXpositionLHC());
  assert (p1.getYpositionLHC() == p2.getYpositionLHC());
  assert (p1.getZpositionLHC() == p2.getZpositionLHC());
  assert (p1.getXpositionPot() == p2.getXpositionPot());
  assert (p1.getYpositionPot() == p2.getYpositionPot());
  assert (p1.getXpositionStat() == p2.getXpositionStat());
  assert (p1.getYpositionStat() == p2.getYpositionStat());
  assert (p1.getXpositionBeam() == p2.getXpositionBeam());
  assert (p1.getYpositionBeam() == p2.getYpositionBeam());
}


void testit (const ALFA_LocRecCorrEvent& trans1)
{
  MsgStream log (0, "test");
  ALFA_LocRecCorrEventCnv_p1 cnv;
  ALFA_LocRecCorrEvent_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  ALFA_LocRecCorrEvent trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  ALFA_LocRecCorrEvent trans1 (123, 234,
                               10.5, 11.5, 12.5,
                               13.5, 14.5,
                               15.5, 16.5,
                               17.5, 18.5);
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
