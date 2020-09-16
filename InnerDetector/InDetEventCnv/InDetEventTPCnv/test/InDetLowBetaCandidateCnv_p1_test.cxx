/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file InDetEventTPCnv/test/InDetLowBetaCandidateCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "InDetEventTPCnv/InDetLowBetaInfo/InDetLowBetaCandidateCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const InDet::InDetLowBetaCandidate& p1,
              const InDet::InDetLowBetaCandidate& p2)
{
  assert (p1.getTRTCorrBitsOverThreshold() == p2.getTRTCorrBitsOverThreshold());
  assert (p1.getTRTTrailingEdge() == p2.getTRTTrailingEdge());
  assert (p1.getTRTTrailingEdgeError() == p2.getTRTTrailingEdgeError());
  assert (p1.getTRTNLastBits() == p2.getTRTNLastBits());
  assert (p1.getTRTdEdx() == p2.getTRTdEdx());
  assert (p1.getTRTLikelihoodBeta() == p2.getTRTLikelihoodBeta());
  assert (p1.getTRTLikelihoodError() == p2.getTRTLikelihoodError());
  assert (p1.getTRTHighTbits() == p2.getTRTHighTbits());
}


void testit (const InDet::InDetLowBetaCandidate& trans1)
{
  MsgStream log (0, "test");
  InDetLowBetaCandidateCnv_p1 cnv;
  InDetLowBetaCandidate_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  InDet::InDetLowBetaCandidate trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  InDet::InDetLowBetaCandidate trans1 (1.5, 2.5, 3.5, 4,
                                       5.5, 6.5, 7.5, 8.5);

  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}
