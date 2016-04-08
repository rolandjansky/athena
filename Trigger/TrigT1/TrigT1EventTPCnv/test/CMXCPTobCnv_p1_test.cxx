/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigT1EventTPCnv/test/CMXCPTobCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Tests for CMXCPTobCnv_p1.
 */


#undef NDEBUG
#include "TrigT1EventTPCnv/CMXCPTobCnv_p1.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const LVL1::CMXCPTob& p1,
              const LVL1::CMXCPTob& p2)
{
  assert (p1.crate() == p2.crate());
  assert (p1.cmx() == p2.cmx());
  assert (p1.cpm() == p2.cpm());
  assert (p1.chip() == p2.chip());
  assert (p1.location() == p2.location());
  assert (p1.peak() == p2.peak());
  assert (p1.energyVec() == p2.energyVec());
  assert (p1.isolationVec() == p2.isolationVec());
  assert (p1.errorVec() == p2.errorVec());
  assert (p1.presenceMapVec() == p2.presenceMapVec());
}


void testit (const LVL1::CMXCPTob& trans1)
{
  MsgStream log (0, "test");
  CMXCPTobCnv_p1 cnv;
  CMXCPTob_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  LVL1::CMXCPTob trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  LVL1::CMXCPTob trans1 (1, 2, 3, 4, 5,
                         std::vector<int> {10, 11, 12},
                         std::vector<int> {13, 14},
                         std::vector<int> {15, 16, 17},
                         std::vector<unsigned int> {18, 19},
                         20);

  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
