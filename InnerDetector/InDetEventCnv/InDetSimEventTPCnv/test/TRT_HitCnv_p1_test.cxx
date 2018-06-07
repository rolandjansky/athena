/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file InDetSimEventTPCnv/test/TRT_HitCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for TRT_HitCnv_p1.
 */


#undef NDEBUG
#include "InDetSimEventTPCnv/InDetHits/TRT_HitCnv_p1.h"
#include <cassert>
#include <iostream>


void compare (const TRTUncompressedHit& p1,
              const TRTUncompressedHit& p2)
{
  assert (p1.GetHitID() == p2.GetHitID());
  assert (p1.particleLink() == p2.particleLink());
  assert (p1.GetParticleEncoding() == p2.GetParticleEncoding());
  assert (p1.GetKineticEnergy() == p2.GetKineticEnergy());
  assert (p1.GetEnergyDeposit() == p2.GetEnergyDeposit());
  assert (p1.GetPreStepX() == p2.GetPreStepX());
  assert (p1.GetPreStepY() == p2.GetPreStepY());
  assert (p1.GetPreStepZ() == p2.GetPreStepZ());
  assert (p1.GetPostStepX() == p2.GetPostStepX());
  assert (p1.GetPostStepY() == p2.GetPostStepY());
  assert (p1.GetPostStepZ() == p2.GetPostStepZ());
  assert (p1.GetGlobalTime() == p2.GetGlobalTime());
}


void testit (const TRTUncompressedHit& trans1)
{
  MsgStream log (0, "test");
  TRT_HitCnv_p1 cnv;
  TRT_Hit_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TRTUncompressedHit trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  TRTUncompressedHit trans1 (101, 102, 103,
                             104.5, 105.5,
                             106.5, 107.5, 108.5, 
                             109.5, 110.5, 111.5,
                             112.5);
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
