/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonEventTPCnv/test/STGC_DigitCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2016
 * @brief Regression tests.
 */

#undef NDEBUG
#include "MuonEventTPCnv/MuonDigitContainer/STGC_DigitCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const sTgcDigit& p1,
              const sTgcDigit& p2)
{
  assert (p1.identify() == p2.identify());
  assert (p1.bcTag() == p2.bcTag());
  assert (p2.time() == 0); // not saved
  assert (p1.charge() == p2.charge());
}

void testit (const sTgcDigit& trans1)
{
  MsgStream log (0, "test");
  STGC_DigitCnv_p1 cnv;
  Muon::STGC_Digit_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  sTgcDigit trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  sTgcDigit trans1 (Identifier (1234),
                    123, 4.5, 6.5, false, false);
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
