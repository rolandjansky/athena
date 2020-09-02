/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file StoreGate/test/ReadHandle_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for ReadHandle.
 */


#undef NDEBUG
#include "AthenaKernel/ExtendedEventContext.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/exceptions.h"
#include "SGTools/TestStore.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "TestTools/initGaudi.h"
#include "TestTools/expect_exception.h"
#include "AthenaKernel/errorcheck.h"
#include "boost/timer/timer.hpp"
#include <cassert>
#include <iostream>


class MyObj
{
public:
  MyObj(int x=0) : x(x) {}
  int x;
};
CLASS_DEF (MyObj, 293847295, 1)
static const CLID MyCLID = 293847295;


// Ctors.
void test1()
{
  std::cout << "test1\n";

  SG::ReadHandle<MyObj> h1;
  assert (h1.clid() == MyCLID);
  assert (h1.key() == "");
  assert (h1.storeHandle().name() == "StoreGateSvc");
  assert (h1.mode() == Gaudi::DataHandle::Reader);

  SG::ReadHandle<MyObj> h2 ("foo", "FooSvc");
  assert (h2.clid() == MyCLID);
  assert (h2.key() == "foo");
  assert (h2.name() == "foo");
  assert (h2.storeHandle().name() == "FooSvc");
  assert (h2.mode() == Gaudi::DataHandle::Reader);

  SG::ReadHandleKey<MyObj> k3 ("asd");
  assert (k3.initialize().isSuccess());
  SG::ReadHandle<MyObj> h3 (k3);
  assert (h3.clid() == MyCLID);
  assert (h3.key() == "asd");
  assert (h3.storeHandle().name() == "StoreGateSvc");
  assert (h3.mode() == Gaudi::DataHandle::Reader);

  {
    SG::ReadHandleKey<MyObj> k4 ("asd", "BazSvc");
    k4.initialize().ignore();
    EXPECT_EXCEPTION (SG::ExcUninitKey, SG::ReadHandle<MyObj> h4 (k4));
  }

  SGTest::TestStore dumstore;
  EventContext ctx5;
  ctx5.setExtension( Atlas::ExtendedEventContext(&dumstore) );
  
  SG::ReadHandle<MyObj> h5 (k3, ctx5);
  assert (h5.clid() == MyCLID);
  assert (h5.key() == "asd");
  assert (h5.storeHandle().name() == "StoreGateSvc");
  assert (h5.mode() == Gaudi::DataHandle::Reader);
  assert (h5.store() == "TestStore");

  SG::ReadHandleKey<MyObj> k6 ("asd", "OtherStore");
  assert (k6.initialize().isSuccess());
  SG::ReadHandle<MyObj> h6 (k6, ctx5);
  assert (h6.clid() == MyCLID);
  assert (h6.key() == "asd");
  assert (h6.storeHandle().name() == "OtherStore");
  assert (h6.mode() == Gaudi::DataHandle::Reader);
  assert (h6.store() == "OtherStore" || h6.store() == "OtherStore_Impl");

  {
    SG::ReadHandleKey<MyObj> k7 ("asd", "BazSvc");
    k7.initialize().ignore();
    EXPECT_EXCEPTION (SG::ExcUninitKey, SG::ReadHandle<MyObj> h7 (k7, ctx5));
  }
}


// Copy.
void test2()
{
  std::cout << "test2\n";
  SGTest::TestStore testStore;

  MyObj* fooptr = new MyObj(13);
  testStore.record (fooptr, "foo");
  SG::DataProxy* foo_proxy = testStore.proxy (MyCLID, "foo");
  assert (foo_proxy->refCount() == 1);

  MyObj* barptr = new MyObj(14);
  testStore.record (barptr, "bar");
  SG::DataProxy* bar_proxy = testStore.proxy (MyCLID, "bar");
  assert (bar_proxy->refCount() == 1);

  SG::ReadHandle<MyObj> h1 ("foo", "FooSvc");
  assert (h1.store() == "FooSvc");
  assert (h1.setProxyDict (&testStore).isSuccess());
  assert (h1.store() == "TestStore");
  assert (h1.cptr() == fooptr);
  assert (foo_proxy->refCount() == 2);

  SG::ReadHandle<MyObj> h2 (h1);
  assert (h2.key() == "foo");
  assert (h2.store() == "TestStore");
  assert (h2.isInitialized());
  assert (h2.cptr() == fooptr);
  assert (foo_proxy->refCount() == 3);

  SG::ReadHandle<MyObj> h3 (std::move(h2));
  assert (h3.key() == "foo");
  assert (h3.store() == "TestStore");
  assert (h3.isInitialized());
  assert (h3.cptr() == fooptr);
  assert (foo_proxy->refCount() == 3);
  assert (h2.store() == "TestStore");
  assert (!h2.isInitialized());
  assert (h2.cachedPtr() == nullptr);

  SG::ReadHandle<MyObj> h4 ("bar", "FooSvc");
  assert (h4.setProxyDict (&testStore).isSuccess());
  assert (h4.cptr() == barptr);
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
  assert (h3.cachedPtr() == nullptr);

  assert (foo_proxy->refCount() == 2);
  assert (bar_proxy->refCount() == 3);
}


// Retrieve
void test3()
{
  std::cout << "test3\n";
  SGTest::TestStore testStore;

  MyObj* fooptr = new MyObj(23);
  testStore.record (fooptr, "foo");
  SG::DataProxy* foo_proxy = testStore.proxy (MyCLID, "foo");
  assert (foo_proxy->refCount() == 1);

  SG::ReadHandle<MyObj> h1 ("foo", "FooSvc");
  assert (h1.setProxyDict (&testStore).isSuccess());
  assert (!h1.isInitialized());
  assert (h1.get() == fooptr);
  assert (h1.cachedPtr() == nullptr);
  assert (h1.cptr() == fooptr);
  assert (h1.isInitialized());
  assert (h1.isValid());
  assert (h1.cachedPtr() == fooptr);
  assert (h1.ptr() == fooptr);
  assert (h1->x == 23);
  assert ((*h1).x == 23);
  assert (foo_proxy->refCount() == 2);

  SG::ReadHandle<MyObj> h2 ("foox", "FooSvc");
  assert (h2.setProxyDict (&testStore).isSuccess());
  assert (h2.cachedPtr() == nullptr);
  assert (!h2.isInitialized());
  assert (!h2.isValid());
  assert (h2.cachedPtr() == nullptr);
  assert (h2.ptr() == nullptr);
  assert (h2.cptr() == nullptr);
  [[maybe_unused]]
  int xx = 0;
  EXPECT_EXCEPTION (SG::ExcNullReadHandle, xx = (*h2).x);
  EXPECT_EXCEPTION (SG::ExcNullReadHandle, xx = h2->x );

  SG::ReadHandle<MyObj> h3 ("foox");
  assert (h3.setProxyDict (&testStore).isSuccess());
  assert (h3.get() == nullptr);
  SGTest::TestStore testStore2;
  MyObj* foox = new MyObj;
  testStore2.record (foox, "foox");
  EventContext ctx2;
  ctx2.setExtension( Atlas::ExtendedEventContext(&testStore2) );
  assert (h3.get(ctx2) == foox);
}


// makeHandle
void test4()
{
  std::cout << "test4\n";
  SGTest::TestStore testStore;

  SG::ReadHandleKey<MyObj> k1 ("asd");
  assert (k1.initialize().isSuccess());
  auto h1 = SG::makeHandle (k1);
  assert (h1.clid() == MyCLID);
  assert (h1.key() == "asd");
  assert (h1.storeHandle().name() == "StoreGateSvc");
  assert (h1.mode() == Gaudi::DataHandle::Reader);

  SG::ReadHandleKey<MyObj> k2 ("asd", "BazSvc");
  k2.initialize().ignore();
  EXPECT_EXCEPTION (SG::ExcUninitKey, SG::makeHandle (k2));

  SGTest::TestStore dumstore;
  EventContext ctx;
  ctx.setExtension( Atlas::ExtendedEventContext(&dumstore) );

  auto h2 = SG::makeHandle (k1, ctx);
  assert (h2.clid() == MyCLID);
  assert (h2.key() == "asd");
  assert (h2.storeHandle().name() == "StoreGateSvc");
  assert (h2.mode() == Gaudi::DataHandle::Reader);
  assert (h2.store() == "TestStore");

  SG::ReadHandleKey<MyObj> k3 ("asd", "OtherStore");
  assert (k3.initialize().isSuccess());
  auto h3 = SG::makeHandle (k3, ctx);
  assert (h3.clid() == MyCLID);
  assert (h3.key() == "asd");
  assert (h3.storeHandle().name() == "OtherStore");
  assert (h3.mode() == Gaudi::DataHandle::Reader);
  assert (h3.store() == "OtherStore" || h3.store() == "OtherStore_Impl");

  SG::ReadHandleKey<MyObj> k4 ("asd", "BazSvc");
  k4.initialize().ignore();
  EXPECT_EXCEPTION (SG::ExcUninitKey, SG::makeHandle (k4, ctx));
}


// alias.
void test5()
{
  std::cout << "test5\n";

  SGTest::TestStore testStore;

  SG::ReadHandle<MyObj> h1 ("foo1", "FooSvc");
  assert (h1.setProxyDict (&testStore).isSuccess());

  MyObj* fooptr = new MyObj(20);
  testStore.record (fooptr, "foo1");
  SG::DataProxy* prox1 = testStore.proxy (MyCLID, "foo1");
  //assert (foo_proxy->refCount() == 1);

#if 0
  assert (h1.record (std::make_unique<MyObj>(20)).isSuccess());
  assert (h1.isValid());
  assert (h1->x == 20);
  SG::DataProxy* prox1 = testStore.proxy (ClassID_traits<MyObj>::ID(), "foo1");
  #endif

  // Making alias.
  SG::WriteHandleKey<MyObj> h2 ("foo3", "FooSvc");
  assert (h1.alias (h2).isSuccess());
  assert (testStore.proxy (MyCLID, "foo3") == prox1);
  assert (prox1->alias().count ("foo3") == 1);
  #if 0

  // Making symlink.
  SG::WriteHandleKey<MyObj2> h3 ("foo1", "FooSvc");
  assert (h1.symLink (h3).isSuccess());
  assert (testStore.proxy (ClassID_traits<MyObj2>::ID(), "foo1") == prox1);
  assert (prox1->transientID (ClassID_traits<MyObj2>::ID()));

  // Should give an error.
  SG::WriteHandleKey<MyObj2> h4 ("foo3", "FooSvc");
  assert (h1.symLink (h4).isFailure());
  #endif
}


// constructor from proxy.
void test6()
{
  std::cout << "test6\n";

  auto obj = std::make_unique<MyObj>();
  MyObj* objptr = obj.get();
  SG::TransientAddress taddr (293847295, "foo13");
  SG::DataProxy* proxy = new SG::DataProxy (SG::asStorable (std::move(obj)),
                                            std::move (taddr));
  proxy->addRef();
  assert (proxy->refCount() == 1);

  SGTest::TestStore testStore;
  proxy->setStore (&testStore);

  {
    SG::ReadHandle<MyObj> h1 (proxy);
    assert (h1.clid() == 293847295);
    assert (h1.key() == "foo13");
    assert (h1.store() == "TestStore");
    assert (h1.mode() == Gaudi::DataHandle::Reader);
    assert (h1.cptr() == objptr);
    assert (proxy->refCount() == 2);
    assert (testStore.m_boundHandles == std::vector<IResetable*>{&h1});
  }
  assert (proxy->refCount() == 1);
  assert (testStore.m_boundHandles == std::vector<IResetable*>{});
}


//************************************************************************


unsigned int perftest (ISvcLocator* svcloc, unsigned int ntry)
{
  StoreGateSvc* sg = nullptr;
  assert (svcloc->service ("StoreGateSvc", sg).isSuccess());
  assert (sg->record (std::make_unique<MyObj> (42), "MyObj", false).isSuccess());

  SG::ReadHandleKey<MyObj> key ("MyObj");
  assert (key.initialize().isSuccess());

  EventContext ctx;
  ctx.setExtension( Atlas::ExtendedEventContext(sg->hiveProxyDict()) );
  unsigned int sum = 0;
  boost::timer::cpu_timer timer;
  for (unsigned int i=0; i < ntry; i++) {
    SG::ReadHandle<MyObj> h (key, ctx);
    sum += h->x;
  }
  timer.stop();
  boost::timer::cpu_times times = timer.elapsed();
  std::cout << ntry << " times: " << boost::timer::format(times);
  std::cout << "Each: " << (float)times.user / ntry / 1000 << " us (user)\n";

  return sum;
}



//************************************************************************


int main (int argc, char** argv)
{
  errorcheck::ReportMessage::hideErrorLocus();
  ISvcLocator* svcloc;
  //need MessageSvc
  if (!Athena_test::initGaudi("StoreGate/VarHandleBase_test.txt", svcloc)) {
    return 1;
  }

  if (argc >= 2 && strncmp (argv[1], "--perf", 6) == 0) {
    unsigned int ntry = 1000000;
    const char* p = strchr (argv[1], '=');
    if (p) {
      ntry = atoi (p+1);
      if (ntry < 1) ntry = 1;
    }
    perftest (svcloc, ntry);
    return 0;
  }

  test1();
  test2();
  test3();
  test4();
  test5();
  test6();
  return 0;
}
