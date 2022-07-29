/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrigMonitoringEventTPCnv/test/TrigMonRoiCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2019
 * @brief Tests for TrigMonRoiCnv_p1.
 */


#undef NDEBUG
#include "TrigMonitoringEventTPCnv/TrigMonRoiCnv_p1.h"
#include "CxxUtils/checker_macros.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TrigMonRoi& p1,
              const TrigMonRoi& p2)
{
  assert ( p1.getWord()   == p2.getWord() );
  assert ( p1.getVarKey() == p2.getVarKey() );
  assert ( p1.getVarVal() == p2.getVarVal() );
}


void testit (const TrigMonRoi& trans1)
{
  MsgStream log (0, "test");
  TrigMonRoiCnv_p1 cnv;
  TrigMonRoi_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigMonRoi trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  TrigMonRoi trans1 (123);
  trans1.addWord (234);
  trans1.addWord (345);
  trans1.setEtaPhi (1.5, 2.5);
  trans1.setRoIArea (0.25, 0.5);
  trans1.addVar (TrigMonVar (13, 12.5));

  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "TrigMonitoringEventTPCnv/TrigMonRoiCnv_p1_test\n";
  test1();
  return 0;
}
