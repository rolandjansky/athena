/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrigT1EventTPCnv/test/CPMRoICnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Tests for CPMRoICnv_p1.
 */


#undef NDEBUG
#include "TrigT1EventTPCnv/CPMRoICnv_p1.h"
#include "TestTools/leakcheck.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const LVL1::CPMRoI& p1,
              const LVL1::CPMRoI& p2)
{
  assert (p1.roiWord() == p2.roiWord());
}


void testit (const LVL1::CPMRoI& trans1)
{
  MsgStream log (0, "test");
  CPMRoICnv_p1 cnv;
  CPMRoI_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  LVL1::CPMRoI trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  LVL1::CPMRoI trans1 (123345);
  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}
