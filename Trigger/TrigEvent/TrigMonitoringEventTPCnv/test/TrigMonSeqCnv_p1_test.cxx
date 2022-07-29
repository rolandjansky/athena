/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrigMonitoringEventTPCnv/test/TrigMonSeqCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2019
 * @brief Tests for TrigMonSeqCnv_p1.
 */


#undef NDEBUG
#include "TrigMonitoringEventTPCnv/TrigMonSeqCnv_p1.h"
#include "CxxUtils/checker_macros.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TrigMonAlg& p1,
              const TrigMonAlg& p2)
{
  assert ( p1.getByte() == p2.getByte() );
  assert ( p1.getWord() == p2.getWord() );
}


void compare (const TrigMonSeq& p1,
              const TrigMonSeq& p2)
{
  assert ( p1.getEncoded() == p2.getEncoded() );
  assert ( p1.getVarKey()  == p2.getVarKey() );
  assert ( p1.getVarVal()  == p2.getVarVal() );

  assert ( p1.getAlg().size() == p2.getAlg().size() );
  for (size_t i = 0; i < p1.getAlg().size(); i++) {
    compare (p1.getAlg()[i], p2.getAlg()[i]);
  }
}


void testit (const TrigMonSeq& trans1)
{
  MsgStream log (0, "test");
  TrigMonSeqCnv_p1 cnv;
  TrigMonSeq_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigMonSeq trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


TrigMonAlg makeMonAlg (int i)
{
  TrigMonAlg alg ((i+123)%128, (i%100)&1);
  alg.addRoiId (i+45);
  alg.addRoiId (i+67);
  alg.addTimer (TrigMonTimer (i+345, i+543),
                TrigMonTimer (i+654656, i+23423));
  return alg;
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  TrigMonSeq trans1 (123);
  trans1.addVar (TrigMonVar (4, 4.5));
  trans1.addVar (TrigMonVar (6, 7.5));
  trans1.addAlg (makeMonAlg (100));
  trans1.addAlg (makeMonAlg (200));
  trans1.addAlg (makeMonAlg (300));

  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "TrigMonitoringEventTPCnv/TrigMonSeqCnv_p1_test\n";
  test1();
  return 0;
}
