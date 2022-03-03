/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonEventAthenaPool/test/MdtAmtHitCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2016
 * @brief Regression tests.
 */

#undef NDEBUG
#include "../src/MdtAmtHitCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const MdtAmtHit& p1,
              const MdtAmtHit& p2)
{
  assert (p1.tdcId() == p2.tdcId());
  assert (p1.channelId() == p2.channelId());
  assert (p1.leading() == p2.leading());
  assert (p1.coarse() == p2.coarse());
  assert (p1.fine() == p2.fine());
  assert (p1.width() == p2.width());
  assert (p1.isMasked() == p2.isMasked());
  assert (*p1.dataWords() == *p2.dataWords());
}


void testit (const MdtAmtHit& trans1)
{
  MsgStream log (nullptr, "test");
  MdtAmtHitCnv_p1 cnv;
  MdtAmtHit_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  MdtAmtHit trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  Athena_test::Leakcheck check;

  MdtAmtHit trans1 (123, 234, true);
  trans1.setTdcCounts (345, 456);
  trans1.setWidth (567);
  trans1.setLeading (678);
  trans1.addData (32);
  trans1.addData (43);
  trans1.addData (54);
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
