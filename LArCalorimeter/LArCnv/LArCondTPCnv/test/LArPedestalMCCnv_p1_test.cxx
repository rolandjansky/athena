/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file LArCondTPCnv/test/LArPedestalMCCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2020
 * @brief Tests for LArPedestalMCCnv_p1.
 */


#undef NDEBUG
#include "LArCondTPCnv/LArPedestalMCCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


typedef LArPedestalMC TransType;


void compare (const LArPedestalMC& p1,
              const LArPedestalMC& p2)
{
  assert (p1.pedestal(HWIdentifier(), 0) == p2.pedestal(HWIdentifier(), 0));
  assert (p1.pedestalRMS(HWIdentifier(), 0) == p2.pedestalRMS(HWIdentifier(), 0));
}


void testit (const TransType& trans1)
{
  MsgStream log (0, "test");
  LArPedestalMCCnv_p1 cnv;
  LArPedestalMC_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TransType trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  TransType trans1;
  trans1.set (std::vector<float> { 1.5 }, std::vector<float> { 2.5 } );

  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "LArCondTPCnv/LArPedestalMCCnv_p1_test\n";
  test1();
  return 0;
}
