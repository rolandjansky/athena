/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonEventAthenaPool/test/TgcRawDataCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2016
 * @brief Regression tests.
 */

#undef NDEBUG
#include "../src/TgcRawDataCnv_p2.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TgcRawData& p1,
              const TgcRawData& p2)
{
  assert (p1.bcTag() == p2.bcTag());
  assert (p1.subDetectorId() == p2.subDetectorId());
  assert (p1.rodId() == p2.rodId());
  assert (p1.sswId() == p2.sswId());
  assert (p1.slbId() == p2.slbId());
  assert (p1.l1Id() == p2.l1Id());
  assert (p1.bcId() == p2.bcId());
  assert (p1.slbType() == p2.slbType());
  assert (p1.bitpos() == p2.bitpos());
  assert (p1.tracklet() == p2.tracklet());
  assert (p1.isAdjacent() == p2.isAdjacent());
  assert (p1.type() == p2.type());
  assert (p1.isForward() == p2.isForward());
  assert (p1.index() == p2.index());
  assert (p1.position() == p2.position());
  assert (p1.delta() == p2.delta());
  assert (p1.segment() == p2.segment());
  assert (p1.subMatrix() == p2.subMatrix());
  assert (p1.sector() == p2.sector());
  assert (p1.chip() == p2.chip());
  assert (p1.isHipt() == p2.isHipt());
  assert (p1.hitId() == p2.hitId());
  assert (p1.hsub() == p2.hsub());
  assert (p1.isStrip() == p2.isStrip());
  //assert (p1.inner() == p2.inner());
  assert (p1.cand3plus() == p2.cand3plus());
  assert (p1.isMuplus() == p2.isMuplus());
  assert (p1.threshold() == p2.threshold());
  assert (p1.isOverlap() == p2.isOverlap());
  assert (p1.isVeto() == p2.isVeto());
  assert (p1.roi() == p2.roi());
}


void testit (const TgcRawData& trans1)
{
  MsgStream log (nullptr, "test");
  TgcRawDataCnv_p2 cnv;
  TgcRawData_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  TgcRawData trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  Athena_test::Leakcheck check;

  // hit
  TgcRawData trans1 (1, 2, 3, 4, 5, 6, 7, 8);
  testit (trans1);

  // trigger coincidence
  TgcRawData trans2 (11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21);
  testit (trans2);

  // P2 hits
  TgcRawData trans3 (31, 32, 33, 34, 35, 36, 37,
                     TgcRawData::SLB_TYPE_DOUBLET_STRIP,
                     true, 38, 39);
  testit (trans3);

  // tracklets
  TgcRawData trans4 (41, 42, 43, 44, 45, 46, 47,
                     TgcRawData::SLB_TYPE_TRIPLET_WIRE,
                     48, 49, 50, 51);
  testit (trans4);

  // high-pt
  TgcRawData trans5 (61, 62, 63, 64, 65, true, false, 66, 67, 68, true,
                     69, 70, 71, 72);
  testit (trans5);

  // sector logic
  TgcRawData trans6 (71, 72, 73, 74, 75, false, true, 76, 77, false,
                     78, true, false, 79);
  testit (trans6);
}


int main()
{
  test1();
  return 0;
}
