/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file JetTagInfoTPCnv/test/SMTrackInfoCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2018
 * @brief Regression tests.
 */

#undef NDEBUG
#include "JetTagInfoTPCnv/SMTrackInfoCnv_p1.h"
#include "JetTagInfoTPCnv/JetTagInfoCnv_tlp3.h"
#include "SGTools/TestStore.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <cassert>
#include <iostream>


void compare (const Analysis::SMTrackInfo& p1,
              const Analysis::SMTrackInfo& p2)
{
  assert (p1.muonLink() == p2.muonLink());
  assert (p1.d0Value() == p2.d0Value());
  assert (p1.pTrel() == p2.pTrel());
  assert (p1.tagLikelihood() == p2.tagLikelihood());
}


void testit (const Analysis::SMTrackInfo& trans1)
{
  MsgStream log (0, "test");
  Analysis::SMTrackInfoCnv_p1 cnv;
  Analysis::JetTagInfoCnv_tlp3 tlcnv;
  cnv.setTopConverter (&tlcnv, TPObjRef::typeID_t());
  Analysis::SMTrackInfo_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  Analysis::SMTrackInfo trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  (void)Gaudi::Hive::currentContext();
  Athena::getMessageSvc();
  // Get proxies created outside of leak checking.
  ElementLink<Analysis::MuonContainer> el1 ("c1", 2);

  Athena_test::Leakcheck check;

  Analysis::SMTrackInfo trans1 (el1, 1.5, 2.5, {3.5, 4.5, 5.5});
  testit (trans1);
}


int main()
{
  std::cout << "JetTagInfoTPCnv/SMTrackInfoCnv_p1_test\n";

  std::unique_ptr<SGTest::TestStore> testStore = SGTest::getTestStore();
  test1();
  return 0;
}
