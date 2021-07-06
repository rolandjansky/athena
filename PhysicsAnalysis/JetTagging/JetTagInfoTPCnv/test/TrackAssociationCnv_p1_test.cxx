/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file JetTagInfoTPCnv/test/TrackAssociationCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2018
 * @brief Regression tests.
 */

#undef NDEBUG
#include "JetTagInfoTPCnv/TrackAssociationCnv_p1.h"
#include "JetTagInfoTPCnv/JetTagInfoCnv_tlp3.h"
#include "SGTools/TestStore.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <cassert>
#include <iostream>


void compare (const JetAssociationBase& p1,
              const JetAssociationBase& p2)
{
  assert (p1.name() == p2.name());
  assert (p1.keyIndex() == p2.keyIndex());
}


void compare (const Analysis::TrackAssociation& p1,
              const Analysis::TrackAssociation& p2)
{
  compare (static_cast<const JetAssociationBase&>(p1),
           static_cast<const JetAssociationBase&>(p2));
  assert (p1.size() == p2.size());
  std::unique_ptr<std::vector<const Rec::TrackParticle*> > v1 { p1.tracks() };
  std::unique_ptr<std::vector<const Rec::TrackParticle*> > v2 { p2.tracks() };
  for (size_t i = 0; i < v1->size(); i++) {
    assert ((*v1)[i] == (*v2)[i]);
    assert (p1.getTrackWeight((*v1)[i]) == p2.getTrackWeight((*v2)[i]));
  }
}


void testit (const Analysis::TrackAssociation& trans1)
{
  MsgStream log (0, "test");
  Analysis::TrackAssociationCnv_p1 cnv;
  Analysis::JetTagInfoCnv_tlp3 tlcnv;
  cnv.setTopConverter (&tlcnv, TPObjRef::typeID_t());
  Analysis::NavAssociationCommon_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  Analysis::TrackAssociation trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1 (SGTest::TestStore* store)
{
  std::cout << "test1\n";
  (void)Gaudi::Hive::currentContext();
  Analysis::TrackAssociation trans0a ("name0");
  Analysis::TrackAssociation trans0b ("name1");
  Athena::getMessageSvc();
  // Get proxies created outside of leak checking.
  auto c_up = std::make_unique<Rec::TrackParticleContainer>();
  c_up->push_back (new Rec::TrackParticle);
  c_up->push_back (new Rec::TrackParticle);
  Rec::TrackParticleContainer* c = c_up.get();
  store->record (std::move(c_up), "c");

  Athena_test::Leakcheck check;

  Analysis::TrackAssociation trans1 ("name1");
  testit (trans1);

  Analysis::TrackAssociation trans2 ("name1");
  trans2.set_track (c, size_t(0), 2.5);
  trans2.set_track (c, size_t(1), 3.5);
  testit (trans2);
}


int main()
{
  std::cout << "JetTagInfoTPCnv/TrackAssociationCnv_p1_test\n";

  std::unique_ptr<SGTest::TestStore> testStore = SGTest::getTestStore();
  test1 (testStore.get());
  return 0;
}
