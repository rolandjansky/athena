/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file ALFA_EventTPCnv/test/ALFA_LocRecEventCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief Tests for ALFA_LocRecEventCnv_p1.
 */


#undef NDEBUG
#include "ALFA_EventTPCnv/ALFA_LocRecEventCnv_p1.h"
#include <cassert>
#include <iostream>


void compare (const ALFA_LocRecEvent& p1,
              const ALFA_LocRecEvent& p2)
{
  assert (p1.getAlgoNum() == p2.getAlgoNum());
  assert (p1.getPotNum() == p2.getPotNum());
  assert (p1.getXposition() == p2.getXposition());
  assert (p1.getYposition() == p2.getYposition());
  assert (p1.getOverU() == p2.getOverU());
  assert (p1.getOverV() == p2.getOverV());
  assert (p1.getNumU() == p2.getNumU());
  assert (p1.getNumV() == p2.getNumV());
  assert (p1.getFibSel() == p2.getFibSel());
}


void testit (const ALFA_LocRecEvent& trans1)
{
  MsgStream log (0, "test");
  ALFA_LocRecEventCnv_p1 cnv;
  ALFA_LocRecEvent_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  ALFA_LocRecEvent trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  ALFA_LocRecEvent trans1 (123, 234,
                           10.5, 11.5,
                           12.5, 13.5,
                           345, 456,
                           std::vector<int> {5, 6, 7});
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
