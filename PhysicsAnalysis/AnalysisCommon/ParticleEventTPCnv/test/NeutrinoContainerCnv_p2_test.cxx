/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file ParticleEventTPCnv/test/NeutrinoContainerCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "ParticleEventTPCnv/NeutrinoContainerCnv_p2.h"
#include "TestTools/leakcheck.h"
#include "ParticleEvent/NeutrinoContainer.h"
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


void compare (const NeutrinoContainer& c1,
              const NeutrinoContainer& c2)
{
  assert (c1.size() == c2.size());
  for (size_t i = 0; i < c1.size(); i++)
    compare (*c1[i], *c2[i]);
}


void test1()
{
  std::cout << "test1\n";
  MsgStream log (0, "test");
  AthenaBarCodeImpl dum; // Get services created.
  dum.getVersion();
  ElementLink<VxContainer> origlink ("orig", 10);
  Athena_test::Leakcheck check;

  NeutrinoContainer trans1;
  auto p1 = std::make_unique<Neutrino>();
  p1->set4Mom (CLHEP::HepLorentzVector(100,200,300,400));
  p1->set_dataType (ParticleDataType::FastShower);
  p1->set_origin (origlink);

  auto p2 = std::make_unique<Neutrino>(*p1);
  p2->set_charge (1.5);

  auto p3 = std::make_unique<Neutrino>(*p1);
  p3->set_pdgId (PDG::t);

  trans1.push_back (std::move(p1));
  trans1.push_back (std::move(p2));
  trans1.push_back (std::move(p3));

  NeutrinoContainerCnv_p2 cnv;
  NeutrinoContainer_p2 pers;
  cnv.transToPers (&trans1, &pers, log);

  NeutrinoContainer trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


int main()
{
  SGTest::initTestStore();
  test1();
  return 0;
}
