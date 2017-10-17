/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthLinks/test/DataLink_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2013
 * @brief Regression tests for DataLink
 */

#undef NDEBUG
#include "AthLinks/DataLink.h"
#include "SGTools/CurrentEventStore.h"
#include "SGTools/StorableConversions.h"
#include "SGTools/DataProxy.h"
#include "SGTools/TransientAddress.h"
#include "AthenaKernel/ClassID_traits.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/IProxyDict.h"
#include "AthenaKernel/getMessageSvc.h"
#include "CxxUtils/checker_macros.h"
#include <iostream>
#include <cstdlib>
#include <cassert>


#include "SGTools/TestStore.h"
#include "TestTools/expect_exception.h"


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


void test1 (SGTest::TestStore& store)
{
  std::cout << "test1\n";

  DataLink<Foo> dl1;
  assert (dl1.isDefault());
  assert (!dl1.isValid());
  assert (!dl1);
  assert (dl1.cptr() == 0);
  assert (dl1.getDataPtr() == 0);
  assert (dl1.getDataNonConstPtr() == 0);
  assert (dl1.dataID() == "");
  assert (dl1.key() == 0);
  assert (dl1.source() == 0);

  assert (dl1.classID() == fooclid);

  DataLink<Foo> dl2 ((Foo*)0);
  assert (dl2.isDefault());
  assert (!dl2.isValid());
  assert (!dl2);
  assert (dl2.cptr() == 0);
  assert (dl2.getDataPtr() == 0);
  assert (dl2.getDataNonConstPtr() == 0);
  assert (dl2.dataID() == "");
  assert (dl2.key() == 0);
  assert (dl2.source() == 0);

  assert (dl1 == dl2);
  assert (! (dl1 != dl2));

  Foo* foo2 = new Foo(10);

  store.record (foo2, "foo2");
  DataLink<Foo> dl3 (foo2);
  assert (!dl3.isDefault());
  assert (dl3.isValid());
  assert (!!dl3);
  assert (dl3.cptr() == foo2);
  assert (dl3->x == 10);
  assert ((*dl3).x == 10);
  const Foo* foo2a = dl3;
  assert (foo2a == foo2);
  assert (foo2 == dl3.getDataPtr());
  assert (foo2 == dl3.getDataNonConstPtr());
  assert (dl3.source() == &store);

  Bar* bar = new Bar (5);
  store.record (bar, "bar");

  EXPECT_EXCEPTION (SG::ExcCLIDMismatch, DataLink<Foo> dlx (bar));

  DataLink<Bar> dlb (bar);
  assert (dlb.isValid());

  DataLink<Foo> dl4 (dl3);
  assert (dl4.cptr() == foo2);
  assert (dl4 == dl3);
  assert (dl4 != dl1);
  assert (dl4.dataID() == "foo2");
  assert (dl4.key() == store.stringToKey ("foo2", fooclid));

  dl4.clear();
  assert (dl4.isDefault());
  assert (dl4.cptr() == 0);
  dl4 = dl3;
  assert (dl4.cptr() == foo2);
  dl4.clear();
  assert (dl4.isDefault());
  assert (dl4.cptr() == 0);

  const Foo& foo2b = *foo2;
  DataLink<Foo> dl5 (foo2b);
  assert (dl5.cptr() == foo2);

  DataLink<Foo> dl6 ("foo2");
  assert (!dl6.isDefault());
  assert (dl6.isValid());
  assert (!!dl6);
  assert (dl6.cptr() == foo2);
  assert (dl6->x == 10);

  TestStore::sgkey_t sgkey = store.stringToKey ("foo2", fooclid);
  DataLink<Foo> dl7 (sgkey);
  assert (!dl7.isDefault());
  assert (dl7.isValid());
  assert (!!dl7);
  assert (dl7.cptr() == foo2);
  assert (dl7->x == 10);

  EXPECT_EXCEPTION (SG::ExcCLIDMismatch,
                    {
                      TestStore::sgkey_t sgkey_x =
                        store.stringToKey ("bar", barclid);
                      DataLink<Foo> dlx (sgkey_x);
                    });

  TestStore store2;
  Foo* foo3 = new Foo(20);
  store2.record (foo3, "foo2");

  EventContext ctx;
  ctx.setExtension( Atlas::ExtendedEventContext(&store2) );

  DataLink<Foo> dl8 (foo3, &store2);
  assert (dl8.cptr() == foo3);
  DataLink<Foo> dl8a (foo3, ctx);
  assert (dl8a.cptr() == foo3);
  DataLink<Foo> dl9 (*foo3, &store2);
  assert (dl9.cptr() == foo3);
  DataLink<Foo> dl9a (*foo3, ctx);
  assert (dl9a.cptr() == foo3);
  DataLink<Foo> dl10 ("foo2", &store2);
  assert (dl10.cptr() == foo3);
  DataLink<Foo> dl10a ("foo2", ctx);
  assert (dl10a.cptr() == foo3);
  DataLink<Foo> dl11 (sgkey, &store2);
  assert (dl11.cptr() == foo3);
  assert (dl11.source() == &store2);
  DataLink<Foo> dl11a (sgkey, ctx);
  assert (dl11a.cptr() == foo3);
  assert (dl11a.source() == &store2);

  Foo* foo4 = new Foo(40);

  EXPECT_EXCEPTION (SG::ExcCLIDMismatch, dl3.toStorableObject (*bar));

  store.record (foo4, "foo4");
  dl3.toStorableObject (*foo4);
  assert (dl3.cptr() == foo4);
  dl3.toStorableObject (*foo3, &store2);
  assert (dl3.cptr() == foo3);

  dl3.toIdentifiedObject ("foo2", &store);
  assert (dl3.cptr() == foo2);
  dl3.toIdentifiedObject ("foo4");
  assert (dl3.cptr() == foo4);

  dl3.toIdentifiedObject (sgkey);
  assert (dl3.cptr() == foo2);
  dl3.toIdentifiedObject (sgkey, &store2);
  assert (dl3.cptr() == foo3);

  EXPECT_EXCEPTION (SG::ExcCLIDMismatch, 
                    {
                      TestStore::sgkey_t sgkey_x =
                        store.stringToKey ("bar", barclid);
                      dl3.toIdentifiedObject (sgkey_x, &store);
                    });

  Foo* foox = new Foo(50);
  store.record (foox, "");
  dl4.toDefaultObject();
  assert (dl4.cptr() == foox);

  DataLink<Foo> dl30;

  EXPECT_EXCEPTION (SG::ExcInvalidLink, *dl30);

  dl30.toIdentifiedObject ("foo30");
  EXPECT_EXCEPTION (SG::ExcInvalidLink, *dl30);

  Foo* foo30 = new Foo(30);
  store.record (foo30, "foo30");
  const Foo& rfoo30 = *dl30;
  assert (&rfoo30 == foo30);

  SG::DataProxyHolder holder;
  holder.toIdentifiedObject ("foo2", fooclid, 0);
  DataLink<Foo> l50 (sgkey, holder);
  assert (!l50.isDefault());
  assert (l50.dataID() == "foo2");
  assert (l50.key() == sgkey);
  assert (l50.cptr() == foo2);
  assert (l50.proxy()->name() == "foo2");
  assert (l50.source() == &store);
}


class DataLinkBase_test
{
public:
  static void setLink (DataLinkBase& l, DataLinkBase::sgkey_t key)
  {
    l.m_persKey = key;
  }
};


// toTransient, toPersistent
void test2 (SGTest::TestStore& store)
{
  std::cout << "test2\n";

  TestStore::sgkey_t sgkey = store.stringToKey ("foo5", fooclid);
  TestStore::sgkey_t sgkeyz = store.stringToKey ("fooz", fooclid);

  Foo* foo5 = new Foo(50);
  store.record (foo5, "foo5");
  Foo* fooz = new Foo(999);
  store.record (fooz, "fooz");
  DataLink<Foo> dl;
  DataLinkBase_test::setLink (dl, sgkey);
  dl.toTransient();
  assert (dl.cptr() == foo5);

  dl.clear();
  assert (dl.toTransient ("fooz"));
  assert (dl.dataID() == "fooz");
  assert (dl.key() == sgkeyz);
  assert (dl.proxy()->name() == "fooz");
  assert (dl.source() == &store);
  

  DataLink<Foo> dl2;
  dl2.toPersistent();
  assert (!dl2);
  dl2.toIdentifiedObject ("foo5");
  assert (dl2.cptr() == foo5);
  dl2.toPersistent();
  assert (dl2.cptr() == foo5);

  
  Foo* foo6 = new Foo(60);
  store.record (foo6, "foo6");
  Foo* foo7 = new Foo(70);
  store.record (foo7, "foo7");
  store.remap<Foo> ("foo6", "foo7");

  dl2.toIdentifiedObject ("foo6");
  assert (dl2.cptr() == foo6);
  dl2.toPersistent();
  assert (dl2.cptr() == foo7);
}


// default store setting
void test3 (SGTest::TestStore& store)
{
  std::cout << "test3\n";

  TestStore store2;

  assert (SG::CurrentEventStore::setStore(&store2) == &store);
  assert (SG::CurrentEventStore::setStore(&store) == &store2);
}


// dummy proxy creation.
void test4 (SGTest::TestStore& store)
{
  std::cout << "test4\n";

  DataLink<Foo> dl1;
  dl1.toIdentifiedObject ("foo20");
  assert (!dl1.isDefault());
  assert (dl1.dataID() == "foo20");
  assert (dl1.classID() == fooclid);
  assert (!dl1.isValid());

  TestStore::sgkey_t sgkey = store.stringToKey ("foo20", fooclid);
  assert (dl1.key() == sgkey);
  Foo* foo20 = new Foo(20);
  store.record (foo20, "foo20");
  assert (dl1.isValid());
  assert (dl1.cptr() == foo20);

  TestStore store2;
  TestStore::sgkey_t sgkey2 = store2.stringToKey ("foo21", fooclid);

  DataLink<Foo> dl2;
  dl2.toIdentifiedObject (sgkey2);
  assert (!dl2.isDefault());
  assert (dl2.key() == sgkey2);
  assert (dl2.dataID() == "");
  assert (dl2.classID() == fooclid);
  assert (!dl2.isValid());

  Foo* foo21 = new Foo(21);
  store.record (foo21, "foo21");
  assert (dl2.isValid());
  assert (dl2.cptr() == foo21);

  TestStore::sgkey_t sgkey3 = store.stringToKey ("foo22", fooclid);

  DataLink<Foo> dl3;
  dl3.toIdentifiedObject (sgkey3);
  assert (!dl3.isDefault());
  assert (dl3.key() == sgkey3);
  assert (dl3.dataID() == "foo22");
  assert (dl3.classID() == fooclid);
  assert (!dl3.isValid());

  Foo* foo22 = new Foo(22);
  store.record (foo22, "foo22");
  assert (dl3.isValid());
  assert (dl3.cptr() == foo22);

  TestStore::sgkey_t sgkey4 = store.stringToKey ("foo23", barclid);
  DataLink<Foo> dl4;
  EXPECT_EXCEPTION (SG::ExcCLIDMismatch, dl4.toIdentifiedObject (sgkey4));
}


// references to pointers not in SG.
void test5 (SGTest::TestStore& store)
{
  std::cout << "test5\n";

  Foo* foo1 = new Foo(101);
  DataLink<Foo> dl1 (foo1);
  assert (!dl1.isDefault());
  assert (dl1.isValid());
  assert (!!dl1);
  assert (dl1.cptr() == foo1);
  assert (dl1.getDataPtr() == foo1);
  assert (dl1.getDataNonConstPtr() == foo1);
  assert (dl1.key() == 0);
  assert (dl1.dataID() == "");

  EXPECT_EXCEPTION (SG::ExcPointerNotInSG, dl1.source());
  EXPECT_EXCEPTION (SG::ExcPointerNotInSG, dl1.toPersistent());

  store.record (foo1, "foo101");
  assert (dl1.dataID() == "foo101");
  assert (dl1.source() == &store);
  assert (dl1.key() == 0);
  dl1.toPersistent();

  TestStore::sgkey_t sgkey101 = store.stringToKey ("foo101", fooclid);
  assert (dl1.key() == sgkey101);

  Foo* foo2 = new Foo(102);
  dl1.toStorableObject (*foo2);
  assert (dl1.cptr() == foo2);
  assert (dl1.key() == 0);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  Athena::getMessageSvcQuiet = true;
  initTestStore();

  test1 (store);
  test2 (store);
  test3 (store);
  test4 (store);
  test5 (store);
  return 0;
}
