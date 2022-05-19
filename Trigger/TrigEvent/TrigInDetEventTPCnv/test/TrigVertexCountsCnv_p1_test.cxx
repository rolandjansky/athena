/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrigInDetEventTPCnv/test/TrigVertexCountsCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for TrigVertexCountsCnv_p1.
 */


#undef NDEBUG
#include "TrigInDetEventTPCnv/TrigVertexCountsCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TrigVertexCounts& p1,
              const TrigVertexCounts& p2)
{
  assert (p1.vtxNtrks() == p2.vtxNtrks());
  assert (p1.vtxTrkPtSqSum() == p2.vtxTrkPtSqSum());
}


void testit (const TrigVertexCounts& trans1)
{
  MsgStream log (0, "test");
  TrigVertexCountsCnv_p1 cnv;
  TrigVertexCounts_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigVertexCounts trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  TrigVertexCounts trans1 (std::vector<unsigned int> {1, 2, 3},
                           std::vector<float> {6.5, 7.5});
  
  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}
