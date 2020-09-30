/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file StoreGate/test/ThinningHandleKey_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2019
 * @brief Tests for ThinningHandleKey.
 */


#undef NDEBUG
#include "StoreGate/ThinningHandleKey.h"
#include "TestOwner.h"
#include <cassert>
#include <iostream>


class MyObj {};
CLASS_DEF (MyObj, 293847295, 1)


void test1()
{
  std::cout << "test1\n";

  {
    TestOwner owner;
    SG::ThinningHandleKey<MyObj> k1 ("aaa");
    assert (k1.clid() == 293847295);
    assert (k1.key() == "aaa");
    assert (k1.mode() == Gaudi::DataHandle::Reader);
    assert (k1.storeHandle().name() == "StoreGateSvc");
    assert (!k1.storeHandle().isSet());
    k1.setOwner (&owner);
    assert (k1.initialize("stream").isSuccess());
    assert (k1.storeHandle().isSet());
    assert (k1.decisionHandleKey().key() == "aaa_THINNED_stream.TestOwner");
    assert (owner.m_deps == std::vector<DataObjID> { k1.decisionHandleKey().fullKey() });
    assert (owner.m_modes == std::vector<Gaudi::DataHandle::Mode> { Gaudi::DataHandle::Writer });
  }

  {
    TestOwner owner;
    owner.m_name = "foo.bar";
    SG::ThinningHandleKey<MyObj> k2 (&owner, "AAB", "StoreGateSvc+aab", "doc string");
    assert (k2.clid() == 293847295);
    assert (k2.key() == "aab");
    assert (k2.mode() == Gaudi::DataHandle::Reader);
    assert (k2.storeHandle().name() == "StoreGateSvc");
    assert (!k2.storeHandle().isSet());
    assert (owner.getProperty ("AAB").name() == "AAB");
    assert (owner.getProperty ("AAB").documentation() == "doc string");
    assert (owner.getProperty ("AAB").type_info() == &typeid(SG::ReadHandleKey<MyObj>));
    assert (owner.getProperty ("AAB").toString() == "'StoreGateSvc+aab'");
    assert (owner.getProperty ("AAB").ownerTypeName() == "TestOwner");
    assert (k2.initialize("stream").isSuccess());
    assert (k2.storeHandle().isSet());
    assert (k2.decisionHandleKey().key() == "aab_THINNED_stream.foo_bar");
    assert (k2.decisionHandleKey().fullKey().key() == "StoreGateSvc+aab_THINNED_stream.foo_bar");
    assert (owner.m_deps == std::vector<DataObjID> { k2.decisionHandleKey().fullKey() });
    assert (owner.m_modes == std::vector<Gaudi::DataHandle::Mode> { Gaudi::DataHandle::Writer });
  }

  {
    TestOwner owner;
    SG::ThinningHandleKey<MyObj> k3 (&owner, "AAC", "aac", "doc string");
    assert (k3.key() == "aac");
    assert (k3.initialize("stream", "zzz").isSuccess());
    assert (k3.decisionHandleKey().key() == "aac_THINNED_stream.zzz");
    assert (owner.m_deps == std::vector<DataObjID> { k3.decisionHandleKey().fullKey() });
    assert (owner.m_modes == std::vector<Gaudi::DataHandle::Mode> { Gaudi::DataHandle::Writer });
  }

  {
    TestOwner owner;
    SG::ThinningHandleKey<MyObj> k4 (&owner, "AAD", "aad", "doc string");
    assert (k4.key() == "aad");
    assert (k4.initialize("stream", false).isSuccess());
    assert (k4.decisionHandleKey().key() == "");
    assert (owner.m_deps.empty());
    assert (owner.m_modes.empty());
  }
}


int main()
{
  std::cout << "StoreGate/ThinningHandleKey_test\n";
  test1();
}
