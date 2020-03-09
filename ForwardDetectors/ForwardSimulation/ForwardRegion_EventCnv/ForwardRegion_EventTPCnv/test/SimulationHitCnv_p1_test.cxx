/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file ForwardRegion_EventTPCnv/test/SimulationHitCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2019
 * @brief Regression tests.
 */

#undef NDEBUG
#include "ForwardRegion_EventTPCnv/SimulationHitCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const CLHEP::Hep3Vector& p1,
              const CLHEP::Hep3Vector& p2)
{
  assert (p1.x() == p2.x());
  assert (p1.y() == p2.y());
  assert (p1.z() == p2.z());
}


void compare (const SimulationHit::StepPoint& p1,
              const SimulationHit::StepPoint& p2)
{
  assert (p1.time == p2.time);
  assert (p2.kinetic_energy == p2.kinetic_energy);
  compare (p1.position, p2.position);
  compare (p1.direction, p2.direction);
  compare (p1.momentum, p2.momentum);
}


void compare (const SimulationHit& p1,
              const SimulationHit& p2)
{
  compare (p1.pre(), p2.pre());
  compare (p1.post(), p2.post());
  assert (p1.depositEnergy() == p2.depositEnergy());
  assert (p1.particleEncoding() == p2.particleEncoding());
  assert (p1.trackID() == p2.trackID());
  assert (p1.parentID() == p2.parentID());
}


void testit (const SimulationHit& trans1)
{
  MsgStream log (0, "test");
  SimulationHitCnv_p1 cnv;
  SimulationHit_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  SimulationHit trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}

void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  SimulationHit trans1 (2.5,
                        CLHEP::Hep3Vector (3.5, 4.5, 5.5),
                        6.5,
                        CLHEP::Hep3Vector (7.5, 8.5, 9.5),
                        CLHEP::Hep3Vector (10.5, 11.5, 12.5),

                        13.5,
                        CLHEP::Hep3Vector (14.5, 15.5, 16.5),
                        17.5,
                        CLHEP::Hep3Vector (18.5, 19.5, 20.5),
                        CLHEP::Hep3Vector (21.5, 22.5, 23.5),

                        24.5,
                        25,
                        26,
                        27);
                                  
  testit (trans1);
}


int main()
{
  std::cout << "ForwardRegion_EventTPCnv/test/SimulationHitCnv_p1_test\n";
  test1();
  return 0;
}
