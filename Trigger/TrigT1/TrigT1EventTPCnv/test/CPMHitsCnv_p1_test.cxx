/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigT1EventTPCnv/test/CPMHitsCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Tests for CPMHitsCnv_p1.
 */


#undef NDEBUG
#include "TrigT1EventTPCnv/CPMHitsCnv_p1.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const LVL1::CPMHits& p1,
              const LVL1::CPMHits& p2)
{
  assert (p1.crate() == p2.crate());
  assert (p1.module() == p2.module());
  assert (p1.peak() == p2.peak());
  assert (p1.HitsVec0() == p2.HitsVec0());
  assert (p1.HitsVec1() == p2.HitsVec1());
}


void testit (const LVL1::CPMHits& trans1)
{
  MsgStream log (0, "test");
  CPMHitsCnv_p1 cnv;
  CPMHits_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  LVL1::CPMHits trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  LVL1::CPMHits trans1 (1, 2,
                        std::vector<unsigned int> {3, 4, 5},
                        std::vector<unsigned int> {6, 7},
                        13);

  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
