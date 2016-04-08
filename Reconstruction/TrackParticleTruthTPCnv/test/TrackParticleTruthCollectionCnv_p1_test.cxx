/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file InDetSimEventTPCnv/test/TrackParticleTruthCollectionCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for TrackParticleTruthCollectionCnv_p1.
 */


#undef NDEBUG
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionCnv_p1.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollection_p1.h"
#include "ParticleTruth/TrackParticleTruthCollection.h"
#include "SGTools/TestStore.h"
#include <cassert>
#include <iostream>


void compare (const TrackParticleTruthCollection& p1,
              const TrackParticleTruthCollection& p2)
{
  assert (p1.trackParticleContainerLink() == p2.trackParticleContainerLink());
  assert (p1.size() == p2.size());
  TrackParticleTruthCollection::const_iterator i1 = p1.begin();
  TrackParticleTruthCollection::const_iterator i2 = p2.begin();
  for (; i1 != p1.end(); ++i1, ++i2) {
    assert (i1->first.link() == i2->first.link());
    assert (i1->second.particleLink() == i2->second.particleLink());
    assert (i1->second.probability() == i2->second.probability());
  }
}


void testit (const TrackParticleTruthCollection& trans1)
{
  MsgStream log (0, "test");
  TrackParticleTruthCollectionCnv_p1 cnv;
  Rec::TrackParticleTruthCollection_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrackParticleTruthCollection trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  TrackParticleTruthCollection trans1 (DataLink<Rec::TrackParticleContainer>("tpc"));
  for (int i=0; i<10; i++) {
    Rec::TrackParticleTruthKey key (ElementLink<Rec::TrackParticleContainer> ("tpc", i));
    TrackParticleTruth val (i+100, (float)i/10);
    trans1[key] = val;
  }
    
  testit (trans1);
}


int main()
{
  SGTest::initTestStore();
  test1();
  return 0;
}
