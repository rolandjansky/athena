/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file ParticleEventTPCnv/test/CompositeParticleCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "ParticleEventTPCnv/CompositeParticleCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "ParticleEvent/CompositeParticle.h"
#include "SGTools/TestStore.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const ParticleEvent::Base& p1,
              const ParticleEvent::Base& p2)
{
  assert (p1.dataType() == p2.dataType());
  assert (p1.originLink() == p2.originLink());
  assert (p1.hasCharge() == p2.hasCharge());
  if (p1.hasCharge())
    assert (p1.charge() == p2.charge());
  assert (p1.hasPdgId() == p2.hasPdgId());
  if (p1.hasPdgId())
    assert (p1.pdgId() == p2.pdgId());
}


void compare (const P4ImplPxPyPzE& p1,
              const P4ImplPxPyPzE& p2)
{
  assert (p1.px() == p2.px());
  assert (p1.py() == p2.py());
  assert (p1.pz() == p2.pz());
  assert (p1.e() == p2.e());
}


void compare (const CompositeParticleNavigation& p1,
              const CompositeParticleNavigation& p2)
{
  assert (p1.size() == p2.size());
  CompositeParticleNavigation::object_iter i1 = p1.begin();
  CompositeParticleNavigation::object_iter i2 = p2.begin();
  for (size_t i = 0; i < p1.size(); ++i, ++i1, ++i2) {
    assert (*i1 == *i2);
    assert (p1.getParameter(i1) == p2.getParameter(i2));
  }
}


void compare (const CompositeParticle& p1,
              const CompositeParticle& p2)
{
  compare (p1.particleBase(), p2.particleBase());
  compare (p1.momentumBase(), p2.momentumBase());
  compare (p1.navigableBase(), p2.navigableBase());
}


void testit (const CompositeParticle& trans1)
{
  MsgStream log (0, "test");

  CompositeParticleCnv_p1 cnv;
  CompositeParticle_p1 pers;
  cnv.transToPers (&trans1, &pers, log);

  CompositeParticle trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  AthenaBarCodeImpl dumbc; // Get services created.
  dumbc.getVersion();
  ElementLink<VxContainer> origlink ("orig", 10);
  INav4MomLink dum ("part", 19);
  Athena_test::Leakcheck check;

  CompositeParticle trans1;
  trans1.set4Mom (CLHEP::HepLorentzVector(100,200,300,400));
  trans1.set_dataType (ParticleDataType::FastShower);
  trans1.set_origin (origlink);
  trans1.navigableBase().insertElement (INav4MomLink ("part", 12));
  trans1.navigableBase().insertElement (INav4MomLink ("part", 19));
  testit (trans1);

  CompositeParticle trans2 (trans1);
  trans2.set_charge (1.5);
  testit (trans2);

  CompositeParticle trans3 (trans1);
  trans3.set_pdgId (PDG::t);
  testit (trans3);
}


int main()
{
  SGTest::initTestStore();
  test1();
  return 0;
}
