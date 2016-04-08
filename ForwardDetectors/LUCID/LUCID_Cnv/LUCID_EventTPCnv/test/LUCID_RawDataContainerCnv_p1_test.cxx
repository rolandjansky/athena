/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file LUCID_EventTPCnv/test/LUCID_RawDataContainerCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for LUCID_RawDataContainerCnv_p1.
 */


#undef NDEBUG
#include "LUCID_EventTPCnv/LUCID_RawDataContainerCnv_p1.h"
#include <cassert>
#include <iostream>


void compare (const LUCID_RawData& p1,
              const LUCID_RawData& p2)
{
  assert (p1.getStatus() == p2.getStatus());

  assert (p1.getWord0() == p2.getWord0());
  assert (p1.getWord1() == p2.getWord1());
  assert (p1.getWord2() == p2.getWord2());
  assert (p1.getWord3() == p2.getWord3());

  assert (p1.getWord0p() == p2.getWord0p());
  assert (p1.getWord1p() == p2.getWord1p());
  assert (p1.getWord2p() == p2.getWord2p());
  assert (p1.getWord3p() == p2.getWord3p());

  assert (p1.getWord0n() == p2.getWord0n());
  assert (p1.getWord1n() == p2.getWord1n());
  assert (p1.getWord2n() == p2.getWord2n());
  assert (p1.getWord3n() == p2.getWord3n());
}


void compare (const LUCID_RawDataContainer& p1,
              const LUCID_RawDataContainer& p2)
{
  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++)
    compare (*p1[i], *p2[i]);
}


void testit (const LUCID_RawDataContainer& trans1)
{
  MsgStream log (0, "test");
  LUCID_RawDataContainerCnv_p1 cnv;
  LUCID_RawDataContainer_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  LUCID_RawDataContainer trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  LUCID_RawDataContainer trans1;
  for (int i=0; i < 10; i++) {
    int o = i*100;
    trans1.push_back (new LUCID_RawData (1+o, 2+o, 3+o, 4+o,
                                         5+o, 6+o, 7+o, 8+o,
                                         9+o, 10+o, 11+o, 12+o,
                                         13+o));
  }
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
