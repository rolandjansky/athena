/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file ALFA_EventTPCnv/test/ALFA_GloRecEventCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief Tests for ALFA_GloRecEventCnv_p1.
 */


#undef NDEBUG
#include "ALFA_EventTPCnv/ALFA_GloRecEventCnv_p1.h"
#include <cassert>
#include <iostream>


void compare (const ALFA_GloRecEvent& p1,
              const ALFA_GloRecEvent& p2)
{
  assert (p1.getArmNum() == p2.getArmNum());
  assert (p1.getXposition() == p2.getXposition());
  assert (p1.getYposition() == p2.getYposition());
  assert (p1.getXslope() == p2.getXslope());
  assert (p1.getYslope() == p2.getYslope());
}


void testit (const ALFA_GloRecEvent& trans1)
{
  MsgStream log (0, "test");
  ALFA_GloRecEventCnv_p1 cnv;
  ALFA_GloRecEvent_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  ALFA_GloRecEvent trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  ALFA_GloRecEvent trans1 (123, 10.5, 11.5, 12.5, 13.5, nullptr, nullptr);
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
