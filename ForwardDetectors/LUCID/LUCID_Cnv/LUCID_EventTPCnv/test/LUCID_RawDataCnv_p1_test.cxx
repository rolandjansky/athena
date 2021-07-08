/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file LUCID_EventTPCnv/test/LUCID_RawDataCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for LUCID_RawDataCnv_p1.
 */


#undef NDEBUG
#include "LUCID_EventTPCnv/LUCID_RawDataCnv_p1.h"
#include "TestTools/leakcheck.h"
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


void testit (const LUCID_RawData& trans1)
{
  MsgStream log (nullptr, "test");
  LUCID_RawDataCnv_p1 cnv;
  LUCID_RawData_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  LUCID_RawData trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  LUCID_RawData trans1 (1, 2, 3, 4,
                        5, 6, 7, 8,
                        9, 10, 11, 12,
                        13);
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
