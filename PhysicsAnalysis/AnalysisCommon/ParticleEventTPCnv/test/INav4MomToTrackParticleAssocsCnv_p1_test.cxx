/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file ParticleEventTPCnv/test/INav4MomToTrackParticleAssocsCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "ParticleEventTPCnv/INav4MomToTrackParticleAssocsCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "ParticleEvent/INav4MomToTrackParticleAssocs.h"
#include "ParticleEvent/Neutrino.h"
#include "SGTools/TestStore.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const INav4MomToTrackParticleAssocs& a1,
              const INav4MomToTrackParticleAssocs& a2)
{
  assert (a1.size() == a2.size());
  INav4MomToTrackParticleAssocs::object_iterator i1 = a1.beginObject();
  INav4MomToTrackParticleAssocs::object_iterator i2 = a2.beginObject();
  for (size_t i = 0; i < a1.size(); ++i, ++i1, ++i2) {
    assert (i1.getObjectLink() == i2.getObjectLink());

    assert (i1.getNumberOfAssociations() == i2.getNumberOfAssociations());
    INav4MomToTrackParticleAssocs::asso_iterator j1 = i1.second();
    INav4MomToTrackParticleAssocs::asso_iterator j2 = i2.second();
    for (size_t j = 0; j < i1.getNumberOfAssociations(); ++j, ++j1, ++j2) {
      assert (j1.getLink() == j2.getLink());
    }
  }

  assert (a1.nAssocStores() == a2.nAssocStores());
  assert (a1.assocStore ("foo").isDefault());
  assert (a2.assocStore ("foo").isDefault());
  assert (a1.assocStore ("xstore") == DataLink<INav4MomToTrackParticleAssocs> ("xstore"));
  assert (a2.assocStore ("xstore") == DataLink<INav4MomToTrackParticleAssocs> ("xstore"));
}


void test1()
{
  std::cout << "test1\n";

  INavigable4MomentumCollection* in4mc = new INavigable4MomentumCollection;
  for (int i=0; i < 10; i++)
    in4mc->push_back (std::make_unique<Neutrino>());
  SGTest::store.record (in4mc, "in");
      
  INav4MomToTrackParticleAssocs* xstore = new INav4MomToTrackParticleAssocs;
  SGTest::store.record (xstore, "xstore");

  MsgStream log (0, "test");
  // Get proxies created.
  ElementLink<INavigable4MomentumCollection> dum1 ("in", 3);
  ElementLink<Rec::TrackParticleContainer> dum2   ("tp", 2);
  DataLink<INav4MomToTrackParticleAssocs> storelink ("xstore");

  INavigable4MomentumCollection::dvlinfo();

  Athena_test::Leakcheck check;

  INav4MomToTrackParticleAssocs trans1;
  trans1.addAssociation (ElementLink<INavigable4MomentumCollection> ("in", 3),
                         ElementLink<Rec::TrackParticleContainer>   ("tp", 2));
  trans1.addAssociation (ElementLink<INavigable4MomentumCollection> ("in", 6),
                         ElementLink<Rec::TrackParticleContainer>   ("tp", 4));

  trans1.addAssocStore (storelink);

  INav4MomToTrackParticleAssocsCnv_p1 cnv;
  INav4MomToTrackParticleAssocs_p1 pers;
  cnv.transToPers (&trans1, &pers, log);

  INav4MomToTrackParticleAssocs trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


int main()
{
  SGTest::initTestStore();
  test1();
  return 0;
}
