/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file InDetSimEventTPCnv/test/SiHitCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for SiHitCnv_p1.
 */


#undef NDEBUG
#include "InDetSimEventTPCnv/InDetHits/SiHitCnv_p1.h"
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


void testit (const SiHit& trans1)
{
  MsgStream log (0, "test");
  SiHitCnv_p1 cnv;
  SiHit_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  SiHit trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  SiHit trans1 (HepGeom::Point3D<double> (10.5, 11.5, 12.5),
                HepGeom::Point3D<double> (13.5, 14.5, 15.5),
                16.5,
                17.5,
                18,
                19);
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
