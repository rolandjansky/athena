/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file MuonEventTPCnv/test/STGC_DigitCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2016
 * @brief Regression tests.
 */

#undef NDEBUG
#include "MuonEventTPCnv/MuonDigitContainer/STGC_DigitCnv_p1.h"
#include "CxxUtils/checker_macros.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const sTgcDigit& p1,
              const sTgcDigit& p2)
{
  assert (p1.identify() == p2.identify());
  assert (p1.bcTag() == p2.bcTag());
  assert (p1.time() == p2.time());
  assert (p1.charge() == p2.charge());
}

void testit (const sTgcDigit& trans1)
{
  MsgStream log (nullptr, "test");
  STGC_DigitCnv_p1 cnv;
  Muon::STGC_Digit_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  sTgcDigit trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  sTgcDigit trans1 (Identifier (1234),
                    123, 4.5, 6.5, false, false);
  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}
