/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file InDetSimEventTPCnv/test/SiHitCollectionCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for SiHitCollectionCnv_p2.
 */


#undef NDEBUG
#include "InDetSimEventTPCnv/InDetHits/SiHitCollectionCnv_p2.h"
#include <cassert>
#include <iostream>


void compare (const SiHit& p1,
              const SiHit& p2)
{
  assert (p1.localStartPosition() == p2.localStartPosition());
  assert (p1.localEndPosition() == p2.localEndPosition());
  assert (p1.energyLoss() == p2.energyLoss());
  assert (p1.meanTime() == p2.meanTime());
  assert (p1.particleLink() == p2.particleLink());
  assert (p1.identify() == p2.identify());
}


void compare (const SiHitCollection& p1,
              const SiHitCollection& p2)
{
  //assert (p1.Name() == p2.Name());
  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++)
    compare (p1[i], p2[i]);
}


void testit (const SiHitCollection& trans1)
{
  MsgStream log (0, "test");
  SiHitCollectionCnv_p2 cnv;
  SiHitCollection_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  SiHitCollection trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  SiHitCollection trans1 ("coll");
  for (int i=0; i < 10; i++) {
    int o = i*100;
    trans1.Emplace (HepGeom::Point3D<double> (10.5+o, 11.5+o, 12.5+o),
                    HepGeom::Point3D<double> (13.5+o, 14.5+o, 15.5+o),
                    16.5+o,
                    17.5+o,
                    18+o,
                    19+o);
    
  }
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
