/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file LUCID_EventTPCnv/test/LUCID_DigitCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for LUCID_DigitCnv_p2.
 */


#undef NDEBUG
#include "LUCID_EventTPCnv/LUCID_DigitCnv_p2.h"
#include <cassert>
#include <iostream>


void compare (const LUCID_Digit& p1,
              const LUCID_Digit& p2)
{
  assert (p1.getTubeID() == p2.getTubeID());
  assert (p1.getNpe()    == p2.getNpe());
  assert (p1.getNpeGas() == p2.getNpeGas());
  assert (p1.getNpePmt() == p2.getNpePmt());
  assert (p1.getQDC()    == p2.getQDC());
  assert (p1.getTDC()    == p2.getTDC());
  assert (p1.isHit()     == p2.isHit());
}


void testit (const LUCID_Digit& trans1)
{
  MsgStream log (0, "test");
  LUCID_DigitCnv_p2 cnv;
  LUCID_Digit_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  LUCID_Digit trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  LUCID_Digit trans1 (1, 2.5, 3, 4, 5, 6, true);
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
