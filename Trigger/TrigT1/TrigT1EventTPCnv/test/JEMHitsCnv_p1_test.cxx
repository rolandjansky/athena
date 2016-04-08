/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigT1EventTPCnv/test/JEMHitsCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Tests for JEMHitsCnv_p1.
 */


#undef NDEBUG
#include "TrigT1EventTPCnv/JEMHitsCnv_p1.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const LVL1::JEMHits& p1,
              const LVL1::JEMHits& p2)
{
  assert (p1.crate() == p2.crate());
  assert (p1.module() == p2.module());
  assert (p1.peak() == p2.peak());
  assert (p1.JetHitsVec() == p2.JetHitsVec());
}


void testit (const LVL1::JEMHits& trans1)
{
  MsgStream log (0, "test");
  JEMHitsCnv_p1 cnv;
  JEMHits_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  LVL1::JEMHits trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  LVL1::JEMHits trans1 (1, 2,
                        std::vector<unsigned int> {3, 4, 5},
                        13);

  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
