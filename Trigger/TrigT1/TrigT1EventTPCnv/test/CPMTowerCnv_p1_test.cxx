/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigT1EventTPCnv/test/CPMTowerCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Tests for CPMTowerCnv_p1.
 */


#undef NDEBUG
#include "TrigT1EventTPCnv/CPMTowerCnv_p1.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const LVL1::CPMTower& p1,
              const LVL1::CPMTower& p2)
{
  assert (p1.eta() == p2.eta());
  assert (p1.phi() == p2.phi());
  assert (p1.peak() == p2.peak());
  assert (p1.emEnergyVec() == p2.emEnergyVec());
  assert (p1.hadEnergyVec() == p2.hadEnergyVec());
  assert (p1.emErrorVec() == p2.emErrorVec());
  assert (p1.hadErrorVec() == p2.hadErrorVec());
}


void testit (const LVL1::CPMTower& trans1)
{
  MsgStream log (0, "test");
  CPMTowerCnv_p1 cnv;
  CPMTower_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  LVL1::CPMTower trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  LVL1::CPMTower trans1 (1, 2,
                         std::vector<int> {3, 4, 5},
                         std::vector<int> {6, 7},
                         std::vector<int> {8, 9, 10},
                         std::vector<int> {11, 12},
                         13);
  
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
