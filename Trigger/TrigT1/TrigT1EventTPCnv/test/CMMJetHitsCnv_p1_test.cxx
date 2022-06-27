/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrigT1EventTPCnv/test/CMMJetHitsCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Tests for CMMJetHitsCnv_p1.
 */


#undef NDEBUG
#include "TrigT1EventTPCnv/CMMJetHitsCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const LVL1::CMMJetHits& p1,
              const LVL1::CMMJetHits& p2)
{
  assert (p1.crate() == p2.crate());
  assert (p1.dataID() == p2.dataID());
  assert (p1.peak() == p2.peak());
  assert (p1.HitsVec() == p2.HitsVec());
  assert (p1.ErrorVec() == p2.ErrorVec());
}


void testit (const LVL1::CMMJetHits& trans1)
{
  MsgStream log (0, "test");
  CMMJetHitsCnv_p1 cnv;
  CMMJetHits_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  LVL1::CMMJetHits trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  LVL1::CMMJetHits trans1 (1, 2,
                          std::vector<unsigned int> {3, 4, 5},
                          std::vector<int> {8, 9},
                          13);

  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}
