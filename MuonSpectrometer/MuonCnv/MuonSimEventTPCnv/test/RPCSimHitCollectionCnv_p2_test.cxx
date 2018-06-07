/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonSimEventTPCnv/test/RPCSimHitCollectionCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief Tests for RPCSimHitCollectionCnv_p2.
 */


#undef NDEBUG
#include "MuonSimEventTPCnv/RPCSimHitCollectionCnv_p2.h"
#include <cassert>
#include <iostream>


void compare (const RPCSimHit& p1,
              const RPCSimHit& p2)
{
  assert (p1.RPCid() == p2.RPCid());
  assert (p1.globalTime() == p2.globalTime());
  assert (p1.localPosition() == p2.localPosition());
  assert (p1.particleLink() == p2.particleLink());
  assert (p1.postLocalPosition() == p2.postLocalPosition());
  assert (p1.energyDeposit() == p2.energyDeposit());
  assert (p1.stepLength() == p2.stepLength());
  assert (p1.particleEncoding() == p2.particleEncoding());
  assert (p1.kineticEnergy() == p2.kineticEnergy());
}


void compare (const RPCSimHitCollection& p1,
              const RPCSimHitCollection& p2)
{
  //assert (p1.Name() == p2.Name());
  assert (p1.size() == p2.size());
  for (size_t i=0; i < p1.size(); i++)
    compare (p1[i], p2[i]);
}


void testit (const RPCSimHitCollection& trans1)
{
  MsgStream log (0, "test");
  RPCSimHitCollectionCnv_p2 cnv;
  Muon::RPCSimHitCollection_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  RPCSimHitCollection trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  RPCSimHitCollection trans1 ("coll");
  for (int i=0; i < 10; i++) {
    trans1.Emplace (123, 10.5,
                    Amg::Vector3D (12.5, 13.5, 14.5),
                    15,
                    Amg::Vector3D (16.5, 17.5, 18.5),
                    19.5, 20.5, 21, 22.5);
  }
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
