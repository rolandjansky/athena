/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file EventCommonTPCnv/test/P4ImplEEtaPhiMCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2018
 * @brief Regression tests.
 */

#undef NDEBUG
#include "EventCommonTPCnv/P4ImplEEtaPhiMCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include "CxxUtils/checker_macros.h"
#include <cassert>
#include <iostream>


void compare (const P4ImplEEtaPhiM& a1,
              const P4ImplEEtaPhiM& a2)
{
  assert (a1.e() == a2.e());
  assert (a1.eta() == a2.eta());
  assert (a1.phi() == a2.phi());
  assert (a1.m() == a2.m());
}


void testit (const P4ImplEEtaPhiM& trans1)
{
  MsgStream log (0, "test");
  P4ImplEEtaPhiMCnv_p1 cnv;
  P4ImplEEtaPhiM_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  P4ImplEEtaPhiM trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";

  Athena_test::Leakcheck check;

  P4ImplEEtaPhiM trans1 (1, 2.5, 1.5, 4);
  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "P4ImplEEtaPhiMCnv_p1_test\n";
  test1();
  return 0;
}
