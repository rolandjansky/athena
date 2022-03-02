/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonEventAthenaPool/test/CscRawDataCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2016
 * @brief Regression tests.
 */

#undef NDEBUG
#include "../src/CscRawDataCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const CscRawData& p1,
              const CscRawData& p2)
{
  assert (p1.samples() == p2.samples());
  assert (p1.address() == p2.address());
  assert (p1.identify() == p2.identify());
  //assert (p1.time() == p2.time());
  assert (p1.rpuID() == p2.rpuID());
  //assert (p1.width() == p2.width());
  assert (1 == p2.width());
  //assert (p1.isTimeComputed() == p2.isTimeComputed());
  //assert (p1.hashId() == p2.hashId());
}


void testit (const CscRawData& trans1)
{
  MsgStream log (nullptr, "test");
  CscRawDataCnv_p1 cnv;
  CscRawData_p1 pers;
  //cnv.transToPers (&trans1, &pers, log);

  pers.m_id = trans1.identify();
  for (uint32_t amp : trans1.samples())
    pers.m_amps.push_back (amp - 2048);
  int strip = trans1.address() & 0xff;
  pers.m_address = strip;
  CscRawData trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  Athena_test::Leakcheck check;

  // nb see comment in CscRawDataCnv_p1.cxx on chamberIndex
  CscRawData trans1 (std::vector<uint16_t>(3001, 3002),
                     2048+3, 4, 0, 6);
  testit (trans1);
  CscRawData trans2 (std::vector<uint16_t>(3011, 3012),
                     2048+13, 14, 15, 0, 17, 18);
  testit (trans2);
}


int main()
{
  test1();
  return 0;
}
