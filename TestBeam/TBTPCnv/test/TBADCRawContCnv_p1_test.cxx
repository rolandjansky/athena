/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TBTPCnv/test/TBADCRawContCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2019
 * @brief Regression tests.
 */

#undef NDEBUG
#include "TBTPCnv/TBADCRawContCnv_p1.h"
#include "TBEvent/TBADCRawCont.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TBADCRaw& p1,
              const TBADCRaw& p2)
{
  assert (p1.isOverflow() == p2.isOverflow());
  assert (p1.getDetectorName() == p2.getDetectorName());
  assert (p1.getADC() == p2.getADC());
}


void compare (const TBADCRawCont& p1,
              const TBADCRawCont& p2)
{
  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++) {
    compare (*p1[i], *p2[i]);
  }
}


void testit (const TBADCRawCont& trans1)
{
  MsgStream log (0, "test");
  TBADCRawContCnv_p1 cnv;
  TBADCRawCont_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TBADCRawCont trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  TBADCRawCont trans1;
  trans1.push_back (std::make_unique<TBADCRaw> ("aaa", true,  123));
  trans1.push_back (std::make_unique<TBADCRaw> ("bbb", false, 423));
                                  
  testit (trans1);
}


int main()
{
  std::cout << "TBTPCnv/test/TBADCRawContCnv_p1_test\n";
  test1();
  return 0;
}
