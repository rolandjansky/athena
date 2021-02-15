/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file JetTagInfoTPCnv/test/ElectronAssociationCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2018
 * @brief Regression tests.
 */

#undef NDEBUG
#include "JetTagInfoTPCnv/ElectronAssociationCnv_p1.h"
#include "JetTagInfoTPCnv/JetTagInfoCnv_tlp3.h"
#include "SGTools/TestStore.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <cassert>
#include <iostream>


void compare (const JetAssociationBase& p1,
              const JetAssociationBase& p2)
{
  assert (p1.name() == p2.name());
  assert (p1.keyIndex() == p2.keyIndex());
}


void compare (const Analysis::ElectronAssociation& p1,
              const Analysis::ElectronAssociation& p2)
{
  compare (static_cast<const JetAssociationBase&>(p1),
           static_cast<const JetAssociationBase&>(p2));
  assert (p1.electron() == p2.electron());
}


void testit (const Analysis::ElectronAssociation& trans1)
{
  MsgStream log (0, "test");
  Analysis::ElectronAssociationCnv_p1 cnv;
  Analysis::JetTagInfoCnv_tlp3 tlcnv;
  cnv.setTopConverter (&tlcnv, TPObjRef::typeID_t());
  Analysis::NavAssociationCommon_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  Analysis::ElectronAssociation trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1 (SGTest::TestStore* store)
{
  std::cout << "test1\n";
  (void)Gaudi::Hive::currentContext();
  Analysis::ElectronAssociation trans0a ("name0");
  Analysis::ElectronAssociation trans0b ("name1");
  Athena::getMessageSvc();
  // Get proxies created outside of leak checking.
  auto c_up = std::make_unique<ElectronContainer>();
  c_up->push_back (new Analysis::Electron);
  ElectronContainer* c = c_up.get();
  store->record (std::move(c_up), "c");

  Athena_test::Leakcheck check;

  Analysis::ElectronAssociation trans1 ("name1");
  testit (trans1);

  Analysis::ElectronAssociation trans2 ("name1");
  trans2.set_electron (c, size_t(0), 2.5);
  testit (trans2);
}


int main()
{
  std::cout << "JetTagInfoTPCnv/ElectronAssociationCnv_p1_test\n";

  std::unique_ptr<SGTest::TestStore> testStore = SGTest::getTestStore();
  test1 (testStore.get());
  return 0;
}
