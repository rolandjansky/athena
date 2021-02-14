/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file ParticleEventTPCnv/test/ParticleAssocsCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "ParticleEventTPCnv/TrackParticleAssocsCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "ParticleEvent/TrackParticleAssocs.h"
#include "SGTools/TestStore.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <cassert>
#include <iostream>


void compare (const TrackParticleAssocs& a1,
              const TrackParticleAssocs& a2)
{
  assert (a1.size() == a2.size());
  TrackParticleAssocs::object_iterator i1 = a1.beginObject();
  TrackParticleAssocs::object_iterator i2 = a2.beginObject();
  for (size_t i = 0; i < a1.size(); ++i, ++i1, ++i2) {
    assert (i1.getObjectLink() == i2.getObjectLink());

    assert (i1.getNumberOfAssociations() == i2.getNumberOfAssociations());
    TrackParticleAssocs::asso_iterator j1 = i1.second();
    TrackParticleAssocs::asso_iterator j2 = i2.second();
    for (size_t j = 0; j < i1.getNumberOfAssociations(); ++j, ++j1, ++j2) {
      assert (j1.getLink() == j2.getLink());
    }
  }

  assert (a1.nAssocStores() == a2.nAssocStores());
  assert (a1.assocStore ("foo").isDefault());
  assert (a2.assocStore ("foo").isDefault());
  assert (a1.assocStore ("xstore") == DataLink<TrackParticleAssocs> ("xstore"));
  assert (a2.assocStore ("xstore") == DataLink<TrackParticleAssocs> ("xstore"));
}


void test1()
{
  std::cout << "test1\n";
  (void)Gaudi::Hive::currentContext();

  Rec::TrackParticleContainer* tpc = new Rec::TrackParticleContainer;
  for (int i=0; i < 10; i++)
    tpc->push_back (std::make_unique<Rec::TrackParticle>());
  SGTest::store.record (tpc, "tpc");
      
  TrackParticleAssocs* xstore = new TrackParticleAssocs;
  SGTest::store.record (xstore, "xstore");

  MsgStream log (0, "test");

  Athena_test::Leakcheck check;
  TrackParticleAssocs trans1;
  trans1.addAssociation (ElementLink<Rec::TrackParticleContainer> ("tpc", 3),
                         ElementLink<Rec::TrackParticleContainer> ("tpc", 2));
  trans1.addAssociation (ElementLink<Rec::TrackParticleContainer> ("tpc", 6),
                         ElementLink<Rec::TrackParticleContainer> ("tpc", 4));

  trans1.addAssocStore (DataLink<TrackParticleAssocs> ("xstore"));

  TrackParticleAssocsCnv_p1 cnv;
  TrackParticleAssocs_p1 pers;
  cnv.transToPers (&trans1, &pers, log);

  TrackParticleAssocs trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


int main()
{
  SGTest::initTestStore();
  test1();
  return 0;
}
