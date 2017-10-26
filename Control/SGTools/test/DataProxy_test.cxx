/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataProxy_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2013
 * @brief Regression tests for DataProxy.
 */

#undef NDEBUG
#include "SGTools/DataProxy.h"
#include "SGTools/StorableConversions.h"
#include "SGTools/TestStore.h"
#include "SGTools/CurrentEventStore.h"
#include "SGTools/T2pMap.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/IProxyDict.h"
#include "AthenaKernel/ILockable.h"
#include "AthenaKernel/IResetable.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include <iostream>
#include <cstdlib>
#include <cassert>


struct X1
{
  ~X1() {}
};
CLASS_DEF(X1, 8011, 1)

class XLock : public ILockable
{
public:
  XLock() : m_locked (false) {}
  void lock() { m_locked = true; std::cout << "lock\n"; }
  bool m_locked;
};
CLASS_DEF(XLock, 8114, 1)

class TestConversionSvc
  : public IConversionSvc
{
public:
  virtual StatusCode addConverter(IConverter* /*pConverter*/) 
  { std::cout << "addConverter\n"; std::abort(); }
  virtual StatusCode addConverter(const CLID& /*clid*/)
  { std::cout << "addConverter clid\n"; std::abort(); }
  virtual StatusCode removeConverter(const CLID& /*clid*/)
  { std::cout << "removeConverter\n"; std::abort(); }
  virtual IConverter* converter(const CLID& /*clid*/)
  { std::cout << "converter\n"; std::abort(); }
  virtual StatusCode connectOutput(const std::string& /*outputFile*/)
  { std::cout << "connectOutput1\n"; std::abort(); }
  virtual StatusCode connectOutput(const std::string& /*outputFile*/,
                                   const std::string& /*openMode*/)
  { std::cout << "connectOutput2\n"; std::abort(); }
  virtual StatusCode commitOutput(const std::string& /*outputFile*/,
                                  bool /*do_commit*/)
  { std::cout << "commitOutput\n"; std::abort(); }

  virtual StatusCode initialize()
  { std::cout << "initialize\n"; std::abort(); }
  virtual StatusCode finalize()
  { std::cout << "finalize\n"; std::abort(); }
  virtual const CLID& objType() const
  { std::cout << "objType\n"; std::abort(); }
  virtual long repSvcType() const
  { std::cout << "repSvcType\n"; std::abort(); }
  virtual StatusCode setDataProvider(IDataProviderSvc* /*pService*/)
  { std::cout << "setDataProvider\n"; std::abort(); }
  virtual SmartIF<IDataProviderSvc>& dataProvider() const
  { std::cout << "dataProvider\n"; std::abort(); }
  virtual StatusCode setConversionSvc(IConversionSvc* /*pService*/)
  { std::cout << "setConversionSvc\n"; std::abort(); }
  virtual SmartIF<IConversionSvc>& conversionSvc()    const
  { std::cout << "conversionSvc\n"; std::abort(); }
  virtual StatusCode setAddressCreator(IAddressCreator* /*creator*/)
  { std::cout << "setAddressCreator\n"; std::abort(); }
  virtual SmartIF<IAddressCreator>& addressCreator()    const
  { std::cout << "addressCreator\n"; std::abort(); }
  virtual StatusCode createObj(IOpaqueAddress* /*pAddress*/, DataObject*& /*refpObject*/)
  { std::cout << "createObj\n"; std::abort(); }
  virtual StatusCode fillObjRefs(IOpaqueAddress* /*pAddress*/, DataObject* /*pObject*/)
  { std::cout << "fillObjRefs\n"; std::abort(); }
  virtual StatusCode updateObj(IOpaqueAddress* /*pAddress*/, DataObject* /*refpObject*/)
  { std::cout << "updateObj\n"; std::abort(); }
  virtual StatusCode updateObjRefs(IOpaqueAddress* /*pAddress*/, DataObject* /*pObject*/)
  { std::cout << "updateObjRefs\n"; std::abort(); }
  virtual StatusCode fillRepRefs(IOpaqueAddress* /*pAddress*/, DataObject* /*pObject*/)
  { std::cout << "fillRepRefs\n"; std::abort(); }
  virtual StatusCode updateRep(IOpaqueAddress* /*pAddress*/, DataObject* /*pObject*/)
  { std::cout << "updateRep\n"; std::abort(); }
  virtual StatusCode updateRepRefs(IOpaqueAddress* /*pAddress*/, DataObject* /*pObject*/)
  { std::cout << "updateRepRefs\n"; std::abort(); }

  virtual unsigned long addRef() { return 1; }
  virtual unsigned long release() { return 1; }
  virtual StatusCode queryInterface(const InterfaceID &/*ti*/, void** /*pp*/) 
  { std::cout << "queryInterface\n"; std::abort(); }

  virtual StatusCode createRep(DataObject* /*pObject*/, IOpaqueAddress*& /*refpAddress*/)
  { std::cout << "createRep\n"; std::abort(); }
};


class TestOpaqueAddress
  : public IOpaqueAddress
{
public:
  virtual unsigned long        addRef     () { return 1; }
  virtual unsigned long        release    () { return 1; }
  virtual const CLID&          clID       () const
  { std::cout << "clID\n"; std::abort(); }
  virtual long                 svcType    () const
  { std::cout << "svcType\n"; std::abort(); }
  virtual IRegistry*           registry   () const
  { std::cout << "registry\n"; std::abort(); }
  virtual void                 setRegistry(IRegistry* /*r*/)
  { std::cout << "setRegistry\n"; std::abort(); }
  virtual const std::string*   par        () const
  { std::cout << "par\n"; std::abort(); }
  virtual const unsigned long* ipar       () const
  { std::cout << "ipar\n"; std::abort(); }
};


class TestResetable : public IResetable
{
public:
  using IResetable::reset;
  TestResetable() : set_flag(false) {}
  virtual void reset (bool hard) override { resets.push_back((int)hard); }
  virtual void finalReset() override { resets.push_back(2); }
  virtual bool isSet() const override { return set_flag; }
  virtual const std::string& key() const override { return key_string; }

  std::vector<int> resets;
  bool set_flag;
  std::string key_string;
};


void test1()
{
  std::cout << "test1\n";
  SGTest::TestStore store;

  SG::DataProxy dp;
  assert (dp.store() == 0);
  dp.setStore (&store);
  assert (dp.store() == &store);
}


// Test setConst()
void test2()
{
  std::cout << "test2\n";

  X1* x1 = new X1;
  DataObject* b1 = SG::asStorable (x1);
  SG::DataProxy dp1;
  dp1.setObject (b1);
  assert (!dp1.isConst());
  dp1.setConst();
  assert (dp1.isConst());

  XLock* xlock = new XLock;
  DataObject* b2 = SG::asStorable (xlock);
  SG::DataProxy dp2;
  dp2.setObject (b2);
  assert (!dp2.isConst());
  assert (!xlock->m_locked);
  dp2.setConst();
  assert (dp2.isConst());
  assert (xlock->m_locked);

  XLock* xlock_a = new XLock;
  DataObject* b2a = SG::asStorable (xlock_a);
  assert (!xlock_a->m_locked);
  dp2.setObject (b2a);
  assert (xlock_a->m_locked);
  assert (dp2.isConst());
}


class Test3Loader
  : public TestConversionSvc
{
public:
  Test3Loader();

  virtual StatusCode createObj(IOpaqueAddress* /*pAddress*/, DataObject*& refpObject)
  {
    m_store = SG::CurrentEventStore::store();
    refpObject = m_bucket;
    return StatusCode::SUCCESS;
  }

  IProxyDict* m_store;
  SG::DataBucket<X1>* m_bucket;
};


Test3Loader::Test3Loader()
  : m_store(nullptr),
    m_bucket (new SG::DataBucket<X1>(std::make_unique<X1>()))
{
}


// Test current event store setting in accessData.
void test3()
{
  std::cout << "test3\n";

  SGTest::TestStore store1;
  SGTest::TestStore store2;
  Test3Loader loader;
  TestOpaqueAddress address;
  auto tad = std::make_unique<SG::TransientAddress>();
  tad->setAddress (&address);
  SG::DataProxy dp1 (tad.release(), &loader);
  dp1.setStore (&store1);
  SG::T2pMap t2pmap;
  dp1.setT2p (&t2pmap);
  SG::CurrentEventStore::setStore (&store2);
  assert (SG::CurrentEventStore::store() == &store2);
  assert (dp1.accessData() == loader.m_bucket);
  assert (SG::CurrentEventStore::store() == &store2);
  assert (loader.m_store == &store1);
}


// Test bind/reset.
void test4()
{
  std::cout << "test4\n";

  SGTest::TestStore store;
  SG::DataProxy dp;
  TestResetable r;
  dp.setStore (&store);
  assert (dp.bindHandle (&r));
  assert (store.m_boundHandles == std::vector<IResetable*> {&r} );
  dp.reset (false);
  dp.reset (true);
  dp.finalReset();
  assert (r.resets == (std::vector<int> {0,1,2}) );
  r.resets.clear();
  dp.unbindHandle(&r);
  assert (store.m_boundHandles.empty());
  assert (r.resets.empty());
}


// Test move from TransientAddress.
void test5()
{
  std::cout << "test5\n";

  TestOpaqueAddress ad1;
  SG::TransientAddress tad1 (123, "key", &ad1);
  tad1.setSGKey (876);
  tad1.setTransientID (124);
  tad1.setAlias ("key2");

  Test3Loader loader;
  SG::DataProxy dp1 (std::move (tad1), &loader);
  assert (dp1.name() == "key");
  assert (dp1.clID() == 123);
  assert (dp1.address() == &ad1);
  assert (dp1.storeID() == StoreID::UNKNOWN);
  assert (dp1.sgkey() == 876);
  assert (dp1.transientID().size() == 2);
  assert (dp1.transientID (123));
  assert (dp1.transientID (124));
  assert (dp1.alias() == std::set<std::string> {"key2"});

  assert (tad1.address() == nullptr);
  assert (tad1.transientID().empty());
  assert (tad1.alias().empty());
}


int main()
{
  test1();
  test2();
  test3();
  test4();
  test5();

  // FIXME: INCOMPLETE!

  return 0;
}


