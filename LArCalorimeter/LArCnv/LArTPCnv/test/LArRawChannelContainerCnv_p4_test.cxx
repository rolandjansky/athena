/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file LArTPCnv/test/LArRawChannelContainerCnv_p4_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief Tests for LArRawChannelContainerCnv_p4.
 */


#undef NDEBUG
#include "LArTPCnv/LArRawChannelContainerCnv_p4.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include "CxxUtils/checker_macros.h"
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


void compare (const LArRawChannelContainer& p1,
              const LArRawChannelContainer& p2)
{
  assert (p1.size() == p2.size());
  for (size_t i=0; i < p1.size(); i++)
    compare (p1[i], p2[i]);
}


void testit (const LArRawChannelContainer& trans1)
{
  MsgStream log (0, "test");
  LArRawChannelContainerCnv_p4 cnv;
  LArRawChannelContainer_p4 pers;
  cnv.transToPers (&trans1, &pers, log);
  LArRawChannelContainer trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  CaloGain::CaloGain gains[CaloGain::LARNGAIN] =
    {CaloGain::LARHIGHGAIN, CaloGain::LARMEDIUMGAIN, CaloGain::LARLOWGAIN};

  LArRawChannelContainer trans1;
  for (int i=0; i < 10; i++) {
    int o = i*100;
    trans1.emplace_back (HWIdentifier (0x1234+o),
                         1000+o,
                         1010+o,
                         1020+o,
                         1030+o,
                         gains[i%CaloGain::LARNGAIN]);
  }
    
  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}
