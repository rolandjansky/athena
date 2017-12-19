/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthLinks/test/GenericElementLinkBase_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2014
 * @brief Regression tests for GenericElementLinkBase
 */


#undef NDEBUG
#include "AthLinks/GenericElementLinkBase.h"
#include "AthLinks/exceptions.h"
#include "SGTools/CurrentEventStore.h"
#include "AthLinks/tools/MapIndexingPolicy.h"
#include "AthLinks/tools/SetIndexingPolicy.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/getMessageSvc.h"
#include "CxxUtils/checker_macros.h"
#include <map>
#include <iostream>
#include <cstdlib>
#include <cassert>


#include "SGTools/TestStore.h"
#include "TestTools/expect_exception.h"


using namespace SGTest;


typedef std::map<std::string, std::string> StrMap;
const unsigned int strmapclid = 23423559;
CLASS_DEF (StrMap, strmapclid, 1)


typedef std::set<std::string> StrSet;
const unsigned int strsetclid = 23423571;
CLASS_DEF (StrSet, strsetclid, 1)


struct Bar
  : public std::string
{
  Bar (std::string the_s = "") : std::string(the_s), s (the_s+"asd") {}
  std::string s;
};


// Ensure an offset between BarCont and base.
struct XXX { virtual ~XXX(){}  XXX():x(0){} int x; };

struct BarCont
  : public XXX, public StrSet
{
  typedef Bar key_type;
  typedef Bar value_type;
  typedef const Bar* const_pointer;
  typedef const Bar& const_reference;
  
  struct const_iterator
    : public StrSet::const_iterator
  {
    const_iterator (StrSet::const_iterator it)
      : StrSet::const_iterator (it) {}
    Bar operator*() const {
      const StrSet::const_iterator* baseit= static_cast<const StrSet::const_iterator*>(this);
      return Bar (**baseit);
    }
  };
  const_iterator begin() const { return const_iterator(StrSet::begin()); }
  const_iterator end() const { return const_iterator(StrSet::end()); }
};


const unsigned int barclid = 23423576;
CLASS_DEF (BarCont, barclid, 1)

SG_BASE(Bar, std::string);
SG_BASE(BarCont, StrSet);


struct StrTraits
{
  typedef StrSet Storable;
  typedef SG::SetIndexingPolicy<StrSet> IndexingPolicy;
};


struct BarTraits
{
  typedef BarCont Storable;
  typedef SG::SetIndexingPolicy<BarCont> IndexingPolicy;
};


template <class POLICY>
class ElementLinkBaseT_test
  : public SG::GenericElementLinkBase<POLICY>
{
public:
  typedef SG::GenericElementLinkBase<POLICY> Base;
  typedef typename Base::index_type index_type;

  ElementLinkBaseT_test() {}
  ElementLinkBaseT_test(const std::string& key, CLID clid,
                        const index_type& index,
                        IProxyDict* sg)
    : Base (key, clid, index, sg) {}
  ElementLinkBaseT_test(SG::sgkey_t key, CLID clid,
                        const index_type& index,
                        IProxyDict* sg)
    : Base (key, clid, index, sg) {}
  ElementLinkBaseT_test(const std::string& key, CLID clid,
                        const index_type& index,
                        const std::string& elt, IProxyDict* sg)
    : Base (key, clid, index, elt, sg) {}
  ElementLinkBaseT_test(SG::sgkey_t key, CLID clid,
                        const index_type& index,
                        const std::string& elt, IProxyDict* sg)
    : Base (key, clid, index, elt, sg) {}
  ElementLinkBaseT_test(const void* obj, CLID clid,
                        const index_type& index,
                        IProxyDict* sg)
    : Base (obj, clid, index, sg) {}
  ElementLinkBaseT_test(const ElementLinkBaseT_test& other,
                        const index_type& index)
    : Base (other, index) {}
  template <class OTHER_POLICY, class FROM_STORABLE, class TO_STORABLE>
  ElementLinkBaseT_test (const SG::GenericElementLinkBase<OTHER_POLICY>& other,
                         FROM_STORABLE* from, TO_STORABLE* to)
    : Base (other, from, to) {}
  using Base::getCachedElement;
  using Base::storableBase;
  using Base::storedIndex;
  using Base::toIndexedElement;
  using Base::setCachedElement;
  using Base::setIndex;
  using Base::setStorableObject;
  using Base::resetWithKeyAndIndex;
  using Base::proxyHolder;

  // Keep coverity happy.
  ElementLinkBaseT_test& operator=(const ElementLinkBaseT_test&) = default;

  void setLink (SG::sgkey_t key,
                const std::string& index)
  {
    this->m_key = key;
    this->m_index = index;
  }
};


typedef ElementLinkBaseT_test<SG::MapIndexingPolicy<StrMap> >
  ElementLinkBase_test;


void test1 (SGTest::TestStore& store)
{
  std::cout << "test1\n";

  typedef ElementLinkBase_test Link;
  CLID clid = strmapclid;
  typedef std::string element_t;
  typedef const element_t* element_const_pointer;
  element_const_pointer  eltp = 0;

  StrMap* cont = new StrMap;
  (*cont)["0"] = "10";
  (*cont)["1"] = "11";
  (*cont)["2"] = "12";
  (*cont)["3"] = "13";
  std::string key = "strcont";
  TestStore::sgkey_t sgkey = store.stringToKey (key, clid);
  store.record (cont, key);
  Link::index_type index1 = "1";
  Link::index_type index2 = "2";
  std::string elt2 = (*cont)[index2];

  Link::index_type null_index;

  StrMap* cont2 = new StrMap;
  (*cont2)["0b"] = "110";
  (*cont2)["1b"] = "111";
  (*cont2)["2b"] = "112";
  (*cont2)["3b"] = "113";
  std::string key2 = "strcont2";
  store.record (cont2, key2);
  Link::index_type index1b = "1b";
  Link::index_type index2b = "2bj";
  TestStore::sgkey_t sgkey2 = store.stringToKey (key2, clid);

  Link el1;
  assert (el1.isDefault());
  assert (el1.index() == null_index);
  assert (el1.storableBase (0, clid) == 0);
  assert (el1.dataID() == "");
  assert (el1.key() == 0);
  assert (el1.source() == 0);
  eltp = 0;
  assert (!el1.getCachedElement(eltp));
  assert (eltp == 0);
  assert (el1.proxy() == 0);
  assert (!el1.hasCachedElement());
  assert (!el1.storedIndex().isValid());

  Link el2 (key, clid, index2, 0);
  eltp = 0;
  assert (!el2.getCachedElement(eltp));
  assert (eltp == 0);
  assert (!el2.isDefault());
  assert (el2.index() == index2);
  assert (el2.dataID() == key);
  assert (el2.key() == sgkey);
  assert (el2.source() == &store);
  assert (el2.proxy()->name() == key);
  assert (el2.proxy() == el2.proxyHolder().proxy());
  assert (!el2.hasCachedElement());
  assert (el2.storedIndex().isValid());
  assert (el2.storableBase (0, clid) == cont);

  Link el3 (sgkey, clid, index2, 0);
  eltp = 0;
  assert (!el3.getCachedElement(eltp));
  assert (eltp == 0);
  assert (!el3.isDefault());
  assert (el3.index() == index2);
  assert (el3.dataID() == key);
  assert (el3.key() == sgkey);
  assert (el3.source() == &store);
  assert (!el3.hasCachedElement());
  assert (el3.storableBase (0, clid) == cont);

  Link el4 (key, clid, index2, elt2, 0);
  eltp = 0;
  assert (el4.getCachedElement(eltp));
  assert (*eltp == elt2);
  assert (!el4.isDefault());
  assert (el4.index() == index2);
  assert (el4.dataID() == key);
  assert (el4.key() == sgkey);
  assert (el4.source() == &store);
  assert (el4.hasCachedElement());
  assert (el4.storableBase (0, clid) == cont);

  Link el5 (sgkey, clid, index2, elt2, 0);
  eltp = 0;
  assert (el5.getCachedElement(eltp));
  assert (*eltp == elt2);
  assert (!el5.isDefault());
  assert (el5.index() == index2);
  assert (el5.dataID() == key);
  assert (el5.key() == sgkey);
  assert (el5.source() == &store);
  assert (el5.storableBase (0, clid) == cont);
  assert (el5.hasCachedElement());

  Link el6 (cont2, clid, index2b, 0);
  assert (!el6.isDefault());
  assert (el6.index() == index2b);
  assert (el6.dataID() == key2);
  assert (el6.key() == sgkey2);
  assert (el6.source() == &store);
  assert (el6.storableBase (0, clid) == cont2);
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
  assert (el7.storableBase (0, clid) == cont2);
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
  assert (el1.storableBase (0, clid) == cont2);
  eltp = 0;
  assert (!el1.getCachedElement(eltp));
  assert (eltp == 0);
  assert (!el1.hasCachedElement());

  Link el8;
  assert (el8.isDefault());
  assert (el8.toIndexedElement (cont, clid, index1, 0));
  assert (!el8.isDefault());
  assert (el8.index() == index1);
  assert (el8.dataID() == key);
  assert (el8.key() == sgkey);
  assert (el8.source() == &store);
  assert (el8.storableBase (0, clid) == cont);
  assert (!el8.toIndexedElement (cont, clid, index1, 0));
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
  assert (!el8.toIndexedElement (cont, clid, index1, 0));
  assert (el8.isDefaultIndex());
  assert ((std::string)(el8.storedIndex()) == null_index);
  el8.setIndex ("3");
  assert (!el8.isDefaultIndex());
  assert ((std::string)(el8.storedIndex()) == "3");
  assert (!el8.toIndexedElement (cont, clid, index1, 0));

  Link el11;
  el11.setIndex (index2);
  assert (el11.setStorableObject (cont, clid, false, 0));
  assert (!el11.isDefault());
  assert (el11.index() == index2);
  assert (el11.dataID() == key);
  assert (el11.key() == sgkey);
  assert (el11.source() == &store);
  assert (el11.storableBase (0, clid) == cont);
  assert (!el11.hasCachedElement());
  assert (!el11.setStorableObject (cont2, clid, false, 0));
  assert (el11.setStorableObject (cont2, clid, true, 0));
  assert (el11.index() == null_index);
  assert (el11.storableBase (0, clid) == cont2);
  assert (el11.dataID() == key2);

  Link el12;
  el12.setCachedElement (elt2);
  assert (el12.hasCachedElement());
  el12.resetWithKeyAndIndex (key2, clid, index1b, 0);
  assert (!el12.hasCachedElement());
  assert (!el12.isDefault());
  assert (el12.index() == index1b);
  assert (el12.dataID() == key2);
  assert (el12.key() == sgkey2);
  assert (el12.source() == &store);
  assert (el12.storableBase (0, clid) == cont2);

  el12.setCachedElement (elt2);
  assert (el12.hasCachedElement());
  el12.resetWithKeyAndIndex (sgkey, clid, index2, 0);
  assert (!el12.hasCachedElement());
  assert (!el12.isDefault());
  assert (el12.index() == index2);
  assert (el12.dataID() == key);
  assert (el12.key() == sgkey);
  assert (el12.source() == &store);
  assert (el12.storableBase (0, clid) == cont);

  Link el13 (el12, index1);
  assert (!el13.hasCachedElement());
  assert (!el13.isDefault());
  assert (el13.index() == index1);
  assert (el13.dataID() == key);
  assert (el13.key() == sgkey);
  assert (el13.source() == &store);
  assert (el13.storableBase (0, clid) == cont);

  el12.reset();
  assert (el12.isDefault());
  assert (el12.index() == null_index);
  assert (el12.dataID() == "");
  assert (el12.key() == 0);
  assert (el12.source() == 0);
  assert (el12.storableBase (0, clid) == 0);
  assert (!el12.hasCachedElement());
}


// toTransient, toPersistent
void test2 (SGTest::TestStore& store)
{
  std::cout << "test2\n";

  typedef ElementLinkBase_test Link;
  CLID clid = strmapclid;

  StrMap* cont3 = new StrMap;
  (*cont3)["200"] = "300";
  (*cont3)["201"] = "301";
  (*cont3)["202"] = "302";
  (*cont3)["203"] = "303";
  std::string key3 = "strcont3";
  store.record (cont3, key3);
  TestStore::sgkey_t sgkey3 = store.stringToKey (key3, clid);
  Link::index_type index1 = "201";
  Link::index_type index2 = "202";

  Link el1;
  assert (el1.proxy() == 0);
  el1.setLink (sgkey3, "202");
  assert (el1.toTransient());
  assert (!el1.hasCachedElement());
  assert (el1.proxy()->name() == key3);

  Link el2;
  assert (el2.toPersistent());
  assert (el2.isDefault());
  el2 = Link (key3, clid, index1, 0);
  assert (el2.storableBase (0, clid) == cont3);
  assert (el2.index() == index1);
  assert (el2.key() == sgkey3);
  assert (el2.toPersistent());
  assert (el2.storableBase (0, clid) == cont3);
  assert (el2.index() == index1);
  assert (el2.key() == sgkey3);

  el2.reset();
  el2.setIndex (index2);
  assert (!el2.toPersistent());
  el2.reset();
  el2.setStorableObject (cont3, clid, true, 0);
  assert (!el2.toPersistent());
  el2.setIndex (index2);
  assert (el2.toPersistent());

  StrMap* cont4 = new StrMap;
  el2.setStorableObject (cont4, clid, true, 0);
  el2.setIndex (index2);
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


// Converting constructor
void test4 (SGTest::TestStore& store)
{
  std::cout << "test4\n";

  BarCont* bar = new BarCont;
  bar->insert ("1");
  bar->insert ("2");
  bar->insert ("3");

  typedef ElementLinkBaseT_test<SG::SetIndexingPolicy<StrSet> >
    BaseLinkBase;
  typedef ElementLinkBaseT_test<SG::SetIndexingPolicy<BarCont> >
    DerLinkBase;

  // Reference to raw element.
  DerLinkBase el1;
  el1.setCachedElement (Bar ("bar1"));
  BaseLinkBase el2 (el1, (BarTraits*)0, (StrTraits*)0);

  const Bar* derp1 = 0;
  const std::string* basep1 = 0;
  assert (el1.getCachedElement (derp1));
  assert (el2.getCachedElement (basep1));
  assert (*derp1 == "bar1");
  assert (derp1->s == "bar1asd");
  assert (*basep1 == "bar1");

  // Storable not in SG.
  DerLinkBase el3 (bar, barclid, Bar("2"), nullptr);
  BaseLinkBase el4 (el3, (BarTraits*)0, (StrTraits*)0);
  BarCont* barcont1 = reinterpret_cast<BarCont*>(el3.storableBase (0, barclid));
  StrSet* strcont1 = reinterpret_cast<StrSet*>(el4.storableBase (0, strsetclid));
  assert (barcont1 == bar);
  assert (barcont1 == strcont1);
  assert (static_cast<void*>(barcont1) != static_cast<void*>(strcont1));
  assert (el3.index() == Bar("2"));
  assert (el4.index() == "2");
  assert (el3.key() == 0);
  assert (el4.key() == 0);

  // Storable in SG.
  std::string key = "barcont";
  store.record (bar, key);
  DerLinkBase el5 (key, barclid, Bar("2"), nullptr);
  BaseLinkBase el6 (el5, (BarTraits*)0, (StrTraits*)0);
  barcont1 = reinterpret_cast<BarCont*>(el5.storableBase (0, barclid));
  strcont1 = reinterpret_cast<StrSet*>(el6.storableBase (0, strsetclid));
  assert (barcont1 == bar);
  assert (barcont1 == strcont1);
  assert (static_cast<void*>(barcont1) != static_cast<void*>(strcont1));
  assert (el5.index() == Bar("2"));
  assert (el6.index() == "2");
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
  return 0;
}
