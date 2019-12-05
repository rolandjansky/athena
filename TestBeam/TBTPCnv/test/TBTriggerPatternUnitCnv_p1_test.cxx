/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TBTPCnv/test/TBTriggerPatternUnitCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2019
 * @brief Regression tests.
 */

#undef NDEBUG
#include "TBTPCnv/TBTriggerPatternUnitCnv_p1.h"
#include "TBEvent/TBTriggerPatternUnit.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TBTriggerPatternUnit& p1,
              const TBTriggerPatternUnit& p2)
{
  assert (p1.getTriggerWord() == p2.getTriggerWord());
  assert (p1.getTriggers() == p2.getTriggers());
}


void testit (const TBTriggerPatternUnit& trans1)
{
  MsgStream log (0, "test");
  TBTriggerPatternUnitCnv_p1 cnv;
  TBTriggerPatternUnit_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TBTriggerPatternUnit trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  TBTriggerPatternUnit trans1;
  trans1.setTriggerWord (1234);
                                  
  testit (trans1);
}


int main()
{
  std::cout << "TBTPCnv/test/TBTriggerPatternUnitCnv_p1_test\n";
  test1();
  return 0;
}
