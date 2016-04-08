/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigT1EventTPCnv/test/CMXJetTobCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Tests for CMXJetTobCnv_p1.
 */


#undef NDEBUG
#include "TrigT1EventTPCnv/CMXJetTobCnv_p1.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const LVL1::CMXJetTob& p1,
              const LVL1::CMXJetTob& p2)
{
  assert (p1.crate() == p2.crate());
  assert (p1.jem() == p2.jem());
  assert (p1.frame() == p2.frame());
  assert (p1.location() == p2.location());
  assert (p1.peak() == p2.peak());
  assert (p1.energyLgVec() == p2.energyLgVec());
  assert (p1.energySmVec() == p2.energySmVec());
  assert (p1.errorVec() == p2.errorVec());
  assert (p1.presenceMapVec() == p2.presenceMapVec());
}


void testit (const LVL1::CMXJetTob& trans1)
{
  MsgStream log (0, "test");
  CMXJetTobCnv_p1 cnv;
  CMXJetTob_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  LVL1::CMXJetTob trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  LVL1::CMXJetTob trans1 (1, 2, 3, 4,
                          std::vector<int> {10, 11, 12},
                          std::vector<int> {13, 14},
                          std::vector<int> {15, 16, 17},
                          std::vector<unsigned int> {18, 19},
                          13);

  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
