/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TBTPCnv/test/TBTrackCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2019
 * @brief Regression tests.
 */

#undef NDEBUG
#include "TBTPCnv/TBTrackCnv_p1.h"
#include "TBEvent/TBTrack.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TBTrack& p1,
              const TBTrack& p2)
{
  assert (p1.getHitNumberU() == p2.getHitNumberU());
  assert (p1.getHitNumberV() == p2.getHitNumberV());
  assert (p1.getResidualu() == p2.getResidualu());
  assert (p1.getResidualv() == p2.getResidualv());
  assert (p1.getChi2_global() == p2.getChi2_global());
  assert (p1.getChi2_u() == p2.getChi2_u());
  assert (p1.getChi2_v() == p2.getChi2_v());
  assert (p1.getAngle() == p2.getAngle());
  assert (p1.getUslope() == p2.getUslope());
  assert (p1.getVslope() == p2.getVslope());
  assert (p1.getUintercept() == p2.getUintercept());
  assert (p1.getVintercept() == p2.getVintercept());
  assert (p1.getCryoHitu() == p2.getCryoHitu());
  assert (p1.getCryoHitv() == p2.getCryoHitv());
  assert (p1.getCryoHitw() == p2.getCryoHitw());
}


void testit (const TBTrack& trans1)
{
  MsgStream log (0, "test");
  TBTrackCnv_p1 cnv;
  TBTrack_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TBTrack trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  TBTrack trans1 (3, 4);
  for (int i=0; i < 3; i++) {
    trans1.setResidualu (i, 1.5 + i);
  }
  for (int i=0; i < 4; i++) {
    trans1.setResidualv (i, 10.5 + i);
  }

  trans1.setChi2 (20.5);
  trans1.setChi2_u (21.5);
  trans1.setChi2_v (22.5);
  trans1.setAngle (23.5);
  trans1.setUslope (24.5);
  trans1.setVslope (25.5);
  trans1.setUintercept (26.5);
  trans1.setVintercept (27.5);
  trans1.setCryoHitu (28.5);
  trans1.setCryoHitv (29.5);
  trans1.setCryoHitw (30.5);

  testit (trans1);
}


int main()
{
  std::cout << "TBTPCnv/test/TBTrackCnv_p1_test\n";
  test1();
  return 0;
}
