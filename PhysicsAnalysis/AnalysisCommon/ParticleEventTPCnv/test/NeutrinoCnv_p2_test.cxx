/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file ParticleEventTPCnv/test/NeutrinoCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "ParticleEventTPCnv/NeutrinoCnv_p2.h"
#include "TestTools/leakcheck.h"
#include "ParticleEvent/Neutrino.h"
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


void compare (const Neutrino& p1,
              const Neutrino& p2)
{
  compare (p1.particleBase(), p2.particleBase());
  compare (p1.momentumBase(), p2.momentumBase());
}


void testit (const Neutrino& trans1)
{
  MsgStream log (0, "test");

  NeutrinoCnv_p2 cnv;
  Neutrino_p2 pers;
  cnv.transToPers (&trans1, &pers, log);

  Neutrino trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  ElementLink<VxContainer> origlink ("orig", 10);
  AthenaBarCodeImpl dum; // Get services created.
  dum.getVersion();
  Athena_test::Leakcheck check;

  Neutrino trans1;
  trans1.set4Mom (CLHEP::HepLorentzVector(100,200,300,400));
  trans1.set_dataType (ParticleDataType::FastShower);
  trans1.set_origin (origlink);
  testit (trans1);

  Neutrino trans2 (trans1);
  trans2.set_charge (1.5);
  testit (trans2);

  Neutrino trans3 (trans1);
  trans3.set_pdgId (PDG::t);
  testit (trans3);
}


int main()
{
  SGTest::initTestStore();
  test1();
  return 0;
}
