/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file ParticleEventTPCnv/test/ParticleBaseCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "ParticleEventTPCnv/ParticleBaseCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "ParticleEvent/ParticleBase.h"
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


void testit (const ParticleEvent::Base& trans1)
{
  MsgStream log (0, "test");

  ParticleBaseCnv_p1 cnv;
  ParticleBase_p1 pers;
  cnv.transToPers (&trans1, &pers, log);

  ParticleEvent::Base trans2;
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

  ParticleEvent::Base trans1;
  trans1.set_dataType (ParticleDataType::FastShower);
  trans1.set_origin (origlink);
  testit (trans1);

  ParticleEvent::Base trans2 (trans1);
  trans2.set_charge (1.5);
  testit (trans2);

  ParticleEvent::Base trans3 (trans1);
  trans3.set_pdgId (PDG::t);
  testit (trans3);
}


int main()
{
  SGTest::initTestStore();
  test1();
  return 0;
}
