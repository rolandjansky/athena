/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TBTPCnv/test/TBPhaseCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2019
 * @brief Regression tests.
 */

#undef NDEBUG
#include "TBTPCnv/TBPhaseCnv_p1.h"
#include "TBEvent/TBPhase.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TBPhase& p1,
              const TBPhase& p2)
{
  assert (p1.getPhase() == p2.getPhase());
  assert (p1.getPhaseind() == p2.getPhaseind());
  assert (p1.getdTtoWACSigned() == p2.getdTtoWACSigned());
}


void testit (const TBPhase& trans1)
{
  MsgStream log (0, "test");
  TBPhaseCnv_p1 cnv;
  TBPhase_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TBPhase trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  TBPhase trans1 (123.5, 234, 345.5);
                                  
  testit (trans1);
}


int main()
{
  std::cout << "TBTPCnv/test/TBPhaseCnv_p1_test\n";
  test1();
  return 0;
}
