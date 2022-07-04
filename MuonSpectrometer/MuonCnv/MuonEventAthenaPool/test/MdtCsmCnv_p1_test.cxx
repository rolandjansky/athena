/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file MuonEventAthenaPool/test/MdtCsmCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2016
 * @brief Regression tests.
 */

#undef NDEBUG
#include "../src/MdtCsmCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const MdtCsm& p1,
              const MdtCsm& p2)
{
  assert (p1.identify() == p2.identify());
  assert (p1.identifyHash() == p2.identifyHash());
  assert (p1.SubDetId() == p2.SubDetId());
  assert (p1.MrodId() == p2.MrodId());
  assert (p1.CsmId() == p2.CsmId());
}


void testit (const MdtCsm& trans1)
{
  MsgStream log (nullptr, "test");
  MdtCsmCnv_p1 cnv;
  MdtCsm_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  MdtCsm trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";

  Athena_test::Leakcheck check;

  MdtCsm trans1 (Identifier(123), 234,
                 345, 456, 567);
  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}
