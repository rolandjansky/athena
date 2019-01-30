/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG
#include "MuonSimEventTPCnv/MMSimHitCollectionCnv_p2.h"
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

void compare (const MMSimHit& p1,
              const MMSimHit& p2)
{
  assert (p1.MMId() == p2.MMId());
  assert (p1.globalTime() == p2.globalTime());
  assert (p1.globalPosition() == p2.globalPosition());
  assert (p1.particleEncoding() == p2.particleEncoding());
  assert (p1.kineticEnergy() == p2.kineticEnergy());
  assert (p1.globalDirection() == p2.globalDirection());
  assert (p1.depositEnergy() == p2.depositEnergy());
  compare(p1.particleLink(), p2.particleLink());
  assert (p1.particleLink() == p2.particleLink());
}


void compare (const MMSimHitCollection& p1,
              const MMSimHitCollection& p2)
{
  //assert (p1.Name() == p2.Name());
  assert (p1.size() == p2.size());
  for (size_t i=0; i < p1.size(); i++)
    compare (p1[i], p2[i]);
}


void testit (const MMSimHitCollection& trans1)
{
  MsgStream log (0, "test");
  MMSimHitCollectionCnv_p2 cnv;
  Muon::MMSimHitCollection_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  MMSimHitCollection trans2;
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
  MMSimHitCollection dum ("coll");

  MMSimHitCollection trans1 ("coll");
  for (int i=0; i < 10; i++) {
    const HepMC::GenParticle* pGenParticle = genPartVector.at(i);
    HepMcParticleLink trkLink(pGenParticle->barcode(),pGenParticle->parent_event()->event_number());
    trans1.Emplace (123, 10.5,
                    Amg::Vector3D (12.5, 13.5, 14.5),
                    pGenParticle->pdg_id(), 25.5,
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
