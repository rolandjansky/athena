/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigT1EventTPCnv/test/CMMEtSumsCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Tests for CMMEtSumsCnv_p1.
 */


#undef NDEBUG
#include "TrigT1EventTPCnv/CMMEtSumsCnv_p1.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const LVL1::CMMEtSums& p1,
              const LVL1::CMMEtSums& p2)
{
  assert (p1.crate() == p2.crate());
  assert (p1.dataID() == p2.dataID());
  assert (p1.peak() == p2.peak());
  assert (p1.EtVec() == p2.EtVec());
  assert (p1.ExVec() == p2.ExVec());
  assert (p1.EyVec() == p2.EyVec());
  assert (p1.EtErrorVec() == p2.EtErrorVec());
  assert (p1.ExErrorVec() == p2.ExErrorVec());
  assert (p1.EyErrorVec() == p2.EyErrorVec());
}


void testit (const LVL1::CMMEtSums& trans1)
{
  MsgStream log (0, "test");
  CMMEtSumsCnv_p1 cnv;
  CMMEtSums_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  LVL1::CMMEtSums trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  LVL1::CMMEtSums trans1 (1, 2,
                          std::vector<unsigned int> {3, 4, 5},
                          std::vector<unsigned int> {6, 7},
                          std::vector<unsigned int> {100, 101},
                          std::vector<int> {8, 9},
                          std::vector<int> {10, 11, 12},
                          std::vector<int> {102, 103},
                          13);

  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
