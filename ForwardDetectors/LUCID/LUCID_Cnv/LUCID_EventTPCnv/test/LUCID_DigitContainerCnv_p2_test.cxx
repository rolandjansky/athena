/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file LUCID_EventTPCnv/test/LUCID_DigitContainerCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for LUCID_DigitContainerCnv_p2.
 */


#undef NDEBUG
#include "LUCID_EventTPCnv/LUCID_DigitContainerCnv_p2.h"
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



void compare (const LUCID_DigitContainer& p1,
              const LUCID_DigitContainer& p2)
{
  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++)
    compare (*p1[i], *p2[i]);
}


void testit (const LUCID_DigitContainer& trans1)
{
  MsgStream log (0, "test");
  LUCID_DigitContainerCnv_p2 cnv;
  LUCID_DigitContainer_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  LUCID_DigitContainer trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  LUCID_DigitContainer trans1;
  for (int i=0; i < 10; i++) {
    int o = i*100;
    trans1.push_back (new LUCID_Digit (1+o, 2.5+o, 3+o, 4+o, 5+o, 6+o, i&1));
  }
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
