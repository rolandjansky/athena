/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file LArTPCnv/test/LArDigitContainerCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief Tests for LArDigitContainerCnv_p1.
 */


#undef NDEBUG
#include "LArTPCnv/LArDigitContainerCnv_p1.h"
#include "LArRawEvent/LArDigit.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include "CxxUtils/checker_macros.h"
#include <cassert>
#include <iostream>


void compare (const LArDigit& p1,
              const LArDigit& p2)
{
  assert (p1.channelID() == p2.channelID());
  assert (p1.gain() == p2.gain());
  assert (p1.samples() == p2.samples());
}


void compare (const LArDigitContainer& p1,
              const LArDigitContainer& p2)
{
  //assert (p1.Name() == p2.Name());
  assert (p1.size() == p2.size());
  for (size_t i=0; i < p1.size(); i++)
    compare (*p1[i], *p2[i]);
}


void testit (const LArDigitContainer& trans1)
{
  MsgStream log (0, "test");
  LArDigitContainerCnv_p1 cnv;
  LArDigitContainer_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  LArDigitContainer trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  CaloGain::CaloGain gains[CaloGain::LARNGAIN] =
    {CaloGain::LARHIGHGAIN, CaloGain::LARMEDIUMGAIN, CaloGain::LARLOWGAIN};

  LArDigitContainer trans1;
  for (int i=0; i < 10; i++) {
    short o = i*100;
    trans1.push_back (new LArDigit (HWIdentifier (1234+o),
                                    gains[i%CaloGain::LARNGAIN],
                                    std::vector<short> {
                                        (short)(1+o),
                                        (short)(2+o),
                                        (short)(3+o)}));
  }
    
  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}
