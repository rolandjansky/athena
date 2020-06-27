/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file StoreGate/test/WriteDecorHandleKey_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2017
 * @brief Tests for WriteDecorHandleKey.
 */


#undef NDEBUG
#include "StoreGate/WriteDecorHandleKey.h"
#include "TestTools/initGaudi.h"
#include "TestOwner.h"
#include <cassert>
#include <iostream>


class MyObj {};
CLASS_DEF (MyObj, 293847295, 1)


class TestHolder
  : public IDataHandleHolder
{
public:
  virtual std::vector<Gaudi::DataHandle*> inputHandles() const override;
  virtual std::vector<Gaudi::DataHandle*> outputHandles() const override;
  virtual void addDependency(const DataObjID&, const Gaudi::DataHandle::Mode&) override;

  virtual unsigned long addRef() override { std::abort(); }
  virtual unsigned long release() override { std::abort(); }
  virtual StatusCode queryInterface(const InterfaceID &, void**) override { std::abort(); }
  virtual const std::string& name() const override { std::abort(); }
  

  virtual const DataObjIDColl& extraInputDeps() const override { std::abort(); }
  virtual const DataObjIDColl& extraOutputDeps() const override { std::abort(); }

  virtual void acceptDHVisitor(IDataHandleVisitor*) const override { std::abort(); }

  virtual const DataObjIDColl& inputDataObjs() const override { std::abort(); }
  virtual const DataObjIDColl& outputDataObjs() const override { std::abort(); }

  virtual void declare(Gaudi::DataHandle&) override { std::abort(); }
  virtual void renounce(Gaudi::DataHandle&) override { std::abort(); }

  std::vector<Gaudi::DataHandle*> m_inputHandles;
  std::vector<Gaudi::DataHandle*> m_outputHandles;
  std::vector<DataObjID> m_deps;
};


std::vector<Gaudi::DataHandle*> TestHolder::inputHandles() const
{
  return m_inputHandles;
}


std::vector<Gaudi::DataHandle*> TestHolder::outputHandles() const
{
  return m_outputHandles;
}


void TestHolder::addDependency(const DataObjID& id, const Gaudi::DataHandle::Mode&)
{
  m_deps.push_back (id);
}



void test1()
{
  std::cout << "test1\n";

  SG::WriteDecorHandleKey<MyObj> k1 ("aaa.dec");
  assert (k1.clid() == 293847295);
  assert (k1.key() == "aaa.dec");
  assert (k1.mode() == Gaudi::DataHandle::Writer);
  assert (k1.storeHandle().name() == "StoreGateSvc");
  assert (!k1.storeHandle().isSet());
  assert (k1.initialize().isSuccess());
  assert (k1.storeHandle().isSet());

  assert (k1.contHandleKey().clid() == 293847295);
  assert (k1.contHandleKey().key() == "aaa");
  assert (k1.contHandleKey().mode() == Gaudi::DataHandle::Reader);
  assert (k1.contHandleKey().storeHandle().name() == "StoreGateSvc");
  assert (k1.contHandleKey().storeHandle().isSet());

  k1 = "bbb.foo";
  assert (k1.key() == "bbb.foo");
  assert (k1.contHandleKey().key() == "bbb");

  assert (k1.assign ("ccc.fee").isSuccess());
  assert (k1.key() == "ccc.fee");
  assert (k1.contHandleKey().key() == "ccc");

  TestOwner owner;
  SG::WriteDecorHandleKey<MyObj> k3 (&owner, "CCCKey", "ccc.dec", "doc string");
  assert (k3.clid() == 293847295);
  assert (k3.key() == "ccc.dec");
  assert (k3.mode() == Gaudi::DataHandle::Writer);
  assert (k3.contHandleKey().clid() == 293847295);
  assert (k3.contHandleKey().key() == "ccc");
  assert (k3.contHandleKey().mode() == Gaudi::DataHandle::Reader);
  assert (owner.getProperty ("CCCKey").name() == "CCCKey");
  assert (owner.getProperty ("CCCKey").documentation() == "doc string");
  assert (owner.getProperty ("CCCKey").type_info() == &typeid(SG::VarHandleKey));
  assert (owner.getProperty ("CCCKey").toString() == "'StoreGateSvc+ccc.dec'");
  assert (owner.getProperty ("CCCKey").ownerTypeName() == "TestOwner");
}


void test2()
{
  std::cout << "test2\n";

  TestHolder h;
  SG::WriteDecorHandleKey<MyObj> k1 ("aaa.dec");
  k1.setOwner (&h);

  SG::ReadHandleKey<MyObj> r1 ("rrr");
  SG::WriteHandleKey<MyObj> w1 ("sss");
  h.m_inputHandles.push_back (&r1);
  h.m_outputHandles.push_back (&w1);
  assert (k1.initialize().isSuccess());
  assert (h.m_deps.size() == 1);
  assert (h.m_deps[0].fullKey() == "MyObj/StoreGateSvc+aaa");

  h.m_deps.clear();
  SG::ReadHandleKey<MyObj> r2 ("aaa");
  h.m_inputHandles.push_back (&r2);
  assert (k1.initialize().isSuccess());
  assert (h.m_deps.empty());
  h.m_inputHandles.pop_back();

  h.m_deps.clear();
  SG::WriteHandleKey<MyObj> w2 ("aaa");
  h.m_outputHandles.push_back (&w2);
  assert (k1.initialize().isSuccess());
  assert (h.m_deps.empty());
  h.m_outputHandles.pop_back();
}


int main()
{
  ISvcLocator* pDum;
  Athena_test::initGaudi(pDum); //need MessageSvc

  test1();
  test2();
  return 0;
}
