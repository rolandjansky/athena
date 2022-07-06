/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file LumiBlockTPCnv/test/LumuBlockRangeCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "LumiBlockTPCnv/LumiBlockRangeCnv_p2.h"
#include "TestTools/leakcheck.h"
#include "LumiBlockData/LB_IOVRange.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;
  MsgStream log (0, "test");

  LB_IOVRange trans1 (IOVTime(123, 234), IOVTime(123, 987));
  trans1.setNumExpected (10);
  trans1.setNumSeen (20);
  LumiBlockRangeCnv_p2 cnv;
  LumiBlockRange_p2 pers;
  cnv.transToPers (&trans1, &pers, log);

  LB_IOVRange trans2;
  cnv.persToTrans (&pers, &trans2, log);

  assert (trans1 == trans2);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}
