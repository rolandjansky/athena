/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthLinks/test/DataProxyHolder_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2014
 * @brief Regression tests for DataProxyHolder
 */

#undef NDEBUG
#include "AthLinks/tools/DataProxyHolder.h"
#include "AthLinks/exceptions.h"
#include "SGTools/StringPool.h"
#include "AthenaKernel/getMessageSvc.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/errorcheck.h"
#include <iostream>
#include <cassert>


#include "SGTools/TestStore.h"
#include "TestTools/expect_exception.h"
#include "TestThinningSvc.icc"


using namespace SGTest;


struct Foo
{
  Foo (int the_x) : x (the_x) {}
  virtual ~Foo() {}
  int x;
};
const unsigned int fooclid = 23423556;
CLASS_DEF (Foo, fooclid, 1)


struct Bar
  : public Foo
{
  Bar (int the_x) : Foo (the_x) {}
  virtual ~Bar() {}
};
const unsigned int barclid = 23423557;
CLASS_DEF (Bar, barclid, 1)


void* foocast (SG::DataProxy*)
{
  return 0;
}


void test1 (SGTest::TestStore& store)
{
  std::cout << "test1\n";

  SG::DataProxyHolder h1;
  assert (h1.isDefault());
  assert (h1.dataID() == "");
  assert (h1.storableBase (foocast, fooclid) == 0);
  assert (h1.proxy() == 0);
  assert (h1.source() == 0);

  Foo* foo1 = new Foo(1);
  store.record (foo1, "foo1");
  TestStore::sgkey_t sgkey1 = store.stringToKey ("foo1", fooclid);
  TestStore::sgkey_t sgkey3 = store.stringToKey ("foo3", fooclid);
  TestStore::sgkey_t sgkey3x = store.stringToKey ("foo3x", fooclid);

  // An object not in SG
  Foo* foo2 = new Foo(2);

  // toStorableObject

  assert (h1.toStorableObject (foo1, fooclid, 0) == sgkey1);
  assert (!h1.isDefault());
  assert (h1.dataID() == "foo1");
  assert (h1.storableBase (foocast, fooclid) == foo1);
  assert (h1.proxy()->name() == "foo1");
  assert (h1.source() == &store);

  assert (h1.toStorableObject (foo2, fooclid, 0) == 0);
  assert (!h1.isDefault());
  assert (h1.dataID() == "");
  assert (h1.storableBase (foocast, fooclid) == foo2);
  EXPECT_EXCEPTION (SG::ExcPointerNotInSG,
                    assert (h1.proxy()->name() == "foo2"));
  EXPECT_EXCEPTION (SG::ExcPointerNotInSG,
                    assert (h1.source() == &store));
  EXPECT_EXCEPTION (SG::ExcCLIDMismatch,
                    h1.toStorableObject (foo1, barclid, 0));

  // toIdentifiedObject with string key

  assert (h1.toIdentifiedObject ("foo1", fooclid, 0) == sgkey1);
  assert (!h1.isDefault());
  assert (h1.dataID() == "foo1");
  assert (h1.storableBase (foocast, fooclid) == foo1);
  assert (h1.proxy()->name() == "foo1");
  assert (h1.source() == &store);

  assert (h1.toIdentifiedObject ("foo3", fooclid, 0) == sgkey3);
  assert (!h1.isDefault());
  assert (h1.dataID() == "foo3");
  assert (h1.storableBase (foocast, fooclid) == 0);
  assert (h1.proxy()->name() == "foo3");
  assert (h1.source() == &store);

  // toIdentifiedObject with hashed key

  store.m_missedProxies.clear();
  h1.toIdentifiedObject (sgkey1, fooclid, 0);
  assert (!h1.isDefault());
  assert (h1.dataID() == "foo1");
  assert (h1.storableBase (foocast, fooclid) == foo1);
  assert (h1.proxy()->name() == "foo1");
  assert (h1.source() == &store);
  assert (store.m_missedProxies.empty());

  EXPECT_EXCEPTION (SG::ExcCLIDMismatch,
                    h1.toIdentifiedObject (sgkey1, barclid, 0));

  h1.toIdentifiedObject (sgkey3, fooclid, 0);
  assert (!h1.isDefault());
  assert (h1.dataID() == "foo3");
  assert (h1.storableBase (foocast, fooclid) == 0);
  assert (h1.proxy()->name() == "foo3");
  assert (h1.source() == &store);

  h1.toIdentifiedObject (sgkey3x, fooclid, 0);
  assert (!h1.isDefault());
  assert (h1.dataID() == "foo3x");
  assert (h1.storableBase (foocast, fooclid) == 0);
  assert (h1.proxy()->name() == "foo3x");
  assert (h1.source() == &store);
  assert (store.m_missedProxies.size() == 1);
  assert (store.m_missedProxies[0].first == fooclid);
  assert (store.m_missedProxies[0].second == "foo3x");

  h1.clear();
  assert (h1.isDefault());
  assert (h1.dataID() == "");
  assert (h1.storableBase (foocast, fooclid) == 0);
  assert (h1.proxy() == 0);
  assert (h1.source() == 0);
}


// toPersistent / toTransient
void test2 (SGTest::TestStore& store)
{
  std::cout << "test2\n";

  Foo* foo4 = new Foo(4);
  store.record (foo4, "foo4");
  Foo* fooy = new Foo(99);
  store.record (fooy, "fooy");
  Foo* fooz = new Foo(999);
  store.record (fooz, "fooz");
  TestStore::sgkey_t sgkey4 = store.stringToKey ("foo4", fooclid);
  TestStore::sgkey_t sgkeyx = store.stringToKey ("foox", fooclid);
  TestStore::sgkey_t sgkeyz = store.stringToKey ("fooz", fooclid);
  /*TestStore::sgkey_t sgkeyy =*/ store.stringToKey ("fooy", fooclid);

  SG::DataProxyHolder h1;
  h1.toTransient  (sgkey4);
  assert (!h1.isDefault());
  assert (h1.dataID() == "foo4");
  assert (h1.storableBase (foocast, fooclid) == foo4);
  assert (h1.proxy()->name() == "foo4");
  assert (h1.source() == &store);

  h1.clear();
  TestStore::sgkey_t sgkey = 0;
  h1.toPersistentNoRemap (sgkey);
  assert (sgkey == 0);

  h1.toTransient  (sgkeyx);
  assert (!h1.isDefault());
  assert (h1.dataID() == "fooy");
  assert (h1.storableBase (foocast, fooclid) == fooy);
  assert (h1.proxy()->name() == "fooy");
  assert (h1.source() == &store);

  h1.clear();
  assert (h1.toTransient ("fooz", fooclid) == sgkeyz);
  assert (!h1.isDefault());
  assert (h1.dataID() == "fooz");
  assert (h1.storableBase (foocast, fooclid) == fooz);
  assert (h1.proxy()->name() == "fooz");
  assert (h1.source() == &store);

  Foo* foo5 = new Foo(5);
  h1.toStorableObject (foo5, fooclid, 0);
  sgkey = 0;
  EXPECT_EXCEPTION (SG::ExcPointerNotInSG, h1.toPersistentNoRemap (sgkey));

  sgkey = 0;
  h1.toIdentifiedObject ("foo4", fooclid, 0);
  h1.toPersistentNoRemap (sgkey);
  assert (sgkey == sgkey4);

  sgkey = 0;
  size_t index = 10;
  assert (h1.toPersistent (sgkey, index) == false);
  assert (sgkey == sgkey4);
  assert (index == 10);

  store.remap<Foo> ("foo4", "foo4", 10, 20);
  assert (h1.toPersistent (sgkey, index) == true);
  assert (sgkey == sgkey4);
  assert (index == 20);
  assert (h1.dataID() == "foo4");

  store.remap<Foo> ("foo4", "foo4a", 10, 20);
  index = 10;
  assert (h1.toPersistent (sgkey, index) == true);
  assert (index == 20);
  assert (h1.dataID() == "foo4a");
  assert (sgkey == store.stringToKey ("foo4a", fooclid));

  h1.toIdentifiedObject ("foo4", fooclid, 0);
  std::string sindex = "10";
  sgkey = 0;
  assert (h1.toPersistent (sgkey, sindex) == false);
  assert (sgkey == sgkey4);
  assert (sindex == "10");
}


// alt store
void test3 (SGTest::TestStore& store)
{
  std::cout << "test3\n";

  TestStore store2;

  assert (SG::CurrentEventStore::setStore(&store2) == &store);
  assert (SG::CurrentEventStore::setStore(&store) == &store2);
}


// equality
void test4 (SGTest::TestStore& store)
{
  std::cout << "test4\n";

  Foo* foo5 = new Foo(5);

  SG::DataProxyHolder h1;
  SG::DataProxyHolder h2;

  assert (h1 == h2);

  h1.toStorableObject (foo5, fooclid, 0);
  store.record (foo5, "foo5");
  h2.toIdentifiedObject ("foo5", fooclid, 0);
  assert (h1 == h2);
  assert (h2 == h1);
  h2.toIdentifiedObject ("foo6", fooclid, 0);
  assert (!(h1 == h2));
  assert (!(h2 == h1));
}


// other
void test5 (SGTest::TestStore& store)
{
  std::cout << "test5\n";

  SG::DataProxyHolder h1;
  TestStore::sgkey_t sgkey6 = store.stringToKey ("foo6", fooclid);
  assert (h1.toIdentifiedObject ("foo6", fooclid, 0) == sgkey6);
  EXPECT_EXCEPTION (SG::ExcInvalidLink,
                    h1.throwInvalidLink (sgkey6));
}


// thinning
void test6 (SGTest::TestStore& store)
{
  std::cout << "test6\n";
  TestThinningSvc svc;
  SG::DataProxyHolder h1;
  TestStore::sgkey_t sgkey = 0;
  size_t index = 10;
  
  assert (h1.thin (sgkey, index) == false);
  assert (sgkey == 0);
  assert (index == 10);

  TestStore::sgkey_t sgkey_foo = store.stringToKey ("foo", fooclid);
  sgkey = sgkey_foo;
  assert (h1.thin (sgkey, index) == false);
  assert (sgkey == sgkey_foo);
  assert (index == 10);

  TestThinningSvc::instance (&svc, true);
  assert (h1.thin (sgkey, index) == false);
  assert (sgkey == sgkey_foo);
  assert (index == 10);

  Foo* foo = new Foo(0);
  store.record (foo, "foo");
  h1.toTransient (sgkey);
  assert (h1.thin (sgkey, index) == false);
  assert (sgkey == sgkey_foo);
  assert (index == 10);

  svc.remap (10, 12);
  assert (h1.thin (sgkey, index) == true);
  assert (sgkey == sgkey_foo);
  assert (index == 12);

  svc.remap (20, IThinningSvc::RemovedIdx);
  index = 20;
  assert (h1.thin (sgkey, index) == true);
  assert (sgkey == 0);
  assert (index == 0);
}


// converting ctor
void test7 (SGTest::TestStore& store)
{
  std::cout << "test7\n";

  SG::DataProxyHolder dp1;

  // An object not in SG
  Bar* bar2 = new Bar(2);
  assert (dp1.toStorableObject (bar2, barclid, 0) == 0);

  SG::DataProxyHolder dp2 (dp1, (Bar*)0, (Bar*)0);
  assert (dp1.storableBase (foocast, barclid) == bar2);
  assert (dp2.storableBase (foocast, barclid) == bar2);

  SG::DataProxyHolder dp3 (dp1, (Bar*)0, (Foo*)0);
  assert (dp3.storableBase (foocast, fooclid) == static_cast<Foo*>(bar2));

  // An object in SG
  TestStore::sgkey_t sgkey = store.stringToKey ("bar2", barclid);
  store.record (bar2, "bar2");
  SG::DataProxyHolder dp4;
  assert (dp4.toIdentifiedObject ("bar2", barclid, 0) == sgkey);
  assert (dp4.dataID() == "bar2");
  assert (dp4.storableBase (foocast, barclid) == bar2);

  SG::DataProxyHolder dp5 (dp4, (Bar*)0, (Bar*)0);
  assert (dp5.dataID() == "bar2");
  assert (dp5.storableBase (foocast, barclid) == bar2);

  SG::DataProxyHolder dp6 (dp4, (Bar*)0, (Foo*)0);
  assert (dp6.dataID() == "bar2");
  assert (dp6.storableBase (foocast, barclid) == bar2);
}


SG::DataProxyHolder::InputRenameRCU_t inputRenameMap(1);


void initInputRename ATLAS_NOT_THREAD_SAFE ()
{
  SG::StringPool sp;
  auto m = std::make_unique<SG::DataProxyHolder::InputRenameMap_t>();
  (*m)[sp.stringToKey("foox", fooclid)]
    = sp.stringToKey("fooy", fooclid);
  Athena::RCUUpdate<SG::DataProxyHolder::InputRenameMap_t> u (inputRenameMap);
  u.update (std::move (m));
  SG::DataProxyHolder::setInputRenameMap (&inputRenameMap);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  errorcheck::ReportMessage::hideErrorLocus(true);
  Athena::getMessageSvcQuiet = true;
  SGTest::initTestStore();
  initInputRename();

  test1 (store);
  test2 (store);
  test3 (store);
  test4 (store);
  test5 (store);
  test6 (store);
  test7 (store);
  return 0;
}
