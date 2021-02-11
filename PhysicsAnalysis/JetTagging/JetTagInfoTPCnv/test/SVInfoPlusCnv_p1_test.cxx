/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file JetTagInfoTPCnv/test/SVInfoPlusCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2018
 * @brief Regression tests.
 */

#undef NDEBUG
#include "JetTagInfoTPCnv/SVInfoPlusCnv_p1.h"
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


void compare (const Trk::Vertex& p1,
              const Trk::Vertex& p2)
{
  assert (p1.position() == p2.position());
}


void compare (const Trk::FitQuality& p1,
              const Trk::FitQuality& p2)
{
  assert (p1.chiSquared() == p2.chiSquared());
  assert (p1.numberDoF()  == p2.numberDoF());
}


void compare (const Trk::RecVertex& p1,
              const Trk::RecVertex& p2)
{
  compare (static_cast<const Trk::Vertex&>(p1),
           static_cast<const Trk::Vertex&>(p2));
  assert (p1.covariancePosition() == p2.covariancePosition());
  compare (p1.fitQuality(), p2.fitQuality());
}


void compare (const Analysis::SVTrackInfo& p1,
              const Analysis::SVTrackInfo& p2)
{
  assert (p1.trackLink() == p2.trackLink());
}


void compare (const Analysis::SVInfoPlus& p1,
              const Analysis::SVInfoPlus& p2)
{
  compare (p1.getRecSvx(), p2.getRecSvx());
  assert (p1.getNGTrackInJet() == p2.getNGTrackInJet());
  assert (p1.getNGTrackInSvx() == p2.getNGTrackInSvx());
  assert (p1.getN2T() == p2.getN2T());
  assert (p1.getMass() == p2.getMass());
  assert (p1.getEnergyFraction() == p2.getEnergyFraction());
  assert (p1.getNormDist() == p2.getNormDist());
  assert (p1.numTrackInfo() == p2.numTrackInfo());
  for (int i=0; i < p1.numTrackInfo(); i++) {
    compare (p1.getTrackInfo(i), p2.getTrackInfo(i));
  }
}


void testit (const Analysis::SVInfoPlus& trans1)
{
  MsgStream log (0, "test");
  Analysis::SVInfoPlusCnv_p1 cnv;
  Analysis::JetTagInfoCnv_tlp3 tlcnv;
  cnv.setTopConverter (&tlcnv, TPObjRef::typeID_t());
  Analysis::SVInfoPlus_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  Analysis::SVInfoPlus trans2;
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

  Analysis::SVInfoPlus trans1 ("typ");
  trans1.setWeight (1.5);
  trans1.setTagLikelihood ({2.5, 3.5, 4.5});
  trans1.makeValid();
  trans1.setNGTrackInJet (10);
  trans1.setNGTrackInSvx (11);
  trans1.setN2T (12);
  trans1.setMass (1.5);
  trans1.setEnergyFraction (5.5);
  trans1.setNormDist (13.5);

  Amg::Vector3D pos1 (10.5, 11.5, 12.5);
  AmgSymMatrix(3) cov1;
  for (int i=0; i < 3; i++) {
    for (int j=0; j<3; j++) {
      cov1(i,j) = 100*(i+1)*(j+1);
    }
  }
  Trk::RecVertex rv1 (pos1, cov1, 21.5, 22.5);
  trans1.setRecSvx (rv1);

  trans1.addTrackInfo (Analysis::SVTrackInfo (el1));
  trans1.addTrackInfo (Analysis::SVTrackInfo (el2));

  testit (trans1);
}


int main()
{
  std::cout << "JetTagInfoTPCnv/SVInfoPlusCnv_p1_test\n";

  std::unique_ptr<SGTest::TestStore> testStore = SGTest::getTestStore();
  test1();
  return 0;
}
