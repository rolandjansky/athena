/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file ALFA_EventTPCnv/test/ALFA_DigitCollectionCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief Tests for ALFA_DigitCollectionCnv_p1.
 */


#undef NDEBUG
#include "ALFA_EventTPCnv/ALFA_DigitCollectionCnv_p1.h"
#include <cassert>
#include <iostream>


void compare (const ALFA_Digit& p1,
              const ALFA_Digit& p2)
{
  assert (p1.getStation() == p2.getStation());
  assert (p1.getPlate()   == p2.getPlate());
  assert (p1.getFiber()   == p2.getFiber());
}


void compare (const ALFA_DigitCollection& p1,
              const ALFA_DigitCollection& p2)
{
  assert (p1.size() == p2.size());
  for (size_t i=0; i < p1.size(); i++)
    compare (*p1[i], *p2[i]);
}


void testit (const ALFA_DigitCollection& trans1)
{
  MsgStream log (0, "test");
  ALFA_DigitCollectionCnv_p1 cnv;
  ALFA_DigitCollection_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  ALFA_DigitCollection trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  ALFA_DigitCollection trans1;
  for (int i=0; i < 10; i++) {
    int o = i*100;
    trans1.push_back (new ALFA_Digit (123+o, 234+o, 345+o));
  }
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
