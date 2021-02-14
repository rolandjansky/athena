/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file JetTagInfoTPCnv/test/SoftElectronInfoCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2018
 * @brief Regression tests.
 */

#undef NDEBUG
#include "JetTagInfoTPCnv/SoftElectronInfoCnv_p1.h"
#include "JetTagInfoTPCnv/JetTagInfoCnv_tlp3.h"
#include "SGTools/TestStore.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <cassert>
#include <iostream>


void compare (const Analysis::SETrackInfo& p1,
              const Analysis::SETrackInfo& p2)
{
  assert (p1.electronLink() == p2.electronLink());
  assert (p1.photonLink() == p2.photonLink());
  assert (p1.d0Value() == p2.d0Value());
  assert (p1.pTrel() == p2.pTrel());
  assert (p1.tagLikelihood() == p2.tagLikelihood());
}


void compare (const Analysis::BaseTagInfo& p1,
              const Analysis::BaseTagInfo& p2)
{
  assert (p1.isValid() == p2.isValid());
  assert (p1.tagLikelihood() == p2.tagLikelihood());
  assert (p1.weight() == p2.weight());
  assert (p1.infoType() == p2.infoType());
}


void compare (const Analysis::SoftElectronInfo& p1,
              const Analysis::SoftElectronInfo& p2)
{
  compare (static_cast<const Analysis::BaseTagInfo&>(p1),
           static_cast<const Analysis::BaseTagInfo&>(p2));
  assert (p1.vectorTrackProb() == p2.vectorTrackProb());
  assert (p1.nTrackProb() == p2.nTrackProb());
  assert (p1.numTrackInfo() == p2.numTrackInfo());
  for (int i=0; i < p1.numTrackInfo(); i++) {
    compare (p1.getTrackInfo(i), p2.getTrackInfo(i));
  }
}


void testit (const Analysis::SoftElectronInfo& trans1)
{
  MsgStream log (0, "test");
  Analysis::SoftElectronInfoCnv_p1 cnv;
  Analysis::JetTagInfoCnv_tlp3 tlcnv;
  cnv.setTopConverter (&tlcnv, TPObjRef::typeID_t());
  Analysis::SoftElectronInfo_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  Analysis::SoftElectronInfo trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1 (SGTest::TestStore* store)
{
  std::cout << "test1\n";
  (void)Gaudi::Hive::currentContext();
  Athena::getMessageSvc();
  // Get proxies created outside of leak checking.

  auto ele = std::make_unique<ElectronContainer>();
  ele->push_back (new Analysis::Electron);
  ele->push_back (new Analysis::Electron);
  ele->push_back (new Analysis::Electron);
  ele->push_back (new Analysis::Electron);
  store->record (std::move(ele), "c1");

  auto pho = std::make_unique<PhotonContainer>();
  pho->push_back (new Analysis::Photon);
  pho->push_back (new Analysis::Photon);
  pho->push_back (new Analysis::Photon);
  pho->push_back (new Analysis::Photon);
  store->record (std::move(pho), "c2");

  ElementLink<ElectronContainer> el1 ("c1", 1);
  ElementLink<PhotonContainer> el2 ("c2", 3);

  Athena_test::Leakcheck check;

  Analysis::SoftElectronInfo trans1 ("typ");
  trans1.setWeight (1.5);
  trans1.setTagLikelihood ({2.5, 3.5, 4.5});
  trans1.makeValid();
  trans1.setTrackProb ({5.5, 6.5});
  trans1.setNTrackProb (7.5);
  trans1.addTrackInfo (Analysis::SETrackInfo (el1, 1.5, 2.5, {3.5, 4.5, 5.5}));
  trans1.addTrackInfo (Analysis::SETrackInfo (el2, 11.5, 12.5, {13.5, 14.5}));
  testit (trans1);
}


int main()
{
  std::cout << "JetTagInfoTPCnv/SoftElectronInfoCnv_p1_test\n";

  std::unique_ptr<SGTest::TestStore> testStore = SGTest::getTestStore();
  test1 (testStore.get());
  return 0;
}
