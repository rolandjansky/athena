/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthLinks/test/DataLinkBase_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2014
 * @brief Regression tests for DataLinkBase
 */

#undef NDEBUG
#include "AthLinks/DataLinkBase.h"
#include "AthLinks/exceptions.h"
#include "SGTools/CurrentEventStore.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/getMessageSvc.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/EventContext.h"
#include <iostream>
#include <cassert>


#include "SGTools/TestStore.h"
#include "TestTools/expect_exception.h"


using namespace SGTest;


class DataLinkBase_test
  : public DataLinkBase
{
public:
  DataLinkBase_test() {}
  DataLinkBase_test(SG::sgkey_t key, CLID clid) : DataLinkBase (key, clid, 0) {}
  DataLinkBase_test(const DataLinkBase::ID_type& key, CLID clid)
    : DataLinkBase (key, clid, 0) {}
  DataLinkBase_test(DataLinkBase::const_pointer_t obj,
                    CLID clid) : DataLinkBase (obj, clid, 0) {}
  DataLinkBase_test(sgkey_t key, const SG::DataProxyHolder& holder)
    : DataLinkBase (key, holder) {}

  using DataLinkBase::storableBase;
  using DataLinkBase::toStorableObject;
  using DataLinkBase::toIdentifiedObject;
  using DataLinkBase::throwInvalidLink;
};


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

  DataLinkBase_test l1;
  assert (l1.isDefault());
  assert (l1.dataID() == "");
  assert (l1.key() == 0);
  assert (l1.storableBase (foocast, fooclid) == 0);
  assert (l1.proxy() == 0);
  assert (l1.source() == 0);

  Foo* foo1 = new Foo(1);
  store.record (foo1, "foo1");
  TestStore::sgkey_t sgkey1 = store.stringToKey ("foo1", fooclid);
  TestStore::sgkey_t sgkey3 = store.stringToKey ("foo3", fooclid);

  // An object not in SG
  Foo* foo2 = new Foo(2);

  // toStorableObject

  l1.toStorableObject (foo1, fooclid, 0);
  assert (!l1.isDefault());
  assert (l1.dataID() == "foo1");
  assert (l1.key() == sgkey1);
  assert (l1.storableBase (foocast, fooclid) == foo1);
  assert (l1.proxy()->name() == "foo1");
  assert (l1.source() == &store);

  l1.toStorableObject (foo2, fooclid, 0);
  assert (!l1.isDefault());
  assert (l1.dataID() == "");
  assert (l1.key() == 0);
  assert (l1.storableBase (foocast, fooclid) == foo2);
  EXPECT_EXCEPTION (SG::ExcPointerNotInSG,
                    assert (l1.proxy()->name() == "foo2"));
  EXPECT_EXCEPTION (SG::ExcPointerNotInSG,
                    assert (l1.source() == &store));
  EXPECT_EXCEPTION (SG::ExcCLIDMismatch,
                    l1.toStorableObject (foo1, barclid, 0));

  // toIdentifiedObject with string key

  l1.toIdentifiedObject ("foo1", fooclid, 0);
  assert (!l1.isDefault());
  assert (l1.dataID() == "foo1");
  assert (l1.key() == sgkey1);
  assert (l1.storableBase (foocast, fooclid) == foo1);
  assert (l1.proxy()->name() == "foo1");
  assert (l1.source() == &store);

  l1.toIdentifiedObject ("foo3", fooclid, 0);
  assert (!l1.isDefault());
  assert (l1.dataID() == "foo3");
  assert (l1.key() == sgkey3);
  assert (l1.storableBase (foocast, fooclid) == 0);
  assert (l1.proxy()->name() == "foo3");
  assert (l1.source() == &store);

  // toIdentifiedObject with hashed key

  l1.toIdentifiedObject (sgkey1, fooclid, 0);
  assert (!l1.isDefault());
  assert (l1.dataID() == "foo1");
  assert (l1.key() == sgkey1);
  assert (l1.storableBase (foocast, fooclid) == foo1);
  assert (l1.proxy()->name() == "foo1");
  assert (l1.source() == &store);

  EXPECT_EXCEPTION (SG::ExcCLIDMismatch,
                    l1.toIdentifiedObject (sgkey1, barclid, 0));

  l1.toIdentifiedObject (sgkey3, fooclid, 0);
  assert (!l1.isDefault());
  assert (l1.dataID() == "foo3");
  assert (l1.key() == sgkey3);
  assert (l1.storableBase (foocast, fooclid) == 0);
  assert (l1.proxy()->name() == "foo3");
  assert (l1.source() == &store);


  l1.clear();
  assert (l1.isDefault());
  assert (l1.dataID() == "");
  assert (l1.key() == 0);
  assert (l1.storableBase (foocast, fooclid) == 0);
  assert (l1.proxy() == 0);
  assert (l1.source() == 0);

  DataLinkBase_test l2 (foo1, fooclid);
  assert (!l2.isDefault());
  assert (l2.dataID() == "foo1");
  assert (l2.key() == sgkey1);
  assert (l2.storableBase (foocast, fooclid) == foo1);
  assert (l2.proxy()->name() == "foo1");
  assert (l2.source() == &store);

  DataLinkBase_test l3 (static_cast<std::string>("foo1"), fooclid);
  assert (!l3.isDefault());
  assert (l3.dataID() == "foo1");
  assert (l3.key() == sgkey1);
  assert (l3.storableBase (foocast, fooclid) == foo1);
  assert (l3.proxy()->name() == "foo1");
  assert (l3.source() == &store);

  DataLinkBase_test l4 (sgkey1, fooclid);
  assert (!l4.isDefault());
  assert (l4.dataID() == "foo1");
  assert (l4.key() == sgkey1);
  assert (l4.storableBase (foocast, fooclid) == foo1);
  assert (l4.proxy()->name() == "foo1");
  assert (l4.source() == &store);

  SG::DataProxyHolder holder;
  holder.toIdentifiedObject ("foo1", fooclid, 0);
  DataLinkBase_test l5 (sgkey1, holder);
  assert (!l5.isDefault());
  assert (l5.dataID() == "foo1");
  assert (l5.key() == sgkey1);
  assert (l5.storableBase (foocast, fooclid) == foo1);
  assert (l5.proxy()->name() == "foo1");
  assert (l5.source() == &store);
}


// toPersistent / toTransient
void test2 (SGTest::TestStore& store)
{
  std::cout << "test2\n";

  Foo* foo4 = new Foo(4);
  store.record (foo4, "foo4");
  Foo* fooz = new Foo(999);
  store.record (fooz, "fooz");
  TestStore::sgkey_t sgkey4 = store.stringToKey ("foo4", fooclid);
  TestStore::sgkey_t sgkeyz = store.stringToKey ("fooz", fooclid);

  EventContext ctx;
  ctx.setExtension( Atlas::ExtendedEventContext(&store) );

  DataLinkBase_test l1 (sgkey4, fooclid);
  l1.toTransient();
  assert (!l1.isDefault());
  assert (l1.dataID() == "foo4");
  assert (l1.key() == sgkey4);
  assert (l1.storableBase (foocast, fooclid) == foo4);
  assert (l1.proxy()->name() == "foo4");
  assert (l1.source() == &store);
  l1.toTransient(ctx);

  l1.clear();
  assert (l1.toPersistentNoRemap() == true);
  assert (l1.key() == 0);

  Foo* foo5 = new Foo(5);
  l1.toStorableObject (foo5, fooclid, 0);
  EXPECT_EXCEPTION (SG::ExcPointerNotInSG, l1.toPersistentNoRemap());
  assert (l1.key() == 0);

  l1.toIdentifiedObject ("foo4", fooclid, 0);
  l1.toPersistentNoRemap();
  assert (l1.key() == sgkey4);

  assert (l1.toPersistent() == true);
  assert (l1.key() == sgkey4);

  store.remap<Foo> ("foo4", "foo4", 0, 0);
  assert (l1.toPersistent() == true);
  assert (l1.key() == sgkey4);
  assert (l1.dataID() == "foo4");

  store.remap<Foo> ("foo4", "foo4a", 0, 0);
  assert (l1.toPersistent() == true);
  assert (l1.dataID() == "foo4a");
  assert (l1.key() == store.stringToKey ("foo4a", fooclid));

  l1.clear();
  assert (l1.toTransient ("fooz", fooclid));
  assert (!l1.isDefault());
  assert (l1.dataID() == "fooz");
  assert (l1.key() == sgkeyz);
  assert (l1.storableBase (foocast, fooclid) == fooz);
  assert (l1.proxy()->name() == "fooz");
  assert (l1.source() == &store);

  EXPECT_EXCEPTION (SG::ExcBadToTransient,
                    l1.toTransient ("foozz", fooclid, ctx));
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

  DataLinkBase_test l1;
  DataLinkBase_test l2;

  assert (l1 == l2);
  assert (!(l1 != l2));

  l1.toStorableObject (foo5, fooclid, 0);
  store.record (foo5, "foo5");
  l2.toIdentifiedObject ("foo5", fooclid, 0);
  assert (l1 == l2);
  assert (l2 == l1);
  assert (!(l1 != l2));
  assert (!(l2 != l1));
  l2.toIdentifiedObject ("foo6", fooclid, 0);
  assert (!(l1 == l2));
  assert (!(l2 == l1));
  assert (l1 != l2);
  assert (l2 != l1);
}


// other
void test5 (SGTest::TestStore&)
{
  std::cout << "test5\n";

  DataLinkBase_test l1;
  l1.toIdentifiedObject ("foo6", fooclid, 0);
  EXPECT_EXCEPTION (SG::ExcInvalidLink, l1.throwInvalidLink());
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  Athena::getMessageSvcQuiet = true;
  SGTest::initTestStore();

  test1 (store);
  test2 (store);
  test3 (store);
  test4 (store);
  test5 (store);
  return 0;
}
