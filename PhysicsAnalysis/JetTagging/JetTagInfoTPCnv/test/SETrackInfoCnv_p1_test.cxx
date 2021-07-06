/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file JetTagInfoTPCnv/test/SETrackInfoCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2018
 * @brief Regression tests.
 */

#undef NDEBUG
#include "JetTagInfoTPCnv/SETrackInfoCnv_p1.h"
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


void testit (const Analysis::SETrackInfo& trans1)
{
  MsgStream log (0, "test");
  Analysis::SETrackInfoCnv_p1 cnv;
  Analysis::JetTagInfoCnv_tlp3 tlcnv;
  cnv.setTopConverter (&tlcnv, TPObjRef::typeID_t());
  Analysis::SETrackInfo_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  Analysis::SETrackInfo trans2;
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

  Analysis::SETrackInfo trans1 (el1, 1.5, 2.5, {3.5, 4.5, 5.5});
  testit (trans1);

  Analysis::SETrackInfo trans2 (el2, 11.5, 12.5, {13.5, 14.5});
  testit (trans2);
}


int main()
{
  std::cout << "JetTagInfoTPCnv/SETrackInfoCnv_p1_test\n";

  std::unique_ptr<SGTest::TestStore> testStore = SGTest::getTestStore();
  test1 (testStore.get());
  return 0;
}
