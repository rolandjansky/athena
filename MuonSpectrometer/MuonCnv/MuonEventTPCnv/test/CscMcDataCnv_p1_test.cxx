/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonEventTPCnv/test/CscMcDataCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "MuonEventTPCnv/MuonDigitContainer/CscMcDataCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const CscMcData& p1,
              const CscMcData& p2)
{
  assert (p1.energy() == p2.energy());
  assert (p1.ypos() == p2.ypos());
  assert (p1.zpos() == p2.zpos());
  assert (p1.charge() == p2.charge());
}

void testit (const CscMcData& trans1)
{
  MsgStream log (0, "test");
  CscMcDataCnv_p1 cnv;
  Muon::CscMcData_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  CscMcData trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  CscMcData trans1 (2.5, 3.5, 4.5);
  trans1.setCharge (5.5);
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
