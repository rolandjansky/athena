/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonEventTPCnv/test/MM_DigitCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2016
 * @brief Regression tests.
 */

#undef NDEBUG
#include "MuonEventTPCnv/MuonDigitContainer/MM_DigitCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const MmDigit& p1,
              const MmDigit& p2)
{
  assert (p1.identify() == p2.identify());
  assert (p1.stripResponseTime() == p2.stripResponseTime());
  assert (p1.stripResponseCharge() == p2.stripResponseCharge());
  assert (p1.stripResponsePosition() == p2.stripResponsePosition());
  assert (p1.chipResponseTime() == p2.chipResponseTime());
  assert (p1.chipResponsePosition() == p2.chipResponsePosition());
  assert (p2.stripTimeForTrigger().size() == 1);
  assert (p1.stripTimeForTrigger()[0] == p2.stripTimeForTrigger()[0]);
  assert (p2.stripPositionForTrigger().size() == 1);
  assert (p1.stripPositionForTrigger()[0] == p2.stripPositionForTrigger()[0]);
  assert (p2.stripChargeForTrigger().empty());
  assert (p2.MMFE_VMM_idForTrigger().empty());
  assert (p2.VMM_idForTrigger().empty());
}

void testit (const MmDigit& trans1)
{
  MsgStream log (0, "test");
  MM_DigitCnv_p1 cnv;
  Muon::MM_Digit_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  MmDigit trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  MmDigit trans1 (Identifier (1234),
                  std::vector<float> {1.5, 2.5},
                  std::vector<int> {3, 4, 5},
                  std::vector<float> {5.5, 6.5, 7.5},
                  std::vector<float> {8.5, 9.5, 10.5, 11.5},
                  std::vector<int> {12, 13},
                  std::vector<float> {14.5},
                  std::vector<float> {15.5, 16.5, 17.5, 18.5, 19.5},
                  std::vector<int> {20, 21, 22, 23},
                  std::vector<float> {24.5, 26.5},
                  std::vector<int> {27},
                  std::vector<int> {28, 29}
                  );
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
