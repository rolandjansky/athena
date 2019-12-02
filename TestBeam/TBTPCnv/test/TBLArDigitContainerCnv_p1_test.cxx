/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TBTPCnv/test/TBLArDigitContainerCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2019
 * @brief Regression tests.
 */

#undef NDEBUG
#include "TBTPCnv/TBLArDigitContainerCnv_p1.h"
#include "TBEvent/TBLArDigitContainer.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const LArDigit& p1,
              const LArDigit& p2)
{
  assert (p1.hardwareID() == p2.hardwareID());
  assert (p1.gain() == p2.gain());
  assert (p1.samples() == p2.samples());
}


void compare (const TBLArDigitContainer& p1,
              const TBLArDigitContainer& p2)
{
  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++) {
    compare (*p1[i], *p2[i]);
  }
}


void testit (const TBLArDigitContainer& trans1)
{
  MsgStream log (0, "test");
  TBLArDigitContainerCnv_p1 cnv;
  TBLArDigitContainer_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TBLArDigitContainer trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  TBLArDigitContainer trans1;
  trans1.push_back (std::make_unique<LArDigit> (HWIdentifier (123),
                                                CaloGain::LARMEDIUMGAIN,
                                                std::vector<short> {3, 2, 1} ));
  trans1.push_back (std::make_unique<LArDigit> (HWIdentifier (564),
                                                CaloGain::LARHIGHGAIN,
                                                std::vector<short> {9, 3, 6} ));
                                  
  testit (trans1);
}


int main()
{
  std::cout << "TBTPCnv/test/TBLArDigitContainerCnv_p1_test\n";
  test1();
  return 0;
}
