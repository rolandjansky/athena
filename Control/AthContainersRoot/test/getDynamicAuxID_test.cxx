/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainersRoot/test/getDynamicAuxID_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2016
 * @brief Regression test for getDynamicAuxID.
 */


#undef NDEBUG
#include "AthContainersRoot/getDynamicAuxID.h"
#include "AthContainersRoot/AthContainersRootTestDict.h"
#include "AthContainersRoot/RootAuxVectorFactory.h"
#include "AthContainers/AuxElement.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TClass.h"
#include <iostream>
#include <cassert>


void test1 (SG::auxid_t a1_id)
{
  std::cout << "test1\n";
  SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();

  assert (SG::getDynamicAuxID (typeid(int),
                               "a1",
                               "int",
                               "std::vector<int>",
                               false) == a1_id);

  const SG::IAuxTypeVectorFactory* fac;
  fac = r.getFactory (typeid(AthContainersRootTest::Foo));
  assert  (fac == nullptr);
  SG::auxid_t foo_id = SG::getDynamicAuxID (typeid(AthContainersRootTest::Foo),
                                            "foo",
                                            "AthContainersRootTest::Foo",
                                            "std::vector<AthContainersRootTest::Foo>",
                                            false);
  assert (foo_id != SG::null_auxid);
  fac = r.getFactory (typeid(AthContainersRootTest::Foo));
  assert  (fac != nullptr);
  assert (dynamic_cast<const SG::AuxTypeVectorFactory<AthContainersRootTest::Foo>*>(fac) != nullptr);
  assert (r.getName(foo_id) == "foo");
  assert (r.getTypeName(foo_id) == "AthContainersRootTest::Foo");
  assert (r.getVecTypeName(foo_id) == "std::vector<AthContainersRootTest::Foo>");

  fac = r.getFactory (typeid(AthContainersRootTest::Bar));
  assert  (fac == nullptr);
  SG::auxid_t bar_id = SG::getDynamicAuxID (typeid(AthContainersRootTest::Bar),
                                            "bar",
                                            "AthContainersRootTest::Bar",
                                            "std::vector<AthContainersRootTest::Bar>",
                                            false);
  assert (bar_id != SG::null_auxid);
  fac = r.getFactory (typeid(AthContainersRootTest::Bar));
  assert  (fac != nullptr);
  assert (dynamic_cast<const SG::RootAuxVectorFactory*>(fac) != nullptr);
  assert (r.getName(bar_id) == "bar");
  assert (r.getTypeName(bar_id) == "AthContainersRootTest::Bar");
  assert (r.getVecTypeName(bar_id) == "std::vector<AthContainersRootTest::Bar>");

  fac = r.getFactory (typeid(AthContainersRootTest::Baz));
  assert  (fac == nullptr);
  SG::auxid_t baz_id = SG::getDynamicAuxID (typeid(AthContainersRootTest::Baz),
                                            "baz",
                                            "AthContainersRootTest::Baz",
                                            "AthContainersRootTest::Baz",
                                            true);
  assert (baz_id != SG::null_auxid);
  fac = r.getFactory (typeid(AthContainersRootTest::Baz));
  assert  (fac != nullptr);
  assert (dynamic_cast<const SG::RootAuxVectorFactory*>(fac) != nullptr);
  assert (r.getName(baz_id) == "baz");
  assert (r.getTypeName(baz_id) == "AthContainersRootTest::Baz");
  assert (r.getVecTypeName(baz_id) == "std::vector<AthContainersRootTest::Baz>");
}


SG::auxid_t init()
{
  static SG::AuxElement::Accessor<int> a1 ("a1");
  return a1.auxid();
}


int main()
{
  SG::auxid_t a1_auxid = init();
  test1(a1_auxid);
  return 0;
}
