/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file RecTPCnv/test/MuonSpShowerContainerCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2019
 * @brief Regression tests.
 */

#undef NDEBUG
#include "RecTPCnv/MuonSpShowerContainerCnv_p1.h"
#include "muonEvent/MuonSpShowerContainer.h"
#include "TestTools/leakcheck.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/MsgStream.h"
#include "CxxUtils/checker_macros.h"
#include <cassert>
#include <iostream>


void compare (const Rec::MuonSpShower& p1,
              const Rec::MuonSpShower& p2)
{
  assert (p1.eta() == p2.eta());
  assert (p1.phi() == p2.phi());
  assert (p1.numberOfTriggerHits() == p2.numberOfTriggerHits());
  assert (p1.numberOfInnerHits() == p2.numberOfInnerHits());
  assert (p1.numberOfInnerSegments() == p2.numberOfInnerSegments());
  assert (p1.numberOfMiddleHits() == p2.numberOfMiddleHits());
  assert (p1.numberOfMiddleSegments() == p2.numberOfMiddleSegments());
  assert (p1.numberOfOuterHits() == p2.numberOfOuterHits());
  assert (p1.numberOfOuterSegments() == p2.numberOfOuterSegments());
}


void compare (const Rec::MuonSpShowerContainer& p1,
              const Rec::MuonSpShowerContainer& p2)
{
  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++) {
    compare (*p1[i], *p2[i]);
  }
}


void testit (const Rec::MuonSpShowerContainer& trans1)
{
  MsgStream log (0, "test");
  MuonSpShowerContainerCnv_p1 cnv;
  MuonSpShowerContainer_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  Rec::MuonSpShowerContainer trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  Rec::MuonSpShowerContainer trans1;
  for (int i=0; i < 5; i++) {
    int ii = i*100;
    trans1.push_back (std::make_unique<Rec::MuonSpShower>
                      (i+1.5, i+2.5, ii+10, ii+11, ii+12, ii+13, ii+14, ii+15, ii+16));
  }
  
  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE()
{
  std::cout << "RecTPCnv/MuonSpShowerCnv_p1\n";
  test1();
  return 0;
}
