/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file InDetSimEventTPCnv/test/TRT_HitCollectionCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for TRT_HitCollectionCnv_p2.
 */


#undef NDEBUG
#include "InDetSimEventTPCnv/InDetHits/TRT_HitCollectionCnv_p2.h"
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


void compare (const TRTUncompressedHitCollection& p1,
              const TRTUncompressedHitCollection& p2)
{
  //assert (p1.Name() == p2.Name());
  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++)
    compare (p1[i], p2[i]);
}


void testit (const TRTUncompressedHitCollection& trans1)
{
  MsgStream log (0, "test");
  TRT_HitCollectionCnv_p2 cnv;
  TRT_HitCollection_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  TRTUncompressedHitCollection trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  TRTUncompressedHitCollection trans1 ("coll");
  for (int i=0; i < 10; i++) {
    int o = i*100;
    trans1.Emplace (101+o, 102+o, 20+o,
                    104.5+o, 105.5+o,
                    (106.5+o)/1000, (107.5+o)/1000, 108.5+o, 
                    (109.5+o)/1000, (110.5+o)/1000, 111.5+o,
                    112.5+o);
  }
  trans1.Emplace (131, 132, 22,
                  134.5, 135.5,
                  10, 3, 138.5,
                  3, 10, 148.5,
                  142.5);
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
