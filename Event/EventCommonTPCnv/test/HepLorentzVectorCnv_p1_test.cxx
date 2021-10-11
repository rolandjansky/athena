/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file EventCommonTPCnv/test/HepLorentzVectorCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2018
 * @brief Regression tests.
 */

#undef NDEBUG
#include "EventCommonTPCnv/HepLorentzVectorCnv_p1.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include "CxxUtils/checker_macros.h"
#include <cassert>
#include <iostream>


void compare (const CLHEP::HepLorentzVector& a1,
              const CLHEP::HepLorentzVector& a2)
{
  assert (a1 == a2);
}


void testit (const CLHEP::HepLorentzVector& trans1)
{
  MsgStream log (0, "test");
  HepLorentzVectorCnv_p1 cnv;
  HepLorentzVector_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  CLHEP::HepLorentzVector trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";

  Athena_test::Leakcheck check;

  CLHEP::HepLorentzVector trans1 (1, 2, 3, 4);
  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "HepLorentzVectorCnv_p1_test\n";
  test1();
  return 0;
}
