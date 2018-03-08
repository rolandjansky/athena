/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file LUCID_EventTPCnv/test/LUCID_SimHitCnv_p2_test.cxx
 * @date Feb, 2018
 * @brief Tests for LUCID_SimHitCnv_p2.
 */


#undef NDEBUG
#include "LUCID_EventTPCnv/LUCID_SimHitCnv_p2.h"
#include "TestTools/leakcheck.h"
#include <cassert>
#include <iostream>

#include "GeneratorObjectsTPCnv/initMcEventCollection.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"


void compare (const HepMcParticleLink& p1,
              const HepMcParticleLink& p2)
{
  assert ( p1.isValid() == p2.isValid() );
  assert ( p1.barcode() == p2.barcode() );
  assert ( p1.eventIndex() == p2.eventIndex() );
  assert ( p1.cptr() == p2.cptr() );
  assert ( p1 == p2 );
}


void compare (const LUCID_SimHit& p1,
              const LUCID_SimHit& p2)
{
  assert (p1.GetTubeID() == p2.GetTubeID());
  assert (p1.GetPdgCode() == p2.GetPdgCode());
  assert (p1.GetTrack() == p2.GetTrack());
  compare (p1.particleLink(), p2.particleLink());
  assert (p1.particleLink() == p2.particleLink());
  assert (p1.GetGenVolume() == p2.GetGenVolume());
  assert (p1.GetX() == p2.GetX());
  assert (p1.GetY() == p2.GetY());
  assert (p1.GetZ() == p2.GetZ());
  assert (p1.GetEPX() == p2.GetEPX());
  assert (p1.GetEPY() == p2.GetEPY());
  assert (p1.GetEPZ() == p2.GetEPZ());
  assert (p1.GetPreStepTime() == p2.GetPreStepTime());
  assert (p1.GetPostStepTime() == p2.GetPostStepTime());
  assert (p1.GetWavelength() == p2.GetWavelength());
  assert (p1.GetEnergy() == p2.GetEnergy());
}


void testit (const LUCID_SimHit& trans1)
{
  MsgStream log (0, "test");
  LUCID_SimHitCnv_p2 cnv;
  LUCID_SimHit_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  LUCID_SimHit trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1(std::vector<HepMC::GenParticle*>& genPartVector)
{
  std::cout << "test1\n";
  const HepMC::GenParticle *particle = genPartVector.at(0);
  // Create HepMcParticleLink outside of leak check.
  HepMcParticleLink dummyHMPL(particle->barcode(),particle->parent_event()->event_number());
  assert(dummyHMPL.cptr()==particle);
  Athena_test::Leakcheck check;

  HepMcParticleLink trkLink(genPartVector.at(0)->barcode(),genPartVector.at(0)->parent_event()->event_number());
  LUCID_SimHit trans1 (1, genPartVector.at(0)->pdg_id(), trkLink, 4,
                       5.5, 6.5, 7.5,
                       8.5, 9.5, 10.5,
                       11.5, 12.5, 13.5, 14.5);

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

  test1(genPartVector);
  return 0;
}
