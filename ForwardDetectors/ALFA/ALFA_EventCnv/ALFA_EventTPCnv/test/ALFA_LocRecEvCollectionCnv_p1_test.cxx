/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file ALFA_EventTPCnv/test/ALFA_LocRecEvCollectionCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief Tests for ALFA_LocRecEvCollectionCnv_p1.
 */


#undef NDEBUG
#include "ALFA_EventTPCnv/ALFA_LocRecEvCollectionCnv_p1.h"
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


void compare (const ALFA_LocRecEvCollection& p1,
              const ALFA_LocRecEvCollection& p2)
{
  assert (p1.size() == p2.size());
  for (size_t i=0; i < p1.size(); i++)
    compare (*p1[i], *p2[i]);
}


void testit (const ALFA_LocRecEvCollection& trans1)
{
  MsgStream log (0, "test");
  ALFA_LocRecEvCollectionCnv_p1 cnv;
  ALFA_LocRecEvCollection_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  ALFA_LocRecEvCollection trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  ALFA_LocRecEvCollection trans1;
  for (int i=0; i < 10; i++) {
    int o = i*100;
    trans1.push_back (new ALFA_LocRecEvent
                      (123+o, 234+o,
                       10.5+o, 11.5+o,
                       12.5+o, 13.5+o,
                       345+o, 456+o,
                       std::vector<int> {5+o, 6+o, 7+o}));
  }

  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
