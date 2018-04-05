/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file InDetSimEventTPCnv/test/TrackParticleTruthCollectionCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for TrackParticleTruthCollectionCnv_p2.
 */


#undef NDEBUG
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionCnv_p2.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollection_p2.h"
#include "ParticleTruth/TrackParticleTruthCollection.h"
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
  assert ( p1.getEventCollectionAsChar() == p2.getEventCollectionAsChar() );
  assert ( p1.cptr() == p2.cptr() );
  assert ( p1 == p2 );
}

void compare (const TrackParticleTruthCollection& p1,
              const TrackParticleTruthCollection& p2)
{
  assert (p1.trackParticleContainerLink() == p2.trackParticleContainerLink());
  assert (p1.size() == p2.size());
  TrackParticleTruthCollection::const_iterator i1 = p1.begin();
  TrackParticleTruthCollection::const_iterator i2 = p2.begin();
  for (; i1 != p1.end(); ++i1, ++i2) {
    assert (i1->first.link() == i2->first.link());
    compare (i1->second.particleLink(), i2->second.particleLink());
    assert (i1->second.particleLink() == i2->second.particleLink());
    assert (i1->second.probability() == i2->second.probability());
  }
}


void testit (const TrackParticleTruthCollection& trans1)
{
  MsgStream log (0, "test");
  TrackParticleTruthCollectionCnv_p2 cnv;
  Rec::TrackParticleTruthCollection_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrackParticleTruthCollection trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1(std::vector<HepMC::GenParticle*> genPartVector)
{
  std::cout << "test1\n";

  TrackParticleTruthCollection trans1 (DataLink<Rec::TrackParticleContainer>("tpc"));
  for (int i=0; i<10; i++) {
    const HepMC::GenParticle* pGenParticle = genPartVector.at(i);
    HepMcParticleLink trkLink(pGenParticle->barcode(),pGenParticle->parent_event()->event_number());
    Rec::TrackParticleTruthKey key (ElementLink<Rec::TrackParticleContainer> ("tpc", i));
    TrackParticleTruth val (trkLink, (float)i/10);
    trans1[key] = val;
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
