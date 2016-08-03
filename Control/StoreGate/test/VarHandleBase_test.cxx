/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file StoreGate/test/VarHandleBase_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for VarHandleBase.
 */


#undef NDEBUG
#include "StoreGate/VarHandleBase.h"
#include "StoreGate/exceptions.h"
#include "SGTools/TestStore.h"
#include "SGTools/StorableConversions.h"
#include "SGTools/DataProxy.h"
#include "SGTools/TestStore.h"
#include "TestTools/initGaudi.h"
#include "TestTools/expect_exception.h"
#include "AthenaKernel/errorcheck.h"
#include "CxxUtils/make_unique.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <cassert>
#include <iostream>


class MyObj {};
CLASS_DEF (MyObj, 293847295, 1)
class MyObj2 {};
CLASS_DEF (MyObj2, 293847296, 1)


class TestHandle
  : public SG::VarHandleBase
{
public:
  using SG::VarHandleBase::VarHandleBase;

  virtual bool isValid() override { return true; }

  using SG::VarHandleBase::setState;
  using SG::VarHandleBase::typeless_dataPointer;
  using SG::VarHandleBase::typeless_dataPointer_impl;
  using SG::VarHandleBase::typeless_cptr;
  using SG::VarHandleBase::typeless_ptr;
  using SG::VarHandleBase::record_impl;
  using SG::VarHandleBase::m_store;
  using SG::VarHandleBase::m_proxy;
  using SG::VarHandleBase::m_ptr;
};


// Ctors.
void test1()
{
  std::cout << "test1\n";

  TestHandle h1 (1234, Gaudi::DataHandle::Reader);
  assert (h1.clid() == 1234);
  assert (h1.key() == "");
  assert (h1.storeHandle().name() == "StoreGateSvc");
  assert (h1.mode() == Gaudi::DataHandle::Reader);
  assert (h1.m_store == nullptr);

  TestHandle h2 (1234, "foo", Gaudi::DataHandle::Writer, "FooSvc");
  assert (h2.clid() == 1234);
  assert (h2.key() == "foo");
  assert (h2.name() == "foo");
  assert (h2.storeHandle().name() == "FooSvc");
  assert (h2.mode() == Gaudi::DataHandle::Writer);

  SGTest::TestStore dumstore2;
  EventContext ctx3;
  ctx3.setProxy (&dumstore2);
  Gaudi::Hive::setCurrentContext (ctx3);

  SG::VarHandleKey k3 (1234, "asd", Gaudi::DataHandle::Updater);
  k3.initialize().ignore();
  TestHandle h3 (k3);
  assert (h3.clid() == 1234);
  assert (h3.key() == "asd");
  assert (h3.storeHandle().name() == "StoreGateSvc");
  assert (h3.mode() == Gaudi::DataHandle::Updater);
  assert (h3.m_store == &dumstore2);

  {
    SG::VarHandleKey k4 (1234, "asd", Gaudi::DataHandle::Updater, "BazSvc");
    k4.initialize().ignore();
    EXPECT_EXCEPTION (SG::ExcUninitKey, TestHandle h4 (k4));
  }

  SGTest::TestStore dumstore;
  EventContext ctx5;
  ctx5.setProxy (&dumstore);
  TestHandle h5 (k3, ctx5);
  assert (h5.clid() == 1234);
  assert (h5.key() == "asd");
  assert (h5.storeHandle().name() == "StoreGateSvc");
  assert (h5.mode() == Gaudi::DataHandle::Updater);
  assert (h5.m_store == &dumstore);

  SG::VarHandleKey k6 (1234, "asd", Gaudi::DataHandle::Updater,
                       "OtherStore");
  k6.initialize().ignore();
  TestHandle h6 (k6, ctx5);
  assert (h6.clid() == 1234);
  assert (h6.key() == "asd");
  assert (h6.storeHandle().name() == "OtherStore");
  assert (h6.mode() == Gaudi::DataHandle::Updater);
  assert (h6.m_store != &dumstore);

  {
    SG::VarHandleKey k7 (1234, "asd", Gaudi::DataHandle::Updater, "BazSvc");
    k7.initialize().ignore();
    EXPECT_EXCEPTION (SG::ExcUninitKey, TestHandle h7 (k7, ctx5));
  }
}


// Setting, copy, reset
void test2()
{
  std::cout << "test2\n";

  auto obj = CxxUtils::make_unique<MyObj>();
  MyObj* objptr = obj.get();
  auto taddr = CxxUtils::make_unique<SG::TransientAddress> (293847295, "foo");
  SG::DataProxy* proxy = new SG::DataProxy (SG::asStorable (std::move(obj)),
                                            taddr.release());
  SGTest::TestStore testStore;
  proxy->setStore (&testStore);

  TestHandle h1 (293847295, "foo", Gaudi::DataHandle::Writer, "FooSvc");
  assert (h1.store() == "FooSvc");
  assert (h1.setProxyDict (&testStore).isSuccess());
  assert (h1.store() == "TestStore");

  h1.setState (proxy);

  assert (proxy->refCount() == 1);
  assert (objptr == h1.typeless_dataPointer());
  assert (testStore.m_boundHandles == std::vector<IResetable*>{&h1});

  {
    TestHandle h2 (h1);
    assert (h2.clid() == 293847295);
    assert (h2.key() == "foo");
    assert (h2.storeHandle().name() == "FooSvc");
    assert (h2.mode() == Gaudi::DataHandle::Writer);
    assert (h2.m_store == &testStore);
    assert (h2.m_ptr == objptr);
    assert (h2.m_proxy == proxy);
    assert (proxy->refCount() == 2);
    assert ((testStore.m_boundHandles == std::vector<IResetable*>{&h1, &h2}));

    TestHandle h3 (std::move(h2));
    assert (h3.clid() == 293847295);
    assert (h3.key() == "foo");
    assert (h3.storeHandle().name() == "FooSvc");
    assert (h3.mode() == Gaudi::DataHandle::Writer);
    assert (h3.m_store == &testStore);
    assert (h3.m_ptr == objptr);
    assert (h3.m_proxy == proxy);
    assert (proxy->refCount() == 2);
    assert ((testStore.m_boundHandles == std::vector<IResetable*>{&h1, &h3}));

    assert (h2.m_store == &testStore);
    assert (h2.m_ptr == nullptr);
    assert (h2.m_proxy == nullptr);

    h2 = h3;
    assert (h2.clid() == 293847295);
    assert (h2.key() == "foo");
    assert (h2.storeHandle().name() == "FooSvc");
    assert (h2.mode() == Gaudi::DataHandle::Writer);
    assert (h2.m_store == &testStore);
    assert (h2.m_ptr == objptr);
    assert (h2.m_proxy == proxy);
    assert (proxy->refCount() == 3);
    assert ((testStore.m_boundHandles == std::vector<IResetable*>{&h1, &h3, &h2}));

    h1.reset();
    assert (h1.clid() == 293847295);
    assert (h1.key() == "foo");
    assert (h1.storeHandle().name() == "FooSvc");
    assert (h1.mode() == Gaudi::DataHandle::Writer);
    assert (h1.m_store == &testStore);
    assert (h1.m_ptr == nullptr);
    assert (h1.m_proxy == proxy);
    assert (proxy->refCount() == 3);
    assert ((testStore.m_boundHandles == std::vector<IResetable*>{&h1, &h3, &h2}));

    h1.reset(true);
    assert (h1.clid() == 293847295);
    assert (h1.key() == "foo");
    assert (h1.storeHandle().name() == "FooSvc");
    assert (h1.mode() == Gaudi::DataHandle::Writer);
    assert (h1.m_store == nullptr);
    assert (h1.m_ptr == nullptr);
    assert (h1.m_proxy == nullptr);
    assert (proxy->refCount() == 2);
    assert ((testStore.m_boundHandles == std::vector<IResetable*>{&h3, &h2}));

    h1 = std::move(h3);
    assert (h1.clid() == 293847295);
    assert (h1.key() == "foo");
    assert (h1.storeHandle().name() == "FooSvc");
    assert (h1.mode() == Gaudi::DataHandle::Writer);
    assert (h1.m_store == &testStore);
    assert (h1.m_ptr == objptr);
    assert (h1.m_proxy == proxy);
    assert (proxy->refCount() == 2);
    assert ((testStore.m_boundHandles == std::vector<IResetable*>{&h2, &h1}));

    assert (h3.m_store == &testStore);
    assert (h3.m_ptr == nullptr);
    assert (h3.m_proxy == nullptr);
  }
  // h2, h3 went away
  assert (proxy->refCount() == 1);
  assert ((testStore.m_boundHandles == std::vector<IResetable*>{&h1}));

  // Test a !resetOnly proxy.
  auto obj5 = CxxUtils::make_unique<MyObj>();
  //MyObj* objptr5 = obj5.get();
  auto taddr5 = CxxUtils::make_unique<SG::TransientAddress> (293847295, "foo");
  SG::DataProxy* proxy5 = new SG::DataProxy (SG::asStorable (std::move(obj5)),
                                             taddr5.release(),
                                             false, false);
  proxy5->setStore (&testStore);
  proxy->addRef();
  h1.setState (proxy5);
  assert (proxy->refCount() == 1);
  assert (proxy5->refCount() == 1);
  assert ((testStore.m_boundHandles == std::vector<IResetable*>{&h1}));

  proxy5->addRef();
  h1.reset();
  assert (h1.m_proxy == nullptr);
  assert (proxy5->refCount() == 1);
  assert ((testStore.m_boundHandles == std::vector<IResetable*>{}));

  proxy->release();
  proxy5->release();
}


// isPresent, isInitialized, isSet, isConst
void test3()
{
  std::cout << "test3\n";

  SGTest::TestStore store;

  TestHandle h1 (293847295, "foo", Gaudi::DataHandle::Writer, "FooSvc");
  assert (!h1.isPresent());

  assert (h1.setProxyDict (&store).isSuccess());
  assert (!h1.isPresent());

  store.record (new MyObj, "foo");
  assert (h1.isPresent());

  store.proxy(293847295, "foo")->reset();
  assert (!h1.isPresent());

  assert (!h1.isInitialized());
  assert (h1.setState (store.proxy(293847295, "foo")).isFailure());
  assert (!h1.isInitialized());
  assert (!h1.isSet());
  assert (!h1.isConst());

  store.record (new MyObj, "bar");
  assert (h1.setState (store.proxy(293847295, "bar")).isSuccess());
  assert (h1.isInitialized());
  assert (h1.isSet());
  assert (!h1.isConst());

  store.proxy(293847295, "bar")->setConst();
  assert (h1.isConst());
}


// initialize(), setStore()
void test4()
{
  std::cout << "test4\n";

  TestHandle h1 (293847295, "foo", Gaudi::DataHandle::Writer, "FooSvc");
  assert (h1.initialize().isFailure());

  SGTest::TestStore store;
  assert (h1.setProxyDict (&store).isSuccess());
  assert (h1.initialize().isSuccess()); // ok because it's a writer.

  TestHandle h2 (293847295, "foo", Gaudi::DataHandle::Reader, "FooSvc");
  assert (h2.setProxyDict (&store).isSuccess());
  assert (h2.initialize().isFailure());
  store.record (new MyObj, "foo");
  assert (h2.setState().isSuccess());

  assert (h2.isInitialized());
  assert (h2.setProxyDict (&store).isSuccess());
  assert (!h2.isInitialized());

  assert (h2.setState().isSuccess());
  assert (h2.isInitialized());
  h2.reset();
  assert (h2.isInitialized());
  h2.finalReset();
  assert (!h2.isInitialized());
  assert (h2.m_store == 0);
}


// setConst
void test5()
{
  std::cout << "test5\n";
  SGTest::TestStore testStore;

  TestHandle h1 (293847295, "foo", Gaudi::DataHandle::Writer, "FooSvc");
  auto obj = CxxUtils::make_unique<MyObj>();
  //MyObj* objptr = obj.get();
  auto taddr = CxxUtils::make_unique<SG::TransientAddress> (293847295, "foo");
  SG::DataProxy* proxy = new SG::DataProxy (SG::asStorable (std::move(obj)),
                                            taddr.release());
  assert (h1.setConst().isFailure());
  proxy->setStore (&testStore);

  assert (h1.setState (proxy).isSuccess());
  assert (h1.isInitialized());
  assert (!h1.isConst());
  assert (!proxy->isConst());

  assert (h1.setConst().isSuccess());
  assert (h1.isConst());
  assert (proxy->isConst());
}


// setState
void test6()
{
  std::cout << "test6\n";
  SGTest::TestStore testStore;

  TestHandle h1 (293847295, "foo", Gaudi::DataHandle::Writer, "FooSvc");

  SG::DataProxy* proxy1 = nullptr;
  assert (h1.setState(proxy1).isFailure());

  auto taddr1 = CxxUtils::make_unique<SG::TransientAddress> (293847295, "foo");
  proxy1 = new SG::DataProxy (taddr1.release(), nullptr);
  proxy1->addRef();
  proxy1->setStore (&testStore);
  assert (proxy1->refCount() == 1);
  assert (h1.setState(proxy1).isFailure());
  assert (proxy1->refCount() == 1);
  assert ((testStore.m_boundHandles == std::vector<IResetable*>{}));

  auto obj1 = CxxUtils::make_unique<MyObj>();
  proxy1->setObject (SG::asStorable (std::move(obj1)));
  assert (h1.setState(proxy1).isSuccess());
  assert (proxy1->refCount() == 2);
  assert ((testStore.m_boundHandles == std::vector<IResetable*>{&h1}));
  assert (h1.m_proxy == proxy1);

  assert (h1.setState(proxy1).isSuccess());
  assert (proxy1->refCount() == 2);
  assert ((testStore.m_boundHandles == std::vector<IResetable*>{&h1}));
  assert (h1.m_proxy == proxy1);

  auto taddr2 = CxxUtils::make_unique<SG::TransientAddress> (293847295, "foo");
  SG::DataProxy* proxy2 = new SG::DataProxy (taddr2.release(), nullptr);
  proxy2->addRef();
  proxy2->setStore (&testStore);
  assert (proxy2->refCount() == 1);
  auto obj2 = CxxUtils::make_unique<MyObj>();
  proxy2->setObject (SG::asStorable (std::move(obj2)));

  assert (h1.setState(proxy2).isSuccess());
  assert (proxy1->refCount() == 1);
  assert (proxy2->refCount() == 2);
  assert ((testStore.m_boundHandles == std::vector<IResetable*>{&h1}));
  assert (h1.m_proxy == proxy2);

  proxy1->release();
  proxy2->release();

  assert (h1.setState (nullptr, "bar").isFailure());
  assert (h1.setState (&testStore, "bar").isFailure());
  testStore.record (new MyObj, "bar");
  assert (h1.setState (&testStore, "bar").isSuccess());
  assert (h1.isInitialized());
  assert (h1.m_proxy->transientAddress()->name() == "bar");
}


// record_impl
void test7()
{
  std::cout << "test7\n";

  SGTest::TestStore testStore;

  std::unique_ptr<MyObj> obj;
  MyObj* objptr = nullptr;

  TestHandle h1 (293847295, "foo", Gaudi::DataHandle::Writer, "FooSvc");
  obj = CxxUtils::make_unique<MyObj>();
  objptr = obj.get();
  assert (h1.record_impl (std::unique_ptr<DataObject>(SG::asStorable(std::move(obj))),
                          objptr,
                          true, false).isFailure());

  assert (h1.setProxyDict (&testStore).isSuccess());
  obj = CxxUtils::make_unique<MyObj>();
  objptr = obj.get();
  assert (h1.record_impl (std::unique_ptr<DataObject>(SG::asStorable(std::move(obj))),
                          objptr,
                          true, false).isSuccess());
  MyObj* fooptr = objptr;
  assert (h1.m_ptr == fooptr);

  TestHandle h2 (293847295, "foo", Gaudi::DataHandle::Writer, "FooSvc");
  assert (h2.setProxyDict (&testStore).isSuccess());
  obj = CxxUtils::make_unique<MyObj>();
  objptr = obj.get();
  assert (h2.record_impl (std::unique_ptr<DataObject>(SG::asStorable(std::move(obj))),
                          objptr,
                          true, false).isFailure());
  assert (!h2.isInitialized());
  assert (h2.m_ptr == nullptr);


  obj = CxxUtils::make_unique<MyObj>();
  objptr = obj.get();
  assert (h2.record_impl (std::unique_ptr<DataObject>(SG::asStorable(std::move(obj))),
                          objptr,
                          true, true).isSuccess());
  assert (h2.isInitialized());
  assert (h2.m_ptr == fooptr);
  assert (!h2.isConst());

  obj = CxxUtils::make_unique<MyObj>();
  objptr = obj.get();
  assert (h2.record_impl (std::unique_ptr<DataObject>(SG::asStorable(std::move(obj))),
                          objptr,
                          false, true).isSuccess());
  assert (h2.isInitialized());
  assert (h2.m_ptr == fooptr);
  assert (h2.isConst());

  TestHandle h3 (293847295, "", Gaudi::DataHandle::Writer, "FooSvc");
  assert (h3.setProxyDict (&testStore).isSuccess());
  obj = CxxUtils::make_unique<MyObj>();
  objptr = obj.get();
  assert (h3.record_impl (std::unique_ptr<DataObject>(SG::asStorable(std::move(obj))),
                          objptr,
                          true, false).isFailure());
}


// typeless_dataPointer_impl, etc
void test8()
{
  std::cout << "test8\n";
  SGTest::TestStore testStore;

  TestHandle h1 (293847295, "foo", Gaudi::DataHandle::Writer, "FooSvc");
  assert (h1.setProxyDict (&testStore).isSuccess());

  auto obj = CxxUtils::make_unique<MyObj>();
  assert (h1.m_ptr == nullptr);
  h1.m_ptr = obj.get();
  assert (h1.typeless_dataPointer_impl(false) == obj.get());
  h1.m_ptr = nullptr;

  assert (h1.typeless_dataPointer_impl(false) == nullptr);
  assert (h1.typeless_dataPointer_impl(true) == nullptr);

  testStore.record (obj.release(), "foo");
  assert (h1.initialize().isSuccess());
  testStore.proxy(293847295, "foo")->reset();

  assert (h1.typeless_dataPointer_impl(false) == nullptr);
  assert (h1.typeless_dataPointer_impl(true) == nullptr);

  TestHandle h2 (293847295, "bar", Gaudi::DataHandle::Writer, "FooSvc");
  assert (h2.setProxyDict (&testStore).isSuccess());
  obj = CxxUtils::make_unique<MyObj>();
  MyObj* objptr = obj.get();
  testStore.record (obj.release(), "bar");

  assert (h2.typeless_dataPointer_impl(false) == objptr);

  auto obj2 = CxxUtils::make_unique<MyObj2>();
  MyObj2* obj2ptr = obj2.get();
  testStore.record (obj2.release(), "fee");

  SG::sgkey_t sgkey = testStore.stringToKey ("fee", 293847295);
  testStore.m_kmap[sgkey] = testStore.proxy (293847296, "fee");

  TestHandle h3 (293847295, "fee", Gaudi::DataHandle::Writer, "FooSvc");
  assert (h3.setProxyDict (&testStore).isSuccess());
  assert (h3.typeless_dataPointer_impl(false) == nullptr);
  assert (h3.typeless_dataPointer_impl(true) == nullptr);

  testStore.proxy (293847296, "fee")->transientAddress()->setTransientID(293847295);
  assert (h3.typeless_dataPointer_impl(false) == obj2ptr);
  assert (h3.typeless_dataPointer(false) == obj2ptr);
  assert (h3.typeless_cptr() == obj2ptr);

  assert (h3.typeless_ptr(false) == obj2ptr);
  testStore.proxy (293847296, "fee")->setConst();
  EXPECT_EXCEPTION (SG::ExcConstObject, h3.typeless_ptr(false));
}


// free functions
void test9()
{
  std::cout << "test9\n";

  TestHandle h1 (293847295, "foo", Gaudi::DataHandle::Writer, "FooSvc");
  std::cout << h1 << "\n";

  assert (h1 == h1);
  TestHandle h2 (293847295, "foe", Gaudi::DataHandle::Writer, "FooSvc");
  assert (h1 != h2);
}


int main()
{
  errorcheck::ReportMessage::hideErrorLocus();
  ISvcLocator* svcloc;
  Athena_test::initGaudi("VarHandleBase_test.txt", svcloc); //need MessageSvc

  test1();
  test2();
  test3();
  test4();
  test5();
  test6();
  test7();
  test8();
  test9();
  return 0;
}

