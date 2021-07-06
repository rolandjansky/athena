/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrigParticleTPCnv/test/TrigL2BphysCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for TrigL2BphysCnv_p2.
 */


#undef NDEBUG
#include "TrigParticleTPCnv/TrigL2BphysCnv_p2.h"
#include "SGTools/TestStore.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <cassert>
#include <iostream>


void compare (const TrigL2Bphys& p1,
              const TrigL2Bphys& p2)
{
  assert (p1.roiId() == p2.roiId());
  assert (p1.particleType() == p2.particleType());
  assert (p1.eta() == p2.eta());
  assert (p1.phi() == p2.phi());
  assert (p1.mass() == p2.mass());
  assert (p1.fitmass() == p2.fitmass());
  assert (p1.fitchi2() == p2.fitchi2());
  assert (p1.fitndof() == p2.fitndof());
  assert (p1.fitx() == p2.fitx());
  assert (p1.fity() == p2.fity());
  assert (p1.fitz() == p2.fitz());
  assert (p1.secondaryDecayLink() == p2.secondaryDecayLink());
  assert (p1.trackVector() == p2.trackVector());
}


void testit (const TrigL2Bphys& trans1)
{
  MsgStream log (0, "test");
  TrigL2BphysCnv_p2 cnv;
  TrigL2Bphys_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigL2Bphys trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  (void)Gaudi::Hive::currentContext();
  // Get proxies created outside of leak check.
  ElementLink<TrigL2BphysContainer> foo ("foo", 10);
  ElementLink<TrigInDetTrackCollection> bar1 ("bar1", 1);
  ElementLink<TrigInDetTrackCollection> bar2 ("bar2", 2);
  Athena_test::Leakcheck check;

  TrigL2Bphys trans1 (123, 2.5, 1.5,
                      TrigL2Bphys::BMUMU,
                      3.5,
                      ElementLink<TrigL2BphysContainer> ("foo", 10));
  trans1.fitmass (4.5);
  trans1.fitchi2 (5.5);
  trans1.fitndof (6);
  trans1.fitx (6.5);
  trans1.fity (7.5);
  trans1.fitz (8.5);

  trans1.addTrack (ElementLink<TrigInDetTrackCollection> ("bar1", 1));
  trans1.addTrack (ElementLink<TrigInDetTrackCollection> ("bar2", 2));
    
  testit (trans1);
}


int main()
{
  SGTest::initTestStore();
  test1();
  return 0;
}
