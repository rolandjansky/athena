/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file ALFA_EventTPCnv/test/ALFA_GloRecEvCollectionCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief Tests for ALFA_GloRecEvCollectionCnv_p1.
 */


#undef NDEBUG
#include "ALFA_EventTPCnv/ALFA_GloRecEvCollectionCnv_p1.h"
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


void compare (const ALFA_GloRecEvCollection& p1,
              const ALFA_GloRecEvCollection& p2)
{
  assert (p1.size() == p2.size());
  for (size_t i=0; i < p1.size(); i++)
    compare (*p1[i], *p2[i]);
}


void testit (const ALFA_GloRecEvCollection& trans1)
{
  MsgStream log (0, "test");
  ALFA_GloRecEvCollectionCnv_p1 cnv;
  ALFA_GloRecEvCollection_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  ALFA_GloRecEvCollection trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  ALFA_GloRecEvCollection trans1;
  for (int i=0; i < 10; i++) {
    int o = i*100;
    trans1.push_back (new ALFA_GloRecEvent (123+o, 10.5+o, 11.5+o,
                                            12.5+o, 13.5+o,
                                            nullptr, nullptr));
  }
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
