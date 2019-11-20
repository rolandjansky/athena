/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TBTPCnv/test/TBTDCCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2019
 * @brief Regression tests.
 */

#undef NDEBUG
#include "TBTPCnv/TBTDCCnv_p1.h"
#include "TBEvent/TBTDC.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TBTDC& p1,
              const TBTDC& p2)
{
  assert (p1.tdc() == p2.tdc());
  assert (p1.tdcmin() == p2.tdcmin());
  assert (p1.scale() == p2.scale());
  assert (p1.phase() == p2.phase());
}


void testit (const TBTDC& trans1)
{
  MsgStream log (0, "test");
  TBTDCCnv_p1 cnv;
  TBTDC_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TBTDC trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  TBTDC trans1 (123, 234, 345.5, 456);
                                  
  testit (trans1);
}


int main()
{
  std::cout << "TBTPCnv/test/TBTDCCnv_p1_test\n";
  test1();
  return 0;
}
