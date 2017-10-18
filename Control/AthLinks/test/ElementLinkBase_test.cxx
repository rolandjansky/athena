/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthLinks/test/ElementLinkBase_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2014
 * @brief Regression tests for ElementLinkBase
 */


#undef NDEBUG
#include "AthLinks/ElementLinkBase.h"
#include "AthLinks/exceptions.h"
#include "AthLinks/tools/ForwardIndexingPolicy.h"
#include "SGTools/CurrentEventStore.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/getMessageSvc.h" 
#include "CxxUtils/checker_macros.h"
#include <vector>
#include <iostream>
#include <cstdlib>
#include <cassert>


#include "SGTools/TestStore.h"
#include "TestTools/expect_exception.h"
#include "TestThinningSvc.icc"


using namespace SGTest;


struct Foo
{
  Foo (int the_x = 0) : x (the_x) {}
  virtual ~Foo() {}
  int x;

  bool operator== (const Foo& other) const
  { return x == other.x; }
  bool operator!= (const Foo& other) const
  { return x != other.x; }
};

typedef std::vector<Foo*> FooCont;

const unsigned int fooclid = 23423556;
CLASS_DEF (FooCont, fooclid, 1)
CLASS_DEF (std::vector<Foo>, 23423558, 1)

// Ensure an offset between Bar and Foo base.
struct XXX { virtual ~XXX(){}  XXX():x(0){} int x; };

struct Bar
  : public XXX, public Foo
{
  Bar (int the_y = 0) : Foo(the_y+10), y (the_y) {}
  int y;
};

struct BarCont
  : public XXX, public FooCont
{
  typedef Bar* value_type;
  struct const_iterator
    : public FooCont::const_iterator
  {
    const_iterator (FooCont::const_iterator it)
      : FooCont::const_iterator (it) {}
    Bar* operator*() const {
      const FooCont::const_iterator* baseit= static_cast<const FooCont::const_iterator*>(this);
      return static_cast<Bar*> (**baseit);
    }
  };
  const_iterator begin() const { return const_iterator(FooCont::begin()); }
  const_iterator end() const { return const_iterator(FooCont::end()); }
  Bar* operator[] (size_t i) const
  {
    const FooCont* base = static_cast<const FooCont*> (this);
    return static_cast<Bar*> ((*base)[i]);
  }
};


const unsigned int barclid = 23423566;
CLASS_DEF (BarCont, barclid, 1)
CLASS_DEF (std::vector<Bar>, 23423568, 1)

SG_BASE(Bar, Foo);
SG_BASE(BarCont, FooCont);


struct FooTraits
{
  typedef FooCont Storable;
  typedef SG::ForwardIndexingPolicy<FooCont> IndexingPolicy;
};


struct BarTraits
{
  typedef BarCont Storable;
  typedef SG::ForwardIndexingPolicy<BarCont> IndexingPolicy;
};


class ElementLinkBase_test
  : public ElementLinkBase
{
public:
  ElementLinkBase_test() {}
  ElementLinkBase_test(const std::string& key, CLID clid, unsigned index,
                       IProxyDict* sg)
    : ElementLinkBase (key, clid, index, sg) {}
  ElementLinkBase_test(SG::sgkey_t key, CLID clid, unsigned index,
                       IProxyDict* sg)
    : ElementLinkBase (key, clid, index, sg) {}
  ElementLinkBase_test(const std::string& key, CLID clid, unsigned index,
                       const void* elt, IProxyDict* sg)
    : ElementLinkBase (key, clid, index, elt, sg) {}
  ElementLinkBase_test(SG::sgkey_t key, CLID clid, unsigned index,
                       const void* elt, IProxyDict* sg)
    : ElementLinkBase (key, clid, index, elt, sg) {}
  ElementLinkBase_test(const void* obj, CLID clid, index_type index,
                       IProxyDict* sg)
    : ElementLinkBase (obj, clid, index, sg) {}
  ElementLinkBase_test(const ElementLinkBase& other, unsigned index)
    : ElementLinkBase (other, index) {}
  template <class FROM_STORABLE, class TO_STORABLE>
  ElementLinkBase_test (const ElementLinkBase& other,
                        FROM_STORABLE* from, TO_STORABLE* to)
    : ElementLinkBase (other, from, to) {}
  using ElementLinkBase::getCachedElement;
  using ElementLinkBase::storableBase;
  using ElementLinkBase::storedIndex;
  using ElementLinkBase::toIndexedElement;
  using ElementLinkBase::setCachedElement;
  using ElementLinkBase::storeCachedElement;
  using ElementLinkBase::setIndex;
  using ElementLinkBase::setStorableObject;
  using ElementLinkBase::resetWithKeyAndIndex;
  using ElementLinkBase::proxyHolder;

  void setLink (SG::sgkey_t key,
                unsigned int index)
  {
    m_persKey = key;
    m_persIndex = index;
  }
};


void test1 (SGTest::TestStore& store)
{
  std::cout << "test1\n";

  typedef ElementLinkBase_test Link;
  typedef const Foo* element_t;
  typedef const element_t* element_const_pointer;
  element_const_pointer  eltp = 0;

  FooCont* foocont = new FooCont;
  foocont->push_back (new Foo(0));
  foocont->push_back (new Foo(1));
  foocont->push_back (new Foo(2));
  foocont->push_back (new Foo(3));
  const Foo* elt2 = (*foocont)[2];
  std::string key = "foocont";
  store.record (foocont, key);
  Link::index_type index1 = 1;
  Link::index_type index2 = 2;
  TestStore::sgkey_t sgkey = store.stringToKey (key, fooclid);

  Link::index_type null_index = static_cast<Link::index_type>(-1);

  FooCont* foocont2 = new FooCont;
  foocont2->push_back (new Foo(11));
  foocont2->push_back (new Foo(12));
  foocont2->push_back (new Foo(13));
  foocont2->push_back (new Foo(14));
  std::string key2 = "foocont2";
  store.record (foocont2, key2);
  Link::index_type index1b = 1;
  Link::index_type index2b = 2;
  TestStore::sgkey_t sgkey2 = store.stringToKey (key2, fooclid);

  Link el1;
  assert (el1.isDefault());
  assert (el1.index() == null_index);
  assert (el1.persIndex() == static_cast<Link::stored_index_type>(null_index));
  assert (el1.storableBase (0, fooclid) == 0);
  assert (el1.dataID() == "");
  assert (el1.key() == 0);
  assert (el1.persKey() == 0);
  assert (el1.source() == 0);
  eltp = 0;
  assert (!el1.getCachedElement(eltp));
  assert (eltp == 0);
  assert (el1.proxy() == 0);
  assert (!el1.hasCachedElement());
  assert (el1.storedIndex() == static_cast<Link::stored_index_type>(null_index));

  Link el2 (key, fooclid, index2, 0);
  eltp = 0;
  assert (!el2.getCachedElement(eltp));
  assert (eltp == 0);
  assert (!el2.isDefault());
  assert (el2.index() == index2);
  assert (el2.persIndex() == index2);
  assert (el2.dataID() == key);
  assert (el2.key() == sgkey);
  assert (el2.persKey() == sgkey);
  assert (el2.source() == &store);
  assert (el2.proxy()->name() == key);
  assert (el2.proxy() == el2.proxyHolder().proxy());
  assert (!el2.hasCachedElement());
  assert (el2.storedIndex() == index2);
  assert (el2.storableBase (0, fooclid) == foocont);

  Link el3 (sgkey, fooclid, index2, 0);
  eltp = 0;
  assert (!el3.getCachedElement(eltp));
  assert (eltp == 0);
  assert (!el3.isDefault());
  assert (el3.index() == index2);
  assert (el3.dataID() == key);
  assert (el3.key() == sgkey);
  assert (el3.source() == &store);
  assert (!el3.hasCachedElement());
  assert (el3.storableBase (0, fooclid) == foocont);

  Link el4 (key, fooclid, index2, elt2, 0);
  eltp = 0;
  assert (el4.getCachedElement(eltp));
  assert (*eltp == elt2);
  assert (!el4.isDefault());
  assert (el4.index() == index2);
  assert (el4.dataID() == key);
  assert (el4.key() == sgkey);
  assert (el4.source() == &store);
  assert (el4.hasCachedElement());
  assert (el4.storableBase (0, fooclid) == foocont);

  Link el5 (sgkey, fooclid, index2, elt2, 0);
  eltp = 0;
  assert (el5.getCachedElement(eltp));
  assert (*eltp == elt2);
  assert (!el5.isDefault());
  assert (el5.index() == index2);
  assert (el5.dataID() == key);
  assert (el5.key() == sgkey);
  assert (el5.source() == &store);
  assert (el5.storableBase (0, fooclid) == foocont);
  assert (el5.hasCachedElement());

  Link el6 (foocont2, fooclid, index2b, 0);
  assert (!el6.isDefault());
  assert (el6.index() == index2b);
  assert (el6.dataID() == key2);
  assert (el6.key() == sgkey2);
  assert (el6.source() == &store);
  assert (el6.storableBase (0, fooclid) == foocont2);
  eltp = 0;
  assert (!el6.getCachedElement(eltp));
  assert (eltp == 0);
  assert (!el6.hasCachedElement());

  Link el7 (el6);
  assert (!el7.isDefault());
  assert (el7.index() == index2b);
  assert (el7.dataID() == key2);
  assert (el7.key() == sgkey2);
  assert (el7.source() == &store);
  assert (el7.storableBase (0, fooclid) == foocont2);
  eltp = 0;
  assert (!el7.getCachedElement(eltp));
  assert (eltp == 0);
  assert (!el7.hasCachedElement());

  el1 = el7;
  assert (!el1.isDefault());
  assert (el1.index() == index2b);
  assert (el1.dataID() == key2);
  assert (el1.key() == sgkey2);
  assert (el1.source() == &store);
  assert (el1.storableBase (0, fooclid) == foocont2);
  eltp = 0;
  assert (!el1.getCachedElement(eltp));
  assert (eltp == 0);
  assert (!el1.hasCachedElement());

  Link el8;
  assert (el8.isDefault());
  assert (el8.toIndexedElement (foocont, fooclid, index1, 0));
  assert (!el8.isDefault());
  assert (el8.index() == index1);
  assert (el8.dataID() == key);
  assert (el8.key() == sgkey);
  assert (el8.source() == &store);
  assert (el8.storableBase (0, fooclid) == foocont);
  assert (!el8.toIndexedElement (foocont, fooclid, index1, 0));
  el8.reset();
  assert (el8.isDefault());
  assert (!el8.hasCachedElement());
  eltp = 0;
  assert (!el8.getCachedElement(eltp));
  assert (eltp == 0);
  el8.setCachedElement (elt2);
  assert (el8.hasCachedElement());
  eltp = 0;
  assert (el8.getCachedElement(eltp));
  assert (*eltp == elt2);
  assert (!el8.toIndexedElement (foocont, fooclid, index1, 0));
  assert (el8.isDefaultIndex());
  assert (el8.storedIndex() == static_cast<Link::stored_index_type>(null_index));
  el8.setIndex (3);
  assert (!el8.isDefaultIndex());
  assert (el8.storedIndex() == 3);
  assert (!el8.toIndexedElement (foocont, fooclid, index1, 0));

  Link el11;
  el11.setIndex (index2);
  assert (el11.setStorableObject (foocont, fooclid, false, 0));
  assert (!el11.isDefault());
  assert (el11.index() == index2);
  assert (el11.dataID() == key);
  assert (el11.key() == sgkey);
  assert (el11.source() == &store);
  assert (el11.storableBase (0, fooclid) == foocont);
  assert (!el11.hasCachedElement());
  assert (!el11.setStorableObject (foocont2, fooclid, false, 0));
  assert (el11.setStorableObject (foocont2, fooclid, true, 0));
  assert (el11.index() == null_index);
  assert (el11.storableBase (0, fooclid) == foocont2);
  assert (el11.dataID() == key2);

  Link el12;
  el12.setCachedElement (elt2);
  assert (el12.hasCachedElement());
  el12.resetWithKeyAndIndex (key2, fooclid, index1b, 0);
  assert (!el12.hasCachedElement());
  assert (!el12.isDefault());
  assert (el12.index() == index1b);
  assert (el12.dataID() == key2);
  assert (el12.key() == sgkey2);
  assert (el12.source() == &store);
  assert (el12.storableBase (0, fooclid) == foocont2);

  el12.storeCachedElement (elt2);
  assert (el12.hasCachedElement());
  el12.resetWithKeyAndIndex (sgkey, fooclid, index2, 0);
  assert (!el12.hasCachedElement());
  assert (!el12.isDefault());
  assert (el12.index() == index2);
  assert (el12.dataID() == key);
  assert (el12.key() == sgkey);
  assert (el12.source() == &store);
  assert (el12.storableBase (0, fooclid) == foocont);

  Link el13 (el12, index1);
  assert (!el13.hasCachedElement());
  assert (!el13.isDefault());
  assert (el13.index() == index1);
  assert (el13.dataID() == key);
  assert (el13.key() == sgkey);
  assert (el13.source() == &store);
  assert (el13.storableBase (0, fooclid) == foocont);

  el12.reset();
  assert (el12.isDefault());
  assert (el12.index() == null_index);
  assert (el12.dataID() == "");
  assert (el12.key() == 0);
  assert (el12.source() == 0);
  assert (el12.storableBase (0, fooclid) == 0);
  assert (!el12.hasCachedElement());
}


// toTransient, toPersistent
void test2 (SGTest::TestStore& store)
{
  std::cout << "test2\n";

  TestStore::sgkey_t sgkey = store.stringToKey ("foocont3", fooclid);

  FooCont* foocont3 = new FooCont;
  for (int i=0; i < 4; i++)
    foocont3->push_back (new Foo(i+200));
  std::string key3 = "foocont3";
  store.record (foocont3, key3);
  TestStore::sgkey_t sgkey3 = store.stringToKey (key3, fooclid);

  typedef ElementLinkBase_test Link;
  Link el1;
  assert (el1.proxy() == 0);
  el1.setLink (sgkey, 2);
  assert (el1.toTransient());
  assert (!el1.hasCachedElement());
  assert (el1.proxy()->name() == key3);

  Link el2;
  assert (el2.toPersistent());
  assert (el2.isDefault());
  el2 = Link (key3, fooclid, 1, 0);
  assert (el2.storableBase (0, fooclid) == foocont3);
  assert (el2.index() == 1);
  assert (el2.key() == sgkey3);
  assert (el2.toPersistent());
  assert (el2.storableBase (0, fooclid) == foocont3);
  assert (el2.index() == 1);
  assert (el2.key() == sgkey3);

  FooCont* foocont3a = new FooCont;
  for (int i=0; i < 4; i++)
    foocont3a->push_back (new Foo(i+300));
  std::string key3a = "foocont3a";
  TestStore::sgkey_t sgkey3a = store.stringToKey (key3a, fooclid);
  store.record (foocont3a, key3a);

  FooCont* foocont3b = new FooCont;
  for (int i=0; i < 4; i++)
    foocont3b->push_back (new Foo(i+350));
  std::string key3b = "foocont3b";
  TestStore::sgkey_t sgkey3b = store.stringToKey (key3b, fooclid);
  store.record (foocont3b, key3b);

  store.remap<FooCont> (key3a, key3b, 2, 1);

  el2 = Link (key3a, fooclid, 2, 0);
  assert (el2.index() == 2);
  assert (el2.key() == sgkey3a);
  assert (el2.toPersistent());
  assert (el2.index() == 1);
  assert (el2.key() == sgkey3b);

  el2.reset();
  el2.setIndex (2);
  assert (!el2.toPersistent());
  el2.reset();
  el2.setStorableObject (foocont3, fooclid, true, 0);
  assert (!el2.toPersistent());
  el2.setIndex (2);
  assert (el2.toPersistent());

  FooCont* foocont4 = new FooCont;
  el2.setStorableObject (foocont4, fooclid, true, 0);
  el2.setIndex (2);
  EXPECT_EXCEPTION (SG::ExcPointerNotInSG, el2.toPersistent());
}


// default store setting
void test3 (SGTest::TestStore& store)
{
  std::cout << "test3\n";
  TestStore store2;

  assert (SG::CurrentEventStore::setStore(&store2) == &store);
  assert (SG::CurrentEventStore::setStore(&store) == &store2);
}


// thinning
void test4 (SGTest::TestStore& store)
{
  std::cout << "test4\n";

  TestThinningSvc svc;
  TestThinningSvc::instance (&svc, true);
  SG::DataProxyHolder h1;
  TestStore::sgkey_t sgkey_foo = store.stringToKey ("foo", fooclid);
  TestStore::sgkey_t sgkey = sgkey_foo;
  size_t index = 10;
  h1.toTransient (sgkey);

  assert (h1.thin (sgkey, index) == false);
  assert (sgkey == sgkey_foo);
  assert (index == 10);

  svc.remap (10, 12);
  assert (h1.thin (sgkey, index) == true);
  assert (sgkey == sgkey_foo);
  assert (index == 12);
}


// Converting constructor
void test5 (SGTest::TestStore& store)
{
  std::cout << "test5\n";

  BarCont* bar = new BarCont;
  bar->push_back (new Bar (1));
  bar->push_back (new Bar (2));
  bar->push_back (new Bar (3));

  // Reference to raw element.
  ElementLinkBase_test el1;
  el1.setCachedElement ((*bar)[1]);
  ElementLinkBase_test el2 (el1, (BarTraits*)0, (FooTraits*)0);

  typedef const Foo* foo_element_t;
  typedef const Bar* bar_element_t;
  typedef const foo_element_t* foo_element_const_pointer;
  typedef const bar_element_t* bar_element_const_pointer;

  foo_element_const_pointer foop1 = 0;
  bar_element_const_pointer barp1 = 0;
  assert (el1.getCachedElement (barp1));
  assert (el2.getCachedElement (foop1));
  assert (*barp1 == (*bar)[1]);
  assert (*barp1 == *foop1);
  assert (static_cast<const void*>(*barp1) != static_cast<const void*>(*foop1));

  // Storable not in SG.
  ElementLinkBase_test el3 (bar, barclid, 1, nullptr);
  ElementLinkBase_test el4 (el3, (BarTraits*)0, (FooTraits*)0);
  BarCont* barcont1 = reinterpret_cast<BarCont*>(el3.storableBase (0, barclid));
  FooCont* foocont1 = reinterpret_cast<FooCont*>(el4.storableBase (0, fooclid));
  assert (barcont1 == bar);
  assert (barcont1 == foocont1);
  assert (static_cast<void*>(barcont1) != static_cast<void*>(foocont1));
  assert (el3.index() == 1);
  assert (el4.index() == 1);
  assert (el3.key() == 0);
  assert (el4.key() == 0);

  // Storable in SG.
  std::string key = "barcont";
  store.record (bar, key);
  ElementLinkBase_test el5 (key, barclid, 1, nullptr);
  ElementLinkBase_test el6 (el5, (BarTraits*)0, (FooTraits*)0);
  barcont1 = reinterpret_cast<BarCont*>(el5.storableBase (0, barclid));
  foocont1 = reinterpret_cast<FooCont*>(el6.storableBase (0, fooclid));
  assert (barcont1 == bar);
  assert (barcont1 == foocont1);
  assert (static_cast<void*>(barcont1) != static_cast<void*>(foocont1));
  assert (el5.index() == 1);
  assert (el6.index() == 1);
  assert (el5.dataID() == key);
  assert (el6.dataID() == key);
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
