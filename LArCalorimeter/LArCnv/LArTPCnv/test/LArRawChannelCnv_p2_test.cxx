/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file LArTPCnv/test/LArRawChannelCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief Tests for LArRawChannelCnv_p2.
 */


#undef NDEBUG
#include "LArTPCnv/LArRawChannelCnv_p2.h"
#include <cassert>
#include <iostream>


void compare (const LArRawChannel& p1,
              const LArRawChannel& p2)
{
  assert (p1.identify() == p2.identify());
  assert (p1.energy() == p2.energy());
  assert (p1.time() == p2.time());
  assert (p1.gain() == p2.gain());
  assert (p1.quality() == p2.quality());
  assert (p1.provenance() == p2.provenance());
}


void testit (const LArRawChannel& trans1)
{
  MsgStream log (0, "test");
  LArRawChannelCnv_p2 cnv;
  LArRawChannel_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  LArRawChannel trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  LArRawChannel trans1 (HWIdentifier (0x1234),
                        1000,
                        1010,
                        1020,
                        1030,
                        CaloGain::LARMEDIUMGAIN);
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
