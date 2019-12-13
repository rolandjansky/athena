/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TBTPCnv/test/TBTDCRawContCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2019
 * @brief Regression tests.
 */

#undef NDEBUG
#include "TBTPCnv/TBTDCRawContCnv_p1.h"
#include "TBEvent/TBTDCRawCont.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TBTDCRaw& p1,
              const TBTDCRaw& p2)
{
  assert (p1.isOverflow() == p2.isOverflow());
  assert (p1.getDetectorName() == p2.getDetectorName());
  assert (p1.getTDC() == p2.getTDC());
  assert (p1.isUnderThreshold() == p2.isUnderThreshold());
}


void compare (const TBTDCRawCont& p1,
              const TBTDCRawCont& p2)
{
  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++) {
    compare (*p1[i], *p2[i]);
  }
}


void testit (const TBTDCRawCont& trans1)
{
  MsgStream log (0, "test");
  TBTDCRawContCnv_p1 cnv;
  TBTDCRawCont_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TBTDCRawCont trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  TBTDCRawCont trans1;
  trans1.push_back (std::make_unique<TBTDCRaw> ("aaa", true, 123, false));
  trans1.push_back (std::make_unique<TBTDCRaw> ("bbb", false, 423, true));
                                  
  testit (trans1);
}


int main()
{
  std::cout << "TBTPCnv/test/TBTDCRawContCnv_p1_test\n";
  test1();
  return 0;
}
