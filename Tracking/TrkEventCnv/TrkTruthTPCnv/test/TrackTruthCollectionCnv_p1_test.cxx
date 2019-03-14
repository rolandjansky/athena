/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrkTruthTPCnv/test/TrackTruthCollectionCnv_p1_test.cxx
 * @date Feb, 2018
 * @brief Tests for TrackTruthCollectionCnv_p1.
 */


#undef NDEBUG
#include "TrkTruthTPCnv/TrackTruthCollectionCnv_p1.h"
#include "TrkTruthTPCnv/TrackTruthCollection_p1.h"
#include "TrkTruthData/TrackTruthCollection.h"
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


void compare (const TrackTruthCollection& p1,
              const TrackTruthCollection& p2)
{
  assert (p1.trackCollectionLink() == p2.trackCollectionLink());
  assert (p1.size() == p2.size());
  TrackTruthCollection::const_iterator i1 = p1.begin();
  TrackTruthCollection::const_iterator i2 = p2.begin();
  for (; i1 != p1.end(); ++i1, ++i2) {
    assert (i1->first.index() == i2->first.index());
    compare (i1->second.particleLink(), i2->second.particleLink());
    assert (i1->second.particleLink() == i2->second.particleLink());
    assert (i1->second.probability() == i2->second.probability());
  }
}


void testit (const TrackTruthCollection& trans1)
{
  MsgStream log (0, "test");
  TrackTruthCollectionCnv_p1 cnv;
  Trk::TrackTruthCollection_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrackTruthCollection trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1(std::vector<HepMC::GenParticle*> genPartVector)
{
  std::cout << "test1\n";

  TrackTruthCollection trans1 (DataLink<TrackCollection>("tpc"));
  for (int i=0; i<10; i++) {
    const HepMC::GenParticle* pGenParticle = genPartVector.at(i);
    HepMcParticleLink trkLink(pGenParticle->barcode(),pGenParticle->parent_event()->event_number());
    Trk::TrackTruthKey key (ElementLink<TrackCollection> ("tpc", i));
    TrackTruth val (trkLink, (float)i/10);
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

  //SGTest::initTestStore();
  test1(genPartVector);
  return 0;
}
