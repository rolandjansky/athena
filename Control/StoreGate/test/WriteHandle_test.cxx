/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file StoreGate/test/WriteHandle_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for WriteHandle.
 */


#undef NDEBUG
#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/exceptions.h"
#include "SGTools/TestStore.h"
#include "SGTools/CLASS_DEF.h"
#include "SGTools/StorableConversions.h"
#include "SGTools/DataProxy.h"
#include "TestTools/initGaudi.h"
#include "TestTools/expect_exception.h"
#include "AthContainersInterfaces/IConstAuxStore.h"
#include "AthenaKernel/errorcheck.h"
#include "CxxUtils/make_unique.h"
#include "CxxUtils/unused.h"
#include <cassert>
#include <iostream>


class MyObjAux
  : public SG::IConstAuxStore, public ILockable
{
public:
  MyObjAux(int x=0) : x(x) {}
  ~MyObjAux() { deleted.push_back (x); }
  int x;
  bool m_locked = false;

  virtual const void* getData (SG::auxid_t /*auxid*/) const override { return 0; }
  virtual void* getDecoration (SG::auxid_t /*auxid*/, size_t /*size*/, size_t /*capacity*/) override { return 0; }
  virtual const SG::auxid_set_t& getAuxIDs() const override { std::abort(); }
  virtual void lock() override { m_locked = true; }
  virtual void clearDecorations() override { }
  virtual size_t size() const override { return 0; }

  static std::vector<int> deleted;
};
std::vector<int> MyObjAux::deleted;
CLASS_DEF (MyObjAux, 293847296, 1)

class MyObj
{
public:
  MyObj(int x=0) : x(x) {}
  ~MyObj() { deleted.push_back (x); }
  SG::IAuxStore* getStore() const { return nullptr; }
  void setStore (SG::IConstAuxStore* store) {aux = dynamic_cast<MyObjAux*>(store); }
  int x;
  MyObjAux* aux {nullptr};

  static std::vector<int> deleted;
};
std::vector<int> MyObj::deleted;
CLASS_DEF (MyObj, 293847295, 1)
static const CLID MyCLID = 293847295;


class MyDObj : public DataObject
{
public:
  MyDObj(int x=0) : x(x) {}
  ~MyDObj() { deleted.push_back (x); }
  int x;

  static std::vector<int> deleted;
};
std::vector<int> MyDObj::deleted;
CLASS_DEF (MyDObj, 293847297, 1)


std::pair<std::unique_ptr<MyObj>, std::unique_ptr<MyObjAux> >
makeWithAux (int x=0)
{
  auto obj = CxxUtils::make_unique<MyObj>(x);
  auto aux = CxxUtils::make_unique<MyObjAux>(x+100);
  obj->setStore (aux.get());
  return std::make_pair (std::move(obj), std::move(aux));
}



class TestHandle
  : public SG::WriteHandle<MyObj>
{
public:
  using SG::WriteHandle<MyObj>::WriteHandle;
  using SG::WriteHandle<MyObj>::setState;
  using SG::WriteHandle<MyObj>::m_store;
};


// Ctors.
void test1()
{
  std::cout << "test1\n";

  SG::WriteHandle<MyObj> h1;
  assert (h1.clid() == MyCLID);
  assert (h1.key() == "");
  assert (h1.storeHandle().name() == "StoreGateSvc");
  assert (h1.mode() == Gaudi::DataHandle::Writer);

  SG::WriteHandle<MyObj> h2 ("foo", "FooSvc");
  assert (h2.clid() == MyCLID);
  assert (h2.key() == "foo");
  assert (h2.name() == "foo");
  assert (h2.storeHandle().name() == "FooSvc");
  assert (h2.mode() == Gaudi::DataHandle::Writer);

  SG::WriteHandleKey<MyObj> k3 ("asd");
  assert (k3.initialize().isSuccess());
  SG::WriteHandle<MyObj> h3 (k3);
  assert (h3.clid() == MyCLID);
  assert (h3.key() == "asd");
  assert (h3.storeHandle().name() == "StoreGateSvc");
  assert (h3.mode() == Gaudi::DataHandle::Writer);

  {
    SG::WriteHandleKey<MyObj> k4 ("asd", "BazSvc");
    k4.initialize().ignore();
    EXPECT_EXCEPTION (SG::ExcUninitKey, SG::WriteHandle<MyObj> h4 (k4));
  }

  SGTest::TestStore dumstore;
  EventContext ctx5;
  ctx5.setProxy (&dumstore);
  SG::WriteHandle<MyObj> h5 (k3, ctx5);
  assert (h5.clid() == MyCLID);
  assert (h5.key() == "asd");
  assert (h5.storeHandle().name() == "StoreGateSvc");
  assert (h5.mode() == Gaudi::DataHandle::Writer);
  assert (h5.store() == "TestStore");

  SG::WriteHandleKey<MyObj> k6 ("asd", "OtherStore");
  assert (k6.initialize().isSuccess());
  SG::WriteHandle<MyObj> h6 (k6, ctx5);
  assert (h6.clid() == MyCLID);
  assert (h6.key() == "asd");
  assert (h6.storeHandle().name() == "OtherStore");
  assert (h6.mode() == Gaudi::DataHandle::Writer);
  assert (h6.store() == "OtherStore" || h6.store() == "OtherStore_Impl");

  {
    SG::WriteHandleKey<MyObj> k7 ("asd", "BazSvc");
    k7.initialize().ignore();
    EXPECT_EXCEPTION (SG::ExcUninitKey, SG::WriteHandle<MyObj> h7 (k7, ctx5));
  }
}


// Copy
void test2()
{
  std::cout << "test2\n";
  SGTest::TestStore testStore;

  SG::WriteHandle<MyObj> h1 ("foo", "FooSvc");
  assert (h1.store() == "FooSvc");
  assert (h1.setProxyDict (&testStore).isSuccess());
  assert (h1.store() == "TestStore");

  auto fooobj = CxxUtils::make_unique<MyObj>();
  MyObj* fooptr = fooobj.get();
  assert (h1.record (std::move(fooobj)).isSuccess());
  assert (h1.isInitialized());
  assert (h1.cptr() == fooptr);

  SG::DataProxy* foo_proxy = testStore.proxy (MyCLID, "foo");
  assert (foo_proxy->refCount() == 2);

  SG::WriteHandle<MyObj> h2 (h1);
  assert (h2.key() == "foo");
  assert (h2.store() == "TestStore");
  assert (h2.isInitialized());
  assert (h2.cptr() == fooptr);
  assert (foo_proxy->refCount() == 3);

  SG::WriteHandle<MyObj> h3 (std::move(h2));
  assert (h3.key() == "foo");
  assert (h3.store() == "TestStore");
  assert (h3.isInitialized());
  assert (h3.cptr() == fooptr);
  assert (foo_proxy->refCount() == 3);
  assert (h2.key() == "foo");
  assert (h2.store() == "TestStore");
  assert (!h2.isInitialized());
  assert (h2.cptr() == nullptr);

  SG::WriteHandle<MyObj> h4 ("bar", "FooSvc");
  assert (h4.setProxyDict (&testStore).isSuccess());

  auto barobj = CxxUtils::make_unique<MyObj>();
  MyObj* barptr = barobj.get();
  assert (h4.record (std::move(barobj)).isSuccess());
  assert (h4.isInitialized());
  assert (h4.cptr() == barptr);

  SG::DataProxy* bar_proxy = testStore.proxy (MyCLID, "bar");
  assert (bar_proxy->refCount() == 2);

  h3 = h4;
  assert (h3.key() == "bar");
  assert (h3.store() == "TestStore");
  assert (h3.isInitialized());
  assert (h3.cptr() == barptr);
  assert (h4.key() == "bar");
  assert (h4.store() == "TestStore");
  assert (h4.isInitialized());
  assert (h4.cptr() == barptr);

  assert (foo_proxy->refCount() == 2);
  assert (bar_proxy->refCount() == 3);

  // h1: foo, h2: unint, h3: bar, h4: bar

  h2 = std::move(h3);
  assert (h2.key() == "bar");
  assert (h2.store() == "TestStore");
  assert (h2.isInitialized());
  assert (h2.cptr() == barptr);
  assert (h3.key() == "bar");
  assert (h3.store() == "TestStore");
  assert (!h3.isInitialized());
  assert (h3.cptr() == nullptr);

  assert (foo_proxy->refCount() == 2);
  assert (bar_proxy->refCount() == 3);
}


// Retrieve
void test3()
{
  std::cout << "test3\n";

  SGTest::TestStore testStore;
  SG::WriteHandle<MyObj> h1 ("foo", "FooSvc");
  assert (h1.setProxyDict (&testStore).isSuccess());

  assert (!h1.isValid());
  assert (h1.cachedPtr() == nullptr);
  assert (h1.ptr() == nullptr);
  assert (h1.cptr() == nullptr);
  int UNUSED(xx) = 0;
  EXPECT_EXCEPTION (SG::ExcNullWriteHandle, xx = (*h1).x);
  EXPECT_EXCEPTION (SG::ExcNullWriteHandle, xx = h1->x);

  auto obj = CxxUtils::make_unique<MyObj>(10);
  MyObj* objptr = obj.get();
  assert (h1.record(std::move(obj)).isSuccess());

  assert (h1.isValid());
  assert (h1.cachedPtr() == objptr);
  assert (h1.ptr() == objptr);
  assert (h1.cptr() == objptr);
  assert ((*h1).x == 10);
  assert (h1->x == 10);
}


// record (unique_ptr)
void test4()
{
  std::cout << "test4\n";
  SGTest::TestStore testStore;

  SG::WriteHandle<MyObj> h1 ("foo1", "FooSvc");
  assert (h1.setProxyDict (&testStore).isSuccess());

  assert (!h1.isInitialized());
  assert (!h1.isValid());

  assert (h1.recordNonConst (CxxUtils::make_unique<MyObj>(20)).isSuccess());
  assert (h1.isInitialized());
  assert (h1.isValid());
  assert (h1->x == 20);
  assert (!h1.isConst());

  SG::WriteHandle<MyObj> h4 ("foo4", "FooSvc");
  assert (h4.setProxyDict (&testStore).isSuccess());
  assert (h4.record (CxxUtils::make_unique<MyObj>(23)).isSuccess());
  assert (h4.isInitialized());
  assert (h4.isValid());
  assert (h4->x == 23);
  assert (h4.isConst());

  // Record existing object --- should fail.
  MyObj::deleted.clear();
  SG::WriteHandle<MyObj> h5 ("foo1", "FooSvc");
  assert (h5.setProxyDict (&testStore).isSuccess());
  assert (h5.record (CxxUtils::make_unique<MyObj>(25)).isFailure());
  assert (MyObj::deleted == std::vector<int>{25});
}


// record (with aux store)
void test5()
{
  std::cout << "test5\n";
  SGTest::TestStore testStore;

  SG::WriteHandle<MyObj> h1 ("foo1", "FooSvc");
  assert (h1.setProxyDict (&testStore).isSuccess());
  auto ptrs1 = makeWithAux(30);
  assert (h1.recordNonConst (std::move(ptrs1.first), std::move(ptrs1.second)).isSuccess());
  assert (h1.isInitialized());
  assert (h1.isValid());
  assert (h1->x == 30);
  assert (!h1.isConst());

  SG::ReadHandle<MyObjAux> h1a ("foo1Aux.", "FooSvc");
  assert (h1a.setProxyDict (&testStore).isSuccess());
  assert (h1a.isValid());
  assert (h1a.isInitialized());
  assert (h1a->x == 130);
  assert (!h1a.isConst());
  assert (h1->aux->x == 130);

  SG::WriteHandle<MyObj> h4 ("foo4", "FooSvc");
  assert (h4.setProxyDict (&testStore).isSuccess());
  auto ptrs4 = makeWithAux(33);
  assert (h4.record (std::move(ptrs4.first), std::move(ptrs4.second)).isSuccess());
  assert (h4.isInitialized());
  assert (h4.isValid());
  assert (h4->x == 33);
  assert (h4.isConst());

  SG::ReadHandle<MyObjAux> h4a ("foo4Aux.", "FooSvc");
  assert (h4a.setProxyDict (&testStore).isSuccess());
  assert (h4a.isValid());
  assert (h4a.isInitialized());
  assert (h4a->x == 133);
  assert (!h4a.isConst());

  MyObj::deleted.clear();
  MyObjAux::deleted.clear();
  SG::WriteHandle<MyObj> h5 ("foo4", "FooSvc");
  assert (h5.setProxyDict (&testStore).isSuccess());
  auto ptrs5 = makeWithAux(34);
  assert (h5.record (std::move(ptrs5.first), std::move(ptrs5.second)).isFailure());
  assert (!h5.isInitialized());
  assert (MyObj::deleted == std::vector<int>{34});
  assert (MyObjAux::deleted == std::vector<int>{134});

  testStore.record (new MyObjAux(200), "barAux.");
  MyObj::deleted.clear();
  MyObjAux::deleted.clear();
  SG::WriteHandle<MyObj> h6 ("bar", "FooSvc");
  assert (h6.setProxyDict (&testStore).isSuccess());
  auto ptrs6 = makeWithAux(35);
  assert (h6.record (std::move(ptrs6.first), std::move(ptrs6.second)).isFailure());
  assert (h6.isInitialized());
  assert (h6.isValid());
  assert (h6->x == 35);
  assert (h6->aux == nullptr);
  assert (MyObj::deleted == std::vector<int>{});
  assert (MyObjAux::deleted == std::vector<int>{135});

  MyObjAux* paux = nullptr;
  {
    SG::WriteHandle<MyObj> h7 ("foo7", "FooSvc");
    assert (h7.setProxyDict (&testStore).isSuccess());
    auto ptrs7 = makeWithAux(30);
    paux = ptrs7.second.get();
    assert (h7.record (std::move(ptrs7.first), std::move(ptrs7.second)).isSuccess());
    assert (!paux->m_locked);
  }
  assert (paux->m_locked);
}


// record (shared ptr)
void test6()
{
  std::cout << "test6\n";
  SGTest::TestStore testStore;

  SG::DataObjectSharedPtr<MyDObj> p1 (new MyDObj (300));
  assert (p1->refCount() == 1);

  SG::WriteHandle<MyDObj> h1 ("foo1", "FooSvc");
  assert (h1.setProxyDict (&testStore).isSuccess());
  assert (h1.recordNonConst (p1).isSuccess());
  assert (p1->refCount() == 2);
  assert (h1.isValid());
  assert (h1->x == 300);
  assert (!h1.isConst());
  assert (h1.ptr() == p1.get());

  SG::WriteHandle<MyDObj> h4 ("foo4", "FooSvc");
  assert (h4.setProxyDict (&testStore).isSuccess());
  assert (h4.record (p1).isSuccess());
  assert (p1->refCount() == 3);
  assert (h4.isValid());
  assert (h4->x == 300);
  assert (h4.isConst());
  assert (h4.ptr() == p1.get());

  SG::WriteHandle<MyDObj> h5 ("foo4", "FooSvc");
  assert (h5.setProxyDict (&testStore).isSuccess());
  assert (h5.record (p1).isFailure());
  assert (p1->refCount() == 3);
  assert (!h5.isValid());
}


#if 0
// recordOrRetrieve
void test7()
{
  std::cout << "test7\n";
  SGTest::TestStore testStore;

  SG::WriteHandle<MyObj> h1 ("foo1", "FooSvc");
  assert (h1.setProxyDict (&testStore).isSuccess());
  auto obj1 = CxxUtils::make_unique<MyObj>(400);
  MyObj* objptr = obj1.get();
  assert (h1.recordOrRetrieve (std::move(obj1)).isSuccess());
  assert (h1.isValid());
  assert (h1->x == 400);
  assert (h1.ptr() == objptr);
  assert (!h1.isConst());

  MyObj::deleted.clear();
  SG::WriteHandle<MyObj> h2 ("foo1", "FooSvc");
  assert (h2.setProxyDict (&testStore).isSuccess());
  auto obj2 = CxxUtils::make_unique<MyObj>(401);
  assert (h2.recordOrRetrieve (std::move(obj2)).isSuccess());
  assert (h2.isValid());
  assert (h2->x == 400);
  assert (h2.ptr() == objptr);
  assert (MyObj::deleted == std::vector<int>{401});
  assert (!h2.isConst());

  MyObj::deleted.clear();
  assert (h1.setConst().isSuccess());
  SG::WriteHandle<MyObj> h3 ("foo1", "FooSvc");
  assert (h3.setProxyDict (&testStore).isSuccess());
  auto obj3 = CxxUtils::make_unique<MyObj>(402);
  assert (h3.recordOrRetrieve (std::move(obj3)).isFailure());
  assert (MyObj::deleted == std::vector<int>{402});
}
#endif


// makeHandle
void test8()
{
  std::cout << "test8\n";
  SGTest::TestStore testStore;

  SG::WriteHandleKey<MyObj> k1 ("asd");
  assert (k1.initialize().isSuccess());
  auto h1 = SG::makeHandle (k1);
  assert (h1.clid() == MyCLID);
  assert (h1.key() == "asd");
  assert (h1.storeHandle().name() == "StoreGateSvc");
  assert (h1.mode() == Gaudi::DataHandle::Writer);

  SG::WriteHandleKey<MyObj> k2 ("asd", "BazSvc");
  k2.initialize().ignore();
  EXPECT_EXCEPTION (SG::ExcUninitKey, SG::makeHandle (k2));

  SGTest::TestStore dumstore;
  EventContext ctx;
  ctx.setProxy (&dumstore);
  auto h2 = SG::makeHandle (k1, ctx);
  assert (h2.clid() == MyCLID);
  assert (h2.key() == "asd");
  assert (h2.storeHandle().name() == "StoreGateSvc");
  assert (h2.mode() == Gaudi::DataHandle::Writer);
  assert (h2.store() == "TestStore");

  SG::WriteHandleKey<MyObj> k3 ("asd", "OtherStore");
  assert (k3.initialize().isSuccess());
  auto h3 = SG::makeHandle (k3, ctx);
  assert (h3.clid() == MyCLID);
  assert (h3.key() == "asd");
  assert (h3.storeHandle().name() == "OtherStore");
  assert (h3.mode() == Gaudi::DataHandle::Writer);
  assert (h3.store() == "OtherStore" || h3.store() == "OtherStore_Impl");

  SG::WriteHandleKey<MyObj> k4 ("asd", "BazSvc");
  k4.initialize().ignore();
  EXPECT_EXCEPTION (SG::ExcUninitKey, SG::makeHandle (k4, ctx));
}


int main()
{
  errorcheck::ReportMessage::hideErrorLocus();
  errorcheck::ReportMessage::hideFunctionNames();
  ISvcLocator* svcloc;
  Athena_test::initGaudi("VarHandleBase_test.txt", svcloc); //need MessageSvc

  test1();
  test2();
  test3();
  test4();
  test5();
  test6();
  //test7();
  test8();
  return 0;
}
