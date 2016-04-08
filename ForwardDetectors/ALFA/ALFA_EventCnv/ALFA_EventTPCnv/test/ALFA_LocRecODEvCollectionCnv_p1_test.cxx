/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file ALFA_EventTPCnv/test/ALFA_LocRecODEvCollectionCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief Tests for ALFA_LocRecODEvCollectionCnv_p1.
 */


#undef NDEBUG
#include "ALFA_EventTPCnv/ALFA_LocRecODEvCollectionCnv_p1.h"
#include <cassert>
#include <iostream>


void compare (const ALFA_LocRecODEvent& p1,
              const ALFA_LocRecODEvent& p2)
{
  assert (p1.getAlgoNum() == p2.getAlgoNum());
  assert (p1.getPotNum() == p2.getPotNum());
  assert (p1.getSide() == p2.getSide());
  assert (p1.getYposition() == p2.getYposition());
  assert (p1.getOverY() == p2.getOverY());
  assert (p1.getNumY() == p2.getNumY());
  assert (p1.getFibSel() == p2.getFibSel());
}


void compare (const ALFA_LocRecODEvCollection& p1,
              const ALFA_LocRecODEvCollection& p2)
{
  assert (p1.size() == p2.size());
  for (size_t i=0; i < p1.size(); i++)
    compare (*p1[i], *p2[i]);
}


void testit (const ALFA_LocRecODEvCollection& trans1)
{
  MsgStream log (0, "test");
  ALFA_LocRecODEvCollectionCnv_p1 cnv;
  ALFA_LocRecODEvCollection_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  ALFA_LocRecODEvCollection trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  ALFA_LocRecODEvCollection trans1;
  for (int i=0; i < 10; i++) {
    int o = i*100;
    trans1.push_back (new ALFA_LocRecODEvent
                      (123+o, 234+o, 345+o,
                       10.5+o, 11.5+o,
                       543+o,
                       std::vector<int> {5+o, 6+o, 7+o}));
  }
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
