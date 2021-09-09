/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file EventCommonTPCnv/test/P4IPtCotThPhiMCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2018
 * @brief Regression tests.
 */

#undef NDEBUG
#include "EventCommonTPCnv/P4IPtCotThPhiMCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include "CxxUtils/checker_macros.h"
#include <cassert>
#include <iostream>


void compare (const P4IPtCotThPhiM& a1,
              const P4IPtCotThPhiM& a2)
{
  assert (a1.iPt() == a2.iPt());
  assert (a1.cotTh() == a2.cotTh());
  assert (a1.phi() == a2.phi());
  assert (a1.m() == a2.m());
}


void testit (const P4IPtCotThPhiM& trans1)
{
  MsgStream log (0, "test");
  P4IPtCotThPhiMCnv_p1 cnv;
  P4IPtCotThPhiM_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  P4IPtCotThPhiM trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";

  Athena_test::Leakcheck check;

  P4IPtCotThPhiM trans1 (1, 2.5, 1.5, 4);
  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "P4IPtCotThPhiMCnv_p1_test\n";
  test1();
  return 0;
}
