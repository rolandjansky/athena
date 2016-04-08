/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigT1EventTPCnv/test/JetElementCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Tests for JetElementCnv_p1.
 */


#undef NDEBUG
#include "TrigT1EventTPCnv/JetElementCnv_p1.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const LVL1::JetElement& p1,
              const LVL1::JetElement& p2)
{
  assert (p1.eta() == p2.eta());
  assert (p1.phi() == p2.phi());
  assert (p1.key() == p2.key());
  assert (p1.peak() == p2.peak());
  assert (p1.emEnergyVec() == p2.emEnergyVec());
  assert (p1.hadEnergyVec() == p2.hadEnergyVec());
  assert (p1.emErrorVec() == p2.emErrorVec());
  assert (p1.hadErrorVec() == p2.hadErrorVec());
  assert (p1.linkErrorVec() == p2.linkErrorVec());
}


void testit (const LVL1::JetElement& trans1)
{
  MsgStream log (0, "test");
  JetElementCnv_p1 cnv;
  JetElement_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  LVL1::JetElement trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  LVL1::JetElement trans1 (1, 2,
                           std::vector<int> {3, 4, 5},
                           std::vector<int> {6, 7},
                           101,
                           std::vector<int> {8, 9, 10},
                           std::vector<int> {11, 12},
                           std::vector<int> {20, 21, 22},
                           13);
  
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
