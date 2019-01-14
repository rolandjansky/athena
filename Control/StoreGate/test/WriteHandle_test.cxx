/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/StorableConversions.h"
#include "SGTools/DataProxy.h"
#include "TestTools/initGaudi.h"
#include "TestTools/expect_exception.h"
#include "AthContainersInterfaces/IConstAuxStore.h"
#include "AthContainers/DataVector.h"
#include "AthContainers/ConstDataVector.h"
#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "CxxUtils/unused.h"
#include "CxxUtils/checker_macros.h"
#include <cassert>
#include <iostream>

#define DEBUG_VHB 1


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
  virtual bool isDecoration(SG::auxid_t /*auxid*/) const override { std::abort(); }
  virtual void lock() override { m_locked = true; }
  virtual bool clearDecorations() override { return false; }
  virtual size_t size() const override { return 0; }
  virtual void lockDecoration (SG::auxid_t) override { std::abort(); }

  static std::vector<int> deleted ATLAS_THREAD_SAFE;
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

  static std::vector<int> deleted ATLAS_THREAD_SAFE;
};
std::vector<int> MyObj::deleted;
CLASS_DEF (MyObj, 293847295, 1)
static const CLID MyCLID = 293847295;

CLASS_DEF (DataVector<MyObj>, 293847495, 1)


class MyObj2 {};
CLASS_DEF (MyObj2, 293847395, 1)


class MyDObj : public DataObject
{
public:
  MyDObj(int x=0) : x(x) {}
  ~MyDObj() { deleted.push_back (x); }
  int x;

  static std::vector<int> deleted ATLAS_THREAD_SAFE;
};
std::vector<int> MyDObj::deleted;
CLASS_DEF (MyDObj, 293847297, 1)


std::pair<std::unique_ptr<MyObj>, std::unique_ptr<MyObjAux> >
makeWithAux (int x=0)
{
  auto obj = std::make_unique<MyObj>(x);
  auto aux = std::make_unique<MyObjAux>(x+100);
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
  ctx5.setExtension( Atlas::ExtendedEventContext(&dumstore) );
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

  auto fooobj = std::make_unique<MyObj>();
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
  assert (h2.store() == "TestStore");
  assert (!h2.isInitialized());
  assert (h2.cptr() == nullptr);

  SG::WriteHandle<MyObj> h4 ("bar", "FooSvc");
  assert (h4.setProxyDict (&testStore).isSuccess());

  auto barobj = std::make_unique<MyObj>();
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

  auto obj = std::make_unique<MyObj>(10);
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

  assert (h1.recordNonConst (std::make_unique<MyObj>(20)).isSuccess());
  assert (h1.isInitialized());
  assert (h1.isValid());
  assert (h1->x == 20);
  assert (!h1.isConst());

  SG::WriteHandle<MyObj> h4 ("foo4", "FooSvc");
  assert (h4.setProxyDict (&testStore).isSuccess());
  assert (h4.record (std::make_unique<MyObj>(23)).isSuccess());
  assert (h4.isInitialized());
  assert (h4.isValid());
  assert (h4->x == 23);
  assert (h4.isConst());

  // Record existing object --- should fail.
  MyObj::deleted.clear();
  SG::WriteHandle<MyObj> h5 ("foo1", "FooSvc");
  assert (h5.setProxyDict (&testStore).isSuccess());
  assert (h5.record (std::make_unique<MyObj>(25)).isFailure());
  assert (MyObj::deleted == std::vector<int>{25});
}


// record (with aux store)
SG::WriteHandle<MyObj> test5a (IProxyDict& testStore, MyObjAux*& paux)
{
  SG::WriteHandle<MyObj> h7a ("foo7a", "FooSvc");
  assert (h7a.setProxyDict (&testStore).isSuccess());
  auto ptrs7a = makeWithAux(31);
  paux = ptrs7a.second.get();
  assert (h7a.record (std::move(ptrs7a.first), std::move(ptrs7a.second)).isSuccess());
  assert (!paux->m_locked);
  return h7a;
}
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

  paux = nullptr;
  {
    SG::WriteHandle<MyObj> h7b;
    {
      SG::WriteHandle<MyObj> h7a = test5a (testStore, paux);
      assert (!paux->m_locked);
      h7b = std::move(h7a);
    }
    assert (!paux->m_locked);
  }
  assert (paux->m_locked);

  // Test record() setting store pointer.
  SG::WriteHandle<MyObj> h8 ("foo8", "FooSvc");
  assert (h8.setProxyDict (&testStore).isSuccess());
  auto ptrs8 = makeWithAux(30);
  ptrs8.first->setStore (nullptr);
  assert (h8.record (std::move(ptrs8.first), std::move(ptrs8.second)).isSuccess());
  assert (h8.isInitialized());
  assert (h8.isValid());
  assert (h8->x == 30);
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

  SG::DataObjectSharedPtr<MyDObj> p4 (new MyDObj (400));
  assert (p4->refCount() == 1);

  SG::WriteHandle<MyDObj> h4 ("foo4", "FooSvc");
  assert (h4.setProxyDict (&testStore).isSuccess());
  assert (h4.record (p4).isSuccess());
  assert (p4->refCount() == 2);
  assert (h4.isValid());
  assert (h4->x == 400);
  assert (h4.isConst());
  assert (h4.ptr() == p4.get());

  SG::WriteHandle<MyDObj> h5 ("foo5", "FooSvc");
  assert (h5.setProxyDict (&testStore).isSuccess());
  assert (h5.record (p1).isFailure());
  assert (p1->refCount() == 2);
  assert (!h5.isValid());
}


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
  ctx.setExtension( Atlas::ExtendedEventContext(&dumstore) );
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


// put (unique_ptr)
void test9()
{
  std::cout << "test9\n";
  SGTest::TestStore testStore;

  SG::WriteHandle<MyObj> h4 ("foo4");
  assert (h4.setProxyDict (&testStore).isSuccess());
  const MyObj* o = h4.put (std::make_unique<MyObj>(23));
  assert (o->x == 23);

  // Record existing object --- should fail.
  MyObj::deleted.clear();
  SG::WriteHandle<MyObj> h5 ("foo4", "FooSvc");
  assert (h5.setProxyDict (&testStore).isSuccess());
  assert (h5.put (std::make_unique<MyObj>(25)) == nullptr);
  assert (MyObj::deleted == std::vector<int>{25});

  // Record existing object, requesting retrieval of existing.
  MyObj::deleted.clear();
  o = h5.put (std::make_unique<MyObj>(25), true);
  assert (MyObj::deleted == std::vector<int>{25});
  assert (o->x == 23);

  // Record to a different context.
  MyObj::deleted.clear();
  SGTest::TestStore testStore2;
  EventContext ctx2;
  ctx2.setExtension( Atlas::ExtendedEventContext(&testStore2) );
  o = h4.put (ctx2, std::make_unique<MyObj>(26));
  assert (o->x == 26);
  assert (MyObj::deleted.empty());

  SG::WriteHandle<MyObj> h6 ("foo6");
  assert (h6.setProxyDict (&testStore).isSuccess());
  o = h6.put (std::make_unique<const MyObj>(33));
  assert (o->x == 33);
  o = h6.put (ctx2, std::make_unique<const MyObj>(34));
  assert (o->x == 34);
  assert (MyObj::deleted.empty());

  SG::WriteHandle<DataVector<MyObj> > h7 ("foo7");
  assert (h7.setProxyDict (&testStore).isSuccess());
  const DataVector<MyObj>* vo =
    h7.put (std::make_unique<const DataVector<MyObj> >());
  assert (vo->empty());

  SG::WriteHandle<DataVector<MyObj> > h8 ("foo8");
  assert (h8.setProxyDict (&testStore).isSuccess());
  const DataVector<MyObj>* vco =
    h8.put (std::make_unique<ConstDataVector<DataVector<MyObj> > >());
  assert (vco->empty());
}


// put (with aux store)
void test10()
{
  std::cout << "test10\n";
  SGTest::TestStore testStore;

  SG::WriteHandle<MyObj> h1 ("foo1", "FooSvc");
  assert (h1.setProxyDict (&testStore).isSuccess());
  auto ptrs1 = makeWithAux(30);
  const MyObj* o = h1.put (std::move(ptrs1.first), std::move(ptrs1.second));
  assert (o->x == 30);

  SG::ReadHandle<MyObjAux> h1a ("foo1Aux.", "FooSvc");
  assert (h1a.setProxyDict (&testStore).isSuccess());
  assert (h1a.isValid());
  assert (h1a.isInitialized());
  assert (h1a->x == 130);
  assert (h1a.isConst());
  assert (o->aux->x == 130);

  MyObj::deleted.clear();
  MyObjAux::deleted.clear();
  SG::WriteHandle<MyObj> h5 ("foo1");
  assert (h5.setProxyDict (&testStore).isSuccess());
  auto ptrs5 = makeWithAux(34);
  assert (h5.put (std::move(ptrs5.first), std::move(ptrs5.second)) == nullptr);
  assert (MyObj::deleted == std::vector<int>{34});
  assert (MyObjAux::deleted == std::vector<int>{134});

  testStore.record (new MyObjAux(200), "barAux.");
  MyObj::deleted.clear();
  MyObjAux::deleted.clear();
  SG::WriteHandle<MyObj> h6 ("bar", "FooSvc");
  assert (h6.setProxyDict (&testStore).isSuccess());
  auto ptrs6 = makeWithAux(35);
  assert (h6.put (std::move(ptrs6.first), std::move(ptrs6.second)) == nullptr);
  SG::ReadHandle<MyObj> h6a ("bar", "FooSvc");
  assert (h6a.setProxyDict (&testStore).isSuccess());
  assert (h6a->x == 35);
  assert (h6a->aux == nullptr);
  assert (MyObj::deleted == std::vector<int>{});
  assert (MyObjAux::deleted == std::vector<int>{135});

  // Test put() setting store pointer.
  SG::WriteHandle<MyObj> h8 ("foo8", "FooSvc");
  assert (h8.setProxyDict (&testStore).isSuccess());
  auto ptrs8 = makeWithAux(30);
  MyObjAux* auxptr = ptrs8.second.get();
  ptrs8.first->setStore (nullptr);
  o = h8.put (std::move(ptrs8.first), std::move(ptrs8.second));
  assert (o != 0);
  assert (o->x == 30);
  assert (o->aux == auxptr);

  // Record to a different context.
  MyObj::deleted.clear();
  MyObjAux::deleted.clear();
  SGTest::TestStore testStore2;
  EventContext ctx2;
  ctx2.setExtension( Atlas::ExtendedEventContext(&testStore2) );

  auto ptrs9 = makeWithAux(40);
  o = h5.put (ctx2, std::move(ptrs9.first), std::move(ptrs9.second));
  assert (o->x == 40);
  assert (MyObj::deleted.empty());
  assert (MyObjAux::deleted.empty());

  SG::WriteHandle<MyObj> h10 ("foo10");
  assert (h10.setProxyDict (&testStore).isSuccess());
  auto ptrs10 = makeWithAux(40);
  o = h10.put (std::unique_ptr<const MyObj>(std::move(ptrs10.first)),
               std::unique_ptr<const MyObjAux>(std::move(ptrs10.second)));
  assert (o->x == 40);
  auto ptrs11 = makeWithAux(50);
  o = h10.put (ctx2,
               std::unique_ptr<const MyObj>(std::move(ptrs11.first)),
               std::unique_ptr<const MyObjAux>(std::move(ptrs11.second)));
  assert (o->x == 50);
  assert (MyObj::deleted.empty());
  assert (MyObjAux::deleted.empty());
}


// put (with shared pointer)
void test11()
{
  std::cout << "test11\n";

  SGTest::TestStore testStore;

  SG::DataObjectSharedPtr<MyDObj> p1 (new MyDObj (300));
  assert (p1->refCount() == 1);

  SG::WriteHandle<MyDObj> h1 ("foo1", "FooSvc");
  assert (h1.setProxyDict (&testStore).isSuccess());
  assert (h1.put (p1) == p1.get());
  assert (p1->refCount() == 2);

  SG::DataObjectSharedPtr<MyDObj> p4 (new MyDObj (400));
  assert (p4->refCount() == 1);

  SG::WriteHandle<MyDObj> h4 ("foo4", "FooSvc");
  assert (h4.setProxyDict (&testStore).isSuccess());
  assert (h4.put (p4) == p4.get());
  assert (p4->refCount() == 2);

  SG::WriteHandle<MyDObj> h5 ("foo5");
  assert (h5.setProxyDict (&testStore).isSuccess());
  assert (h5.put (p1) == nullptr);
  assert (p1->refCount() == 2);

  // Record to a different context.
  MyObj::deleted.clear();
  SGTest::TestStore testStore2;
  EventContext ctx2;
  ctx2.setExtension( Atlas::ExtendedEventContext(&testStore2) );

  assert (h5.put (ctx2, p1) == p1.get());
}


// symlink/alias.
void test12()
{
  std::cout << "test12\n";

  SGTest::TestStore testStore;

  SG::WriteHandle<MyObj> h1 ("foo1", "FooSvc");
  assert (h1.setProxyDict (&testStore).isSuccess());

  assert (h1.record (std::make_unique<MyObj>(20)).isSuccess());
  assert (h1.isValid());
  assert (h1->x == 20);
  SG::DataProxy* prox1 = testStore.proxy (ClassID_traits<MyObj>::ID(), "foo1");

  // Making alias.
  SG::WriteHandleKey<MyObj> h2 ("foo3", "FooSvc");
  assert (h1.alias (h2).isSuccess());
  assert (testStore.proxy (ClassID_traits<MyObj>::ID(), "foo3") == prox1);
  assert (prox1->alias().count ("foo3") == 1);

  // Making symlink.
  SG::WriteHandleKey<MyObj2> h3 ("foo1", "FooSvc");
  assert (h1.symLink (h3).isSuccess());
  assert (testStore.proxy (ClassID_traits<MyObj2>::ID(), "foo1") == prox1);
  assert (prox1->transientID (ClassID_traits<MyObj2>::ID()));

  // Should give an error.
  SG::WriteHandleKey<MyObj2> h4 ("foo3", "FooSvc");
  assert (h1.symLink (h4).isFailure());
}


int main()
{
  errorcheck::ReportMessage::hideErrorLocus();
  errorcheck::ReportMessage::hideFunctionNames();
  ISvcLocator* svcloc;
  //need MessageSvc
  if (!Athena_test::initGaudi("VarHandleBase_test.txt", svcloc)) {
    return 1;
  }

  test1();
  test2();
  test3();
  test4();
  test5();
  test6();
  //test7();
  test8();
  test9();
  test10();
  test11();
  test12();
  return 0;
}
