/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file JetTagInfoTPCnv/test/IPInfoPlusCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2018
 * @brief Regression tests.
 */

#undef NDEBUG
#include "JetTagInfoTPCnv/IPInfoPlusCnv_p1.h"
#include "JetTagInfoTPCnv/JetTagInfoCnv_tlp3.h"
#include "SGTools/TestStore.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <cassert>
#include <iostream>


void compare (const Analysis::BaseTagInfo& p1,
              const Analysis::BaseTagInfo& p2)
{
  assert (p1.isValid() == p2.isValid());
  assert (p1.tagLikelihood() == p2.tagLikelihood());
  assert (p1.weight() == p2.weight());
  assert (p1.infoType() == p2.infoType());
}


void compare (const Analysis::TrackGrade& p1,
              const Analysis::TrackGrade& p2)
{
  assert (p1.gradeNumber() == p2.gradeNumber());
  assert (p1.gradeString() == p2.gradeString());
}


void compare (const Analysis::IPTrackInfo& p1,
              const Analysis::IPTrackInfo& p2)
{
  assert (p1.trackLink() == p2.trackLink());
  compare (p1.trackGrade(), p2.trackGrade());
  assert (p1.isFromV0() == p2.isFromV0());
  assert (p1.d0Value() == p2.d0Value());
  assert (p1.d0Significance() == p2.d0Significance());
  assert (p1.z0Value() == p2.z0Value());
  assert (p1.z0Significance() == p2.z0Significance());
  assert (p1.trackWeight2D() == p2.trackWeight2D());
  assert (p1.trackWeight3D() == p2.trackWeight3D());
  assert (p1.trackProbJP() == p2.trackProbJP());
  assert (p1.trackProbJPneg() == p2.trackProbJPneg());
}


void compare (const Analysis::IPInfoPlus& p1,
              const Analysis::IPInfoPlus& p2)
{
  compare (static_cast<const Analysis::BaseTagInfo&>(p1),
           static_cast<const Analysis::BaseTagInfo&>(p2));
  assert (p1.numTrackInfo() == p2.numTrackInfo());
  for (int i = 0; i < p1.numTrackInfo(); i++) {
    compare (p1.getTrackInfo(i), p2.getTrackInfo(i));
  }
}


void testit (const Analysis::IPInfoPlus& trans1)
{
  MsgStream log (0, "test");
  Analysis::IPInfoPlusCnv_p1 cnv;
  Analysis::JetTagInfoCnv_tlp3 tlcnv;
  cnv.setTopConverter (&tlcnv, TPObjRef::typeID_t());
  Analysis::IPInfoPlus_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  Analysis::IPInfoPlus trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  (void)Gaudi::Hive::currentContext();
  Athena::getMessageSvc();
  // Get proxies created outside of leak checking.
  ElementLink<Rec::TrackParticleContainer> el1 ("c1", 2);
  ElementLink<Rec::TrackParticleContainer> el2 ("c1", 4);

  Athena_test::Leakcheck check;

  Analysis::IPInfoPlus trans1 ("typ");
  trans1.setWeight (1.5);
  trans1.setTagLikelihood ({2.5, 3.5, 4.5});
  trans1.makeValid();

  Analysis::IPTrackInfo ti1 (el1,
                             Analysis::TrackGrade (11, "grade1"),
                             true, 1.5, 2.5, 3.5, 4.5);
  ti1.setTrackWeight2D (5.5);
  ti1.setTrackWeight3D (5.5);
  ti1.setTrackProbJP (6.5);
  ti1.setTrackProbJPneg (7.5);
  trans1.addTrackInfo (ti1);

  Analysis::IPTrackInfo ti2 (el2,
                             Analysis::TrackGrade (12, "grade2"),
                             false, 11.5, 12.5, 13.5, 14.5);
  ti2.setTrackWeight2D (15.5);
  ti2.setTrackWeight3D (15.5);
  ti2.setTrackProbJP (16.5);
  ti2.setTrackProbJPneg (17.5);
  trans1.addTrackInfo (ti2);

  testit (trans1);
}


int main()
{
  std::cout << "JetTagInfoTPCnv/IPInfoPlusCnv_p1_test\n";

  std::unique_ptr<SGTest::TestStore> testStore = SGTest::getTestStore();
  test1();
  return 0;
}
