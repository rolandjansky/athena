/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrigMonitoringEventTPCnv/test/TrigMonAlgCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2019
 * @brief Tests for TrigMonAlgCnv_p1.
 */


#undef NDEBUG
#include "TrigMonitoringEventTPCnv/TrigMonAlgCnv_p1.h"
#include "CxxUtils/checker_macros.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TrigMonAlg& p1,
              const TrigMonAlg& p2)
{
  assert ( p1.getByte() == p2.getByte() );
  assert ( p1.getWord() == p2.getWord() );
}


void testit (const TrigMonAlg& trans1)
{
  MsgStream log (0, "test");
  TrigMonAlgCnv_p1 cnv;
  TrigMonAlg_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigMonAlg trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  TrigMonAlg trans1 (123, false);
  trans1.addRoiId (45);
  trans1.addRoiId (67);
  trans1.addTimer (TrigMonTimer (345, 543),
                   TrigMonTimer (654656, 23423));

  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "TrigMonitoringEventTPCnv/TrigMonAlgCnv_p1_test\n";
  test1();
  return 0;
}
