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
#include "AthLinks/tools/SGgetDataSource.h"
#include "AthLinks/exceptions.h"
#include "SGTools/CLASS_DEF.h"
#include "AthenaKernel/getMessageSvc.h"
#include "AthenaKernel/errorcheck.h"
#include <iostream>
#include <cassert>


#include "TestStore.icc"
#include "TestThinningSvc.icc"
#include "expect_exception.icc"


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


void test1()
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

  h1.toIdentifiedObject (sgkey1, fooclid, 0);
  assert (!h1.isDefault());
  assert (h1.dataID() == "foo1");
  assert (h1.storableBase (foocast, fooclid) == foo1);
  assert (h1.proxy()->name() == "foo1");
  assert (h1.source() == &store);

  EXPECT_EXCEPTION (SG::ExcCLIDMismatch,
                    h1.toIdentifiedObject (sgkey1, barclid, 0));

  h1.toIdentifiedObject (sgkey3, fooclid, 0);
  assert (!h1.isDefault());
  assert (h1.dataID() == "foo3");
  assert (h1.storableBase (foocast, fooclid) == 0);
  assert (h1.proxy()->name() == "foo3");
  assert (h1.source() == &store);

  h1.clear();
  assert (h1.isDefault());
  assert (h1.dataID() == "");
  assert (h1.storableBase (foocast, fooclid) == 0);
  assert (h1.proxy() == 0);
  assert (h1.source() == 0);
}


// toPersistent / toTransient
void test2()
{
  std::cout << "test2\n";

  Foo* foo4 = new Foo(4);
  store.record (foo4, "foo4");
  TestStore::sgkey_t sgkey4 = store.stringToKey ("foo4", fooclid);

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


IProxyDictWithPool* storePtr2 = &store;
IProxyDictWithPool** getTestDataSourcePointer2 (const std::string&)
{
  return &storePtr2;
}


// alt store
void test3()
{
  std::cout << "test3\n";

  TestStore store2;

  assert (SG::DataProxyHolder::defaultDataSource() == &store);
  SG::getDataSourcePointerFunc_t* old_fn = SG::getDataSourcePointerFunc;
  SG::getDataSourcePointerFunc = getTestDataSourcePointer2;
  assert (SG::DataProxyHolder::defaultDataSource() == &store);
  storePtr2 = &store2;
  assert (SG::DataProxyHolder::defaultDataSource() == &store);
  SG::DataProxyHolder::resetCachedSource();
  assert (SG::DataProxyHolder::defaultDataSource() == &store2);

  SG::getDataSourcePointerFunc = old_fn;
  SG::DataProxyHolder::resetCachedSource();
}


// equality
void test4()
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
void test5()
{
  std::cout << "test5\n";

  SG::DataProxyHolder h1;
  TestStore::sgkey_t sgkey6 = store.stringToKey ("foo6", fooclid);
  assert (h1.toIdentifiedObject ("foo6", fooclid, 0) == sgkey6);
  EXPECT_EXCEPTION (SG::ExcInvalidLink,
                    h1.throwInvalidLink (sgkey6));
}


// thinning
void test6()
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


int main()
{
  errorcheck::ReportMessage::hideErrorLocus(true);
  Athena::getMessageSvcQuiet = true;
  SG::getDataSourcePointerFunc = getTestDataSourcePointer;

  test1();
  test2();
  test3();
  test4();
  test5();
  test6();
  return 0;
}
