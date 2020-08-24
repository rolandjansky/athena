/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file ParticleEventTPCnv/test/ParticleShallowCloneCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "ParticleEventTPCnv/ParticleShallowCloneCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "ParticleEvent/ParticleShallowClone.h"
#include "ParticleEvent/Neutrino.h"
#include "ParticleEvent/NeutrinoContainer.h"
#include "SGTools/TestStore.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const Analysis::ParticleShallowClone& p1,
              const Analysis::ParticleShallowClone& p2)
{
  assert (p1.px() == p2.px());
  assert (p1.py() == p2.py());
  assert (p1.pz() == p2.pz());
  assert (p1.e() == p2.e());

  assert (p1.getMasterLink() == p2.getMasterLink());
  assert (p1.originLink() == p2.originLink());
  assert (p1.hasCharge() == p2.hasCharge());
  if (p1.hasCharge())
    assert (p1.charge() == p2.charge());
}


void testit (const Analysis::ParticleShallowClone& trans1)
{
  MsgStream log (0, "test");

  ParticleShallowCloneCnv_p1 cnv;
  ParticleShallowClone_p1 pers;
  cnv.transToPers (&trans1, &pers, log);

  Analysis::ParticleShallowClone trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  IParticleContainer* c = new IParticleContainer;
  c->push_back (std::make_unique<Neutrino>());
  SGTest::store.record (c, "cont");
  ElementLink<VxContainer> origin ("vx", 0);

  IParticleContainer::dvlinfo();

  Athena_test::Leakcheck check;
  P4Momentum_t mom (100, 200, 300, 400);
  Analysis::ParticleShallowClone trans1 (MasterLink_t ("cont", 0),
                                         &mom,
                                         std::numeric_limits<ChargeType>::quiet_NaN(),
                                         &origin);
  testit (trans1);

  Analysis::ParticleShallowClone trans2 (trans1);
  trans2.set_charge (1.5);
  testit (trans2);
}


int main()
{
  SGTest::initTestStore();
  test1();
  return 0;
}
