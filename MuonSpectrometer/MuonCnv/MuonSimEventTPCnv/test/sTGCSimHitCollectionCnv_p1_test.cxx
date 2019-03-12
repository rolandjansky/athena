/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonSimEventTPCnv/test/sTGCSimHitCollectionCnv_p1_test.cxx
 * @author Jing Chen <jing.chen@cern.ch>
 * @date Jul, 2018
 * @brief Tests for sTGCSimHitCollectionCnv_p1.
 */


#undef NDEBUG
#include "MuonSimEventTPCnv/sTGCSimHitCollectionCnv_p1.h"
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
  assert ( p1.getEventCollectionAsChar() == p2.getEventCollectionAsChar() );
  assert ( p1.cptr() == p2.cptr() );
  assert ( p1 == p2 );
}

void compare (const sTGCSimHit& p1,
              const sTGCSimHit& p2)
{
  assert (p1.sTGCId() == p2.sTGCId());
  assert (p1.globalTime() == p2.globalTime());
  assert (p1.globalPosition() == p2.globalPosition());
  assert (p1.particleEncoding() == p2.particleEncoding());
  assert (p1.globalDirection() == p2.globalDirection());
  assert (p1.depositEnergy() == p2.depositEnergy());
  compare(p1.particleLink(), p2.particleLink());
  assert (p1.particleLink() == p2.particleLink());
}


void compare (const sTGCSimHitCollection& p1,
              const sTGCSimHitCollection& p2)
{
  //assert (p1.Name() == p2.Name());
  assert (p1.size() == p2.size());
  for (size_t i=0; i < p1.size(); i++)
    compare (p1[i], p2[i]);
}


void testit (const sTGCSimHitCollection& trans1)
{
  MsgStream log (0, "test");
  sTGCSimHitCollectionCnv_p1 cnv;
  Muon::sTGCSimHitCollection_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  sTGCSimHitCollection trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1(std::vector<HepMC::GenParticle*> genPartVector)
{
  std::cout << "test1\n";
  const HepMC::GenParticle *particle = genPartVector.at(0);
  // Create HepMcParticleLink outside of leak check.
  HepMcParticleLink dummyHMPL(particle->barcode(),particle->parent_event()->event_number());
  assert(dummyHMPL.cptr()==particle);
  // Create DVL info outside of leak check.
  sTGCSimHitCollection dum ("coll");
  Athena_test::Leakcheck check;

  sTGCSimHitCollection trans1 ("coll");
  for (int i=0; i < 10; i++) {
    const HepMC::GenParticle* pGenParticle = genPartVector.at(i);
    HepMcParticleLink trkLink(pGenParticle->barcode(),pGenParticle->parent_event()->event_number());
    trans1.Emplace (123, 10.5,
                    Amg::Vector3D (12.5, 13.5, 14.5),
                    pGenParticle->pdg_id(),
                    Amg::Vector3D (26.5, 27.5, 28.5),
                    29.5, trkLink
                    );
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

  test1(genPartVector);
  return 0;
}
