/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file LumiBlockTPCnv/test/LumuBlockRangeCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "LumiBlockTPCnv/LumiBlockRangeCnv_p2.h"
#include "LumiBlockData/LB_IOVRange.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void test1()
{
  std::cout << "test1\n";
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


int main()
{
  test1();
  return 0;
}
