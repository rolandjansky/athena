/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigT1EventTPCnv/test/TriggerTowerCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Tests for TriggerTowerCnv_p1.
 */


#undef NDEBUG
#include "TrigT1EventTPCnv/TriggerTowerCnv_p1.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const LVL1::TriggerTower& p1,
              const LVL1::TriggerTower& p2)
{
  assert (p1.eta() == p2.eta());
  assert (p1.phi() == p2.phi());
  assert (p1.key() == p2.key());
  assert (p1.emError() == p2.emError());
  assert (p1.hadError() == p2.hadError());
  assert (p1.emPeak() == p2.emPeak());
  assert (p1.emADCPeak() == p2.emADCPeak());
  assert (p1.hadPeak() == p2.hadPeak());
  assert (p1.hadADCPeak() == p2.hadADCPeak());
  assert (p1.emLUT() == p2.emLUT());
  assert (p1.hadLUT() == p2.hadLUT());
  assert (p1.emADC() == p2.emADC());
  assert (p1.hadADC() == p2.hadADC());
  assert (p1.emBCIDvec() == p2.emBCIDvec());
  assert (p1.hadBCIDvec() == p2.hadBCIDvec());
  assert (p1.emBCIDext() == p2.emBCIDext());
  assert (p1.hadBCIDext() == p2.hadBCIDext());
}


void testit (const LVL1::TriggerTower& trans1)
{
  MsgStream log (0, "test");
  TriggerTowerCnv_p1 cnv;
  TriggerTower_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  LVL1::TriggerTower trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  LVL1::TriggerTower trans1 (1, 2, 3,
                             std::vector<int> {3, 4, 5},
                             std::vector<int> {6, 7},
                             std::vector<int> {8, 9, 10},
                             std::vector<int> {11, 12, 13},
                             101, 102, 103,
                             std::vector<int> {8, 9, 10},
                             std::vector<int> {11, 12},
                             std::vector<int> {20, 21, 22},
                             std::vector<int> {25, 26},
                             13, 14, 15);
  
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
