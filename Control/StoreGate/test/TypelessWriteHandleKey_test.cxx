/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file StoreGate/test/TypelessWriteHandleKey_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2019
 * @brief Tests for TypelessWriteHandleKey.
 */


#undef NDEBUG
#include "StoreGate/TypelessWriteHandleKey.h"
#include "StoreGate/exceptions.h"
#include "TestTools/initGaudi.h"
#include "TestTools/expect_exception.h"
#include "TestOwner.h"
#include <cassert>
#include <iostream>


void test1()
{
  std::cout << "test1\n";

  SG::TypelessWriteHandleKey k1;
  assert (k1.clid() == CLID_NULL);
  assert (k1.key() == "");
  assert (k1.mode() == Gaudi::DataHandle::Writer);
  assert (k1.storeHandle().name() == "StoreGateSvc");

  SG::TypelessWriteHandleKey k2 (123, "aaa", "FooSvc");
  assert (k2.clid() == 123);
  assert (k2.key() == "aaa");
  assert (k1.mode() == Gaudi::DataHandle::Writer);
  assert (k2.storeHandle().name() == "FooSvc");

  k2 = "FeeSvc+bbb";
  assert (k2.clid() == 123);
  assert (k2.key() == "bbb");
  assert (k1.mode() == Gaudi::DataHandle::Writer);
  assert (k2.storeHandle().name() == "FeeSvc");

  k2 = "BarSvc+EventInfo/ccc";
  assert (k2.clid() == 2101);
  assert (k2.key() == "ccc");
  assert (k1.mode() == Gaudi::DataHandle::Writer);
  assert (k2.storeHandle().name() == "BarSvc");

  EXPECT_EXCEPTION (SG::ExcBadHandleKey,
                    k2 = "BarSvc+EventInfo/ddd/");


  TestOwner owner;
  SG::TypelessWriteHandleKey k3 (&owner, "CCCKey", 123, "ccc", "doc string");
  assert (k3.clid() == 123);
  assert (k3.key() == "ccc");
  assert (k3.mode() == Gaudi::DataHandle::Writer);
  assert (owner.getProperty ("CCCKey").name() == "CCCKey");
  assert (owner.getProperty ("CCCKey").documentation() == "doc string");
  assert (owner.getProperty ("CCCKey").type_info() == &typeid(SG::TypelessWriteHandleKey));
  assert (owner.getProperty ("CCCKey").toString() == "'StoreGateSvc+ccc'");
  assert (owner.getProperty ("CCCKey").ownerTypeName() == "TestOwner");
}


int main()
{
  std::cout << "StoreGateSvc/TypelessWriteHandleKey_test\n";

  ISvcLocator* pDum;
  Athena_test::initGaudi(pDum); //need MessageSvc

  test1();
  return 0;
}

  
