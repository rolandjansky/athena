/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file RecTPCnv/test/MissingEtTruthCnv_p3_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2019
 * @brief Regression tests.
 */

#undef NDEBUG
#include "RecTPCnv/MissingEtTruthCnv_p3.h"
#include "MissingETEvent/MissingEtTruth.h"
#include "TestTools/leakcheck.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/MsgStream.h"
#include "CxxUtils/checker_macros.h"
#include <cassert>
#include <iostream>


void compare (const MissingEtRegions& p1,
              const MissingEtRegions& p2)
{
  assert (p1.exRegVec() == p2.exRegVec());
  assert (p1.eyRegVec() == p2.eyRegVec());
  assert (p1.etSumRegVec() == p2.etSumRegVec());
}


void compare (const MissingET& p1,
              const MissingET& p2)
{
  compare (*p1.getRegions(), *p2.getRegions());
  assert (p1.getSource() == p2.getSource());
  assert (p1.etx() == p2.etx());
  assert (p1.ety() == p2.ety());
  assert (p1.sumet() == p2.sumet());
}


void compare (const MissingEtTruth& p1,
              const MissingEtTruth& p2)
{
  compare (static_cast<const MissingET&> (p1), static_cast<const MissingET&> (p2));
  assert (p1.exTruthVec() == p2.exTruthVec());
  assert (p1.eyTruthVec() == p2.eyTruthVec());
  assert (p1.etSumTruthVec() == p2.etSumTruthVec());
}


void testit (const MissingEtTruth& trans1)
{
  MsgStream log (0, "test");
  MissingEtTruthCnv_p3 cnv;
  MissingEtTruth_p3 pers;
  cnv.transToPers (&trans1, &pers, log);
  MissingEtTruth trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  auto regions = std::make_unique<MissingEtRegions>();
  regions->setExRegVec (std::vector<double> {20.5, 21.5, 22.5});
  regions->setEyRegVec (std::vector<double> {23.5, 24.5, 25.5});
  regions->setEtSumRegVec (std::vector<double> {26.5, 27.5, 28.5});
  MissingEtTruth trans1 (MissingEtTruth::Final, regions.release());
  trans1.setEx (12.5);
  trans1.setEy (100.5);
  trans1.setEtSum (110.5);
  trans1.setExTruthVec (std::vector<double>{20.5, 21.5, 22.5, 23.5, 24.5, 25.5});
  trans1.setEyTruthVec (std::vector<double>{30.5, 31.5, 32.5, 33.5, 34.5, 35.5});
  trans1.setEtSumTruthVec (std::vector<double>{40.5, 41.5, 42.5, 43.5, 44.5, 35.5});

  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE()
{
  std::cout << "RecTPCnv/MissingEtTruthCnv_p3\n";
  test1();
  return 0;
}
