/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonEventAthenaPool/test/MdtCsmCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2016
 * @brief Regression tests.
 */

#undef NDEBUG
#include "../src/MdtCsmCnv_p1.h"
#include "TestTools/leakcheck.h"
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


void test1()
{
  std::cout << "test1\n";

  Athena_test::Leakcheck check;

  MdtCsm trans1 (Identifier(123), 234,
                 345, 456, 567);
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
