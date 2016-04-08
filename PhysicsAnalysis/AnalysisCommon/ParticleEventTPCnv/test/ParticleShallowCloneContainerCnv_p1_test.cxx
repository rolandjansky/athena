/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file ParticleEventTPCnv/test/ParticleShallowCloneContainerCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "ParticleEventTPCnv/ParticleShallowCloneContainerCnv_p1.h"
#include "ParticleEvent/ParticleShallowCloneContainer.h"
#include "ParticleEvent/Neutrino.h"
#include "ParticleEvent/NeutrinoContainer.h"
#include "SGTools/TestStore.h"
#include "CxxUtils/make_unique.h"
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


void compare (const ParticleShallowCloneContainer& c1,
              const ParticleShallowCloneContainer& c2)
{
  assert (c1.size() == c2.size());
  for (size_t i = 0; i < c1.size(); i++)
    compare (*c1[i], *c2[i]);
}


void test1()
{
  std::cout << "test1\n";

  IParticleContainer* c = new IParticleContainer;
  c->push_back (CxxUtils::make_unique<Neutrino>());
  SGTest::store.record (c, "cont");

  P4Momentum_t mom (100, 200, 300, 400);
  ElementLink<VxContainer> origin ("vx", 0);
  ParticleShallowCloneContainer trans1;
  auto p1 = CxxUtils::make_unique<Analysis::ParticleShallowClone>
    (MasterLink_t ("cont", 0),
     &mom,
     std::numeric_limits<ChargeType>::quiet_NaN(),
     &origin);

  auto p2 = CxxUtils::make_unique<Analysis::ParticleShallowClone> (*p1);
  p2->set_charge (1.5);

  trans1.push_back (std::move (p1));
  trans1.push_back (std::move (p2));

  MsgStream log (0, "test");
  ParticleShallowCloneContainerCnv_p1 cnv;
  ParticleShallowCloneContainer_p1 pers;
  cnv.transToPers (&trans1, &pers, log);

  ParticleShallowCloneContainer trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


int main()
{
  SGTest::initTestStore();
  test1();
  return 0;
}
