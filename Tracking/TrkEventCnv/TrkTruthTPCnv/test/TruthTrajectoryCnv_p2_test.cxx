/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrkTruthTPCnv/test/TruthTrajectoryCnv_p2_test.cxx
 * @date Feb, 2018
 * @brief Tests for TruthTrajectoryCnv_p2.
 */


#undef NDEBUG
#include "TrkTruthTPCnv/TruthTrajectoryCnv_p2.h"
#include "TrkTruthTPCnv/TruthTrajectory_p2.h"
#include "TrkTruthData/TruthTrajectory.h"
#include "SGTools/TestStore.h"
#include "GeneratorObjectsTPCnv/initMcEventCollection.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"
#include <cassert>
#include <iostream>


void compare (const HepMcParticleLink& p1,
              const HepMcParticleLink& p2)
{
  assert ( p1.isValid() == p2.isValid() );
  assert ( p1.barcode() == p2.barcode() );
  assert ( p1.eventIndex() == p2.eventIndex() );
  assert ( p1.cptr() == p2.cptr() );
  assert ( p1 == p2 );
}


void compare (const TruthTrajectory& p1,
              const TruthTrajectory& p2)
{
  assert (p1.size() == p2.size());
  TruthTrajectory::const_iterator i1 = p1.begin();
  TruthTrajectory::const_iterator i2 = p2.begin();
  for (; i1 != p1.end(); ++i1, ++i2) {
    compare (*i1, *i2);
    assert (*i1 == *i2);
  }
}


void testit (const TruthTrajectory& trans1)
{
  MsgStream log (0, "test");
  TruthTrajectoryCnv_p2 cnv;
  Trk::TruthTrajectory_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  TruthTrajectory trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1(std::vector<HepMC::GenParticle*> genPartVector)
{
  std::cout << "test1\n";

  TruthTrajectory trans1;
  for (int i=0; i<10; i++) {
    const HepMC::GenParticle* pGenParticle = genPartVector.at(i);
    HepMcParticleLink trkLink(pGenParticle->barcode(),pGenParticle->parent_event()->event_number());
    trans1.push_back(trkLink);
  }

  testit (trans1);
}


int main()
{
  ISvcLocator* pSvcLoc = nullptr;
  std::vector<HepMC::GenParticle*> genPartVector;
  if (!Athena_test::initMcEventCollection(pSvcLoc,genPartVector)) {
    std::cerr << "This test can not be run" << std::endl;
    return 0;
  }

  //SGTest::initTestStore();
  test1(genPartVector);
  return 0;
}
