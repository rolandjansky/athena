/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG
#include "AthLinks/ElementLink.h"
#include "AthLinks/ElementLinkVector.h"
#include "AthLinks/DeclareIndexingPolicy.h"
#include "AthLinks/exceptions.h"
#include "AthLinks/tools/SetIndexingPolicy.h"
#include "AthLinks/tools/MapIndexingPolicy.h"
#include "AthLinks/tools/IdentContIndexingPolicy.h"
#include "AthenaKernel/getMessageSvc.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "SGTools/CurrentEventStore.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/EventContext.h"
#include <vector>
#include <set>
#include <iostream>
#include <cstdlib>
#include <cassert>


#include "SGTools/TestStore.h"
#include "TestTools/expect_exception.h"


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


typedef std::vector<std::string> StrVec;
const unsigned int strclid = 23423557;
CLASS_DEF (StrVec, strclid, 1)

typedef std::set<std::string> StrSet;
const unsigned int strsetclid = 23423558;
CLASS_DEF (StrSet, strsetclid, 1)

typedef std::map<std::string, int> StrMap;
const unsigned int strmapclid = 23423559;
CLASS_DEF (StrMap, strmapclid, 1)


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
  typedef std::true_type isSequence;
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


struct Baz
  : public std::string
{
  Baz (std::string the_s = "") : std::string(the_s), s (the_s+"asd") {}
  std::string s;
};


struct BazCont
  : public XXX, public StrSet
{
  typedef Baz key_type;
  typedef Baz value_type;
  typedef const Baz* const_pointer;
  typedef const Baz& const_reference;
  
  struct const_iterator
    : public StrSet::const_iterator
  {
    const_iterator (StrSet::const_iterator it)
      : StrSet::const_iterator (it) {}
    Baz operator*() const {
      const StrSet::const_iterator* baseit= static_cast<const StrSet::const_iterator*>(this);
      return Baz (**baseit);
    }
  };
  const_iterator begin() const { return const_iterator(StrSet::begin()); }
  const_iterator end() const { return const_iterator(StrSet::end()); }
};


CONTAINER_IS_SET(BazCont)
const unsigned int bazclid = 23423576;
CLASS_DEF (BazCont, bazclid, 1)

SG_BASE(Baz, std::string);
SG_BASE(BazCont, StrSet);




struct IdentTest
{
  struct IDENTIFIABLE
    : public std::vector<std::string>
  {
    unsigned short identifyHash() const { return m_hash; }
    unsigned short m_hash;
  };
  typedef std::vector<IDENTIFIABLE*> vec_t;
  typedef vec_t::const_iterator const_iterator;
  typedef std::string value_type;

  const_iterator indexFind(unsigned short ndx) const
  {
    if (ndx < m_vec.size())
      return m_vec.begin() + ndx;
    return end();
  }

  const_iterator begin() const
  {
    return m_vec.begin();
  }
  const_iterator end() const
  {
    return m_vec.end();
  }

  void fill (std::string prefix);
  
  vec_t m_vec;
};


CLASS_DEF (IdentTest, 23423560, 1)

void IdentTest::fill (std::string prefix)
{
  for (int i=0; i < 4; i++) {
    IDENTIFIABLE* v = new IDENTIFIABLE;
    for (int j=0; j<4; j++) {
      std::ostringstream os;
      os << prefix << (i*4+j);
      v->push_back (os.str());
    }
    v->m_hash = i;
    m_vec.push_back (v);
  }
}


template <>
struct DefaultIndexingPolicy<IdentTest> {
  typedef SG::IdentContIndexingPolicy<IdentTest> type;
};


class ElementLinkBase_test
{
public:
  static void setLink (ElementLinkBase& l,
                       SG::sgkey_t key,
                       unsigned int index)
  {
    l.m_persKey = key;
    l.m_persIndex = index;
  }
};


template <class CONT>
struct Index
{
  typedef typename CONT::value_type type;
  Index(const CONT* cont) : m_cont(*cont) {}
  const type& operator[] (size_t idx) const
  { return m_cont[idx]; }
  const CONT& m_cont;
};


template <class T>
struct Index<std::set<T> >
{
  typedef std::set<T> CONT;
  typedef typename CONT::value_type type;
  Index(const CONT*)  {}
  const type& operator[] (const type& idx) const { return idx; }
};


template <class K, class V>
struct Index<std::map<K, V> >
{
  typedef std::map<K, V> CONT;
  typedef V type;
  Index(const CONT* cont) : m_cont(*cont) {}
  const type& operator[] (const K& idx) const
  {
    typename CONT::const_iterator it = m_cont.find (idx);
    if (it == m_cont.end()) std::abort();
    return it->second;
  }
  const CONT& m_cont;
};


template <>
struct Index<IdentTest>
{
  typedef IdentTest CONT;
  typedef std::string type;
  Index(const CONT* cont) : m_cont(*cont) {}
  const type& operator[] (unsigned idx) const
  {
    typename CONT::const_iterator it = m_cont.indexFind ((idx>>16)&0xffff);
    if (it == m_cont.end()) std::abort();
    return (**it)[idx&0xffff];
  }
  const CONT& m_cont;
};


template <class CONT, class IDX>
const typename Index<CONT>::type& doindex (const CONT* cont, const IDX& idx)
{
  return Index<CONT>(cont)[idx];
}



template <class CONT>
void testit (SGTest::TestStore& store,
             const std::string& key,
             const CONT* cont,
             const std::string& key2,
             const CONT* cont2,
             const typename ElementLink<CONT>::index_type& index1,
             const typename ElementLink<CONT>::index_type& index1b,
             const typename ElementLink<CONT>::index_type& index2,
             const typename ElementLink<CONT>::index_type& index2b,
             const typename ElementLink<CONT>::index_type& null_index)
{
  typedef ElementLink<CONT> Link;
  typedef typename Link::ElementType element_t;
  element_t null_element = element_t();
  CLID clid = ClassID_traits<CONT>::ID();
  TestStore::sgkey_t sgkey = store.stringToKey (key, clid);
  TestStore::sgkey_t sgkey2 = store.stringToKey (key2, clid);

  Link el1;
  assert (el1.isDefault());
  assert (el1.index() == null_index);
  assert (!el1.isValid());
  assert (!el1);
  assert (el1.cptr() == 0);
  assert (el1.getDataPtr() == 0);
  assert (el1.getDataNonConstPtr() == 0);
  assert (el1.getStorableObjectPointer() == 0);
  assert (el1.dataID() == "");
  assert (el1.key() == 0);
  assert (el1.source() == 0);
  assert (el1.cachedElement() == null_element);
  assert (el1.proxy() == 0);

  EXPECT_EXCEPTION (SG::ExcInvalidLink, *el1);

  assert (el1.classID() == clid);

  Link el2 (key, index2);
  assert (el2.cachedElement() == null_element);
  assert (!el2.isDefault());
  assert (el2.index() == index2);
  assert (el2.isValid());
  assert (!!el2);
  assert (*el2.cptr() == doindex(cont, index2));
  assert (el2.getDataPtr() == cont);
  assert (el2.getDataNonConstPtr() == cont);
  assert (el2.getStorableObjectPointer() == cont);
  assert (&el2.getStorableObjectRef() == cont);
  assert (el2.dataID() == key);
  assert (el2.key() == sgkey);
  assert (el2.source() == &store);
  typename Link::ElementConstPointer foo2 = el2;
  assert (*foo2 == doindex(cont, index2));
  typename Link::ElementConstReference foo3 = *el2;
  assert (foo3 == doindex(cont, index2));
  assert (el2.cachedElement() == doindex(cont, index2));
  assert (el2.proxy()->name() == key);

  Link el3 (sgkey, index2);
  assert (!el3.isDefault());
  assert (el3.index() == index2);
  assert (el3.isValid());
  assert (!!el3);
  assert (*el3.cptr() == doindex(cont, index2));
  assert (el3.getDataPtr() == cont);
  assert (el3.getDataNonConstPtr() == cont);
  assert (el3.dataID() == key);
  assert (el3.key() == sgkey);
  assert (el3.source() == &store);

  Link el4 (key, index2, doindex(cont, index2));
  assert (!el4.isDefault());
  assert (el4.index() == index2);
  assert (el4.isValid());
  assert (!!el4);
  assert (*el4.cptr() == doindex(cont, index2));
  assert (el4.getDataPtr() == cont);
  assert (el4.getDataNonConstPtr() == cont);
  assert (el4.dataID() == key);
  assert (el4.key() == sgkey);
  assert (el4.source() == &store);

  {
    DataLink<CONT> dl1 = el4.getDataLink();
    assert (!dl1.isDefault());
    assert (dl1.isValid());
    assert (dl1.cptr() == cont);
    assert (dl1.dataID() == key);
    assert (dl1.key() == sgkey);
    assert (dl1.source() == &store);

    const Link& cel4 = el4;
    const DataLink<CONT> dl2 = cel4.getDataLink();
    assert (!dl2.isDefault());
    assert (dl2.isValid());
    assert (dl2.cptr() == cont);
    assert (dl2.dataID() == key);
    assert (dl2.key() == sgkey);
    assert (dl2.source() == &store);
  }

  Link el5 (sgkey, index2, doindex(cont, index2));
  assert (!el5.isDefault());
  assert (el5.index() == index2);
  assert (el5.isValid());
  assert (!!el5);
  assert (*el5.cptr() == doindex(cont, index2));
  assert (el5.getDataPtr() == cont);
  assert (el5.getDataNonConstPtr() == cont);
  assert (el5.dataID() == key);
  assert (el5.key() == sgkey);
  assert (el5.source() == &store);

  Link el6 (*cont2, index2b);
  assert (!el6.isDefault());
  assert (el6.index() == index2b);
  assert (el6.isValid());
  assert (!!el6);
  assert (*el6.cptr() == doindex(cont2, index2b));
  assert (el6.getDataPtr() == cont2);
  assert (el6.getDataNonConstPtr() == cont2);
  assert (el6.dataID() == key2);
  assert (el6.key() == sgkey2);
  assert (el6.source() == &store);

  Link el7 (el6);
  assert (!el7.isDefault());
  assert (el7.index() == index2b);
  assert (el7.isValid());
  assert (!!el7);
  assert (*el7.cptr() == doindex(cont2, index2b));
  assert (el7.getDataPtr() == cont2);
  assert (el7.getDataNonConstPtr() == cont2);
  assert (el7.dataID() == key2);
  assert (el7.key() == sgkey2);
  assert (el7.source() == &store);

  el1 = el7;
  assert (!el1.isDefault());
  assert (el1.index() == index2b);
  assert (el1.isValid());
  assert (!!el1);
  assert (*el1.cptr() == doindex(cont2, index2b));
  assert (el1.getDataPtr() == cont2);
  assert (el1.getDataNonConstPtr() == cont2);
  assert (el1.dataID() == key2);
  assert (el1.key() == sgkey2);
  assert (el1.source() == &store);

  Link el8;
  assert (el8.isDefault());
  assert (el8.toIndexedElement (*cont, index1));
  assert (!el8.isDefault());
  assert (el8.index() == index1);
  assert (el8.isValid());
  assert (!!el8);
  assert (*el8.cptr() == doindex(cont, index1));
  assert (el8.getDataPtr() == cont);
  assert (el8.getDataNonConstPtr() == cont);
  assert (el8.dataID() == key);
  assert (el8.key() == sgkey);
  assert (el8.source() == &store);
  assert (!el8.toIndexedElement (*cont, index2));

  Link el9;
  assert (el9.isDefault());
  assert (el9.toContainedElement (*cont, doindex(cont, index2)));
  assert (!el9.isDefault());
  assert (el9.index() == index2);
  assert (el9.isValid());
  assert (!!el9);
  assert (*el9.cptr() == doindex(cont, index2));
  assert (el9.getDataPtr() == cont);
  assert (el9.getDataNonConstPtr() == cont);
  assert (el9.dataID() == key);
  assert (el9.key() == sgkey);
  assert (el9.source() == &store);
  assert (!el9.toContainedElement (*cont, doindex(cont, index2)));

  EXPECT_EXCEPTION (SG::ExcElementNotFound,
                    {
                      Link el9a;
                      el9a.toContainedElement (*cont, doindex(cont2, index2b));
                    });

  Link el10 (doindex(cont, index1), *cont);
  assert (!el10.isDefault());
  assert (el10.index() == index1);
  assert (el10.isValid());
  assert (!!el10);
  assert (*el10.cptr() == doindex(cont, index1));
  assert (el10.getDataPtr() == cont);
  assert (el10.getDataNonConstPtr() == cont);
  assert (el10.dataID() == key);
  assert (el10.key() == sgkey);
  assert (el10.source() == &store);

  Link el11;
  assert (el11.setElement (doindex(cont, index2)));
  assert (!el11.isDefault());
  assert (el11.index() == null_index);
  assert (el11.isValid());
  assert (!!el11);
  assert (*el11.cptr() == doindex(cont, index2));
  assert (el11.getDataPtr() == 0);
  assert (el11.getDataNonConstPtr() == 0);
  assert (el11.dataID() == "");
  assert (el11.key() == 0);
  assert (el11.source() == 0);

  assert (el11.setStorableObject (*cont));
  assert (!el11.isDefault());
  assert (el11.index() == index2);
  assert (el11.isValid());
  assert (!!el11);
  assert (*el11.cptr() == doindex(cont, index2));
  assert (el11.getDataPtr() == cont);
  assert (el11.getDataNonConstPtr() == cont);
  assert (el11.dataID() == key);
  assert (el11.key() == sgkey);
  assert (el11.source() == &store);

  assert (!el11.setElement (doindex(cont, index2)));
  assert (!el11.setStorableObject (*cont));

  Link el12;
  assert (el12.setStorableObject (*cont));
  assert (!el12.isDefault());
  assert (el12.index() == null_index);
  assert (!el12.isValid());
  assert (!el12);
  assert (el12.cptr() == 0);
  assert (el12.getDataPtr() == cont);
  assert (el12.getDataNonConstPtr() == cont);
  assert (el12.dataID() == key);
  assert (el12.key() == sgkey);
  assert (el12.source() == &store);

  assert (el12.setElement (doindex(cont, index2)));
  assert (!el12.isDefault());
  assert (el12.index() == index2);
  assert (el12.isValid());
  assert (!!el12);
  assert (*el12.cptr() == doindex(cont, index2));
  assert (el12.getDataPtr() == cont);
  assert (el12.getDataNonConstPtr() == cont);
  assert (el12.dataID() == key);
  assert (el12.key() == sgkey);
  assert (el12.source() == &store);

  assert (!el12.setElement (doindex(cont, index2)));
  assert (!el12.setStorableObject (*cont));

  el12.resetWithKeyAndIndex (key2, index1b);
  assert (!el12.isDefault());
  assert (el12.index() == index1b);
  assert (el12.isValid());
  assert (!!el12);
  assert (*el12.cptr() == doindex(cont2, index1b));
  assert (el12.getDataPtr() == cont2);
  assert (el12.getDataNonConstPtr() == cont2);
  assert (el12.dataID() == key2);
  assert (el12.key() == sgkey2);
  assert (el12.source() == &store);

  el12.resetWithKeyAndIndex (sgkey, index2);
  assert (!el12.isDefault());
  assert (el12.index() == index2);
  assert (el12.isValid());
  assert (!!el12);
  assert (*el12.cptr() == doindex(cont, index2));
  assert (el12.getDataPtr() == cont);
  assert (el12.getDataNonConstPtr() == cont);
  assert (el12.dataID() == key);
  assert (el12.key() == sgkey);
  assert (el12.source() == &store);

  Link el13 (el12, index1);
  assert (!el13.isDefault());
  assert (el13.index() == index1);
  assert (el13.isValid());
  assert (!!el13);
  assert (*el13.cptr() == doindex(cont, index1));
  assert (el13.getDataPtr() == cont);
  assert (el13.getDataNonConstPtr() == cont);
  assert (el13.dataID() == key);
  assert (el13.key() == sgkey);
  assert (el13.source() == &store);

  el12.reset();
  assert (el12.isDefault());
  assert (el12.index() == null_index);
  assert (!el12.isValid());
  assert (!el12);
  assert (el12.cptr() == 0);
  assert (el12.getDataPtr() == 0);
  assert (el12.getDataNonConstPtr() == 0);
  assert (el12.dataID() == "");
  assert (el12.key() == 0);
  assert (el12.source() == 0);
}


void test1 (SGTest::TestStore& store)
{
  std::cout << "test1\n";

  FooCont* foocont = new FooCont;
  foocont->push_back (new Foo(1));
  foocont->push_back (new Foo(2));
  foocont->push_back (new Foo(3));
  foocont->push_back (new Foo(4));
  store.record (foocont, "foocont");

  FooCont* foocont2 = new FooCont;
  foocont2->push_back (new Foo(11));
  foocont2->push_back (new Foo(12));
  foocont2->push_back (new Foo(13));
  foocont2->push_back (new Foo(14));
  store.record (foocont2, "foocont2");

  testit<FooCont> (store,
                   "foocont", foocont,
                   "foocont2", foocont2,
                   1, 1, 2, 2, -1);
}


// Testing references to objects not in SG.
void test2 (SGTest::TestStore& store)
{
  std::cout << "test2\n";

  TestStore::sgkey_t fookey = store.stringToKey ("foocont_t2", fooclid);
  Foo* foo1 = new Foo(101);

  ElementLink<FooCont> el1;
  assert (el1.setElement (foo1));
  assert (!el1.isDefault());
  assert ((int)el1.index() == -1);
  assert (el1.isValid());
  assert (!!el1);
  assert (*el1.cptr() == foo1);
  assert (el1.getDataPtr() == 0);
  assert (el1.getDataNonConstPtr() == 0);
  assert (el1.dataID() == "");
  assert (el1.key() == 0);
  assert (el1.source() == 0);
  assert (el1.proxy() == 0);

  ElementLink<FooCont> el2 (fookey, 2, foo1);
  assert (!el2.isDefault());
  assert ((int)el2.index() == 2);
  assert (el2.isValid());
  assert (!!el2);
  assert (*el2.cptr() == foo1);
  assert (el2.getDataPtr() == 0);
  assert (el2.getDataNonConstPtr() == 0);
  assert (el2.dataID() == "foocont_t2");
  assert (el2.key() == fookey);
  assert (el2.source() == &store);
  assert (el2.proxy()->name() == "foocont_t2");

  FooCont* foocont = new FooCont;
  for (int i=0; i < 4; i++)
    foocont->push_back (new Foo(i+100));

  ElementLink<FooCont> el3 (*foocont, 2);
  assert (!el3.isDefault());
  assert ((int)el3.index() == 2);
  assert (el3.isValid());
  assert (!!el3);
  assert (*el3.cptr() == (*foocont)[2]);
  assert (el3.getDataPtr() == foocont);
  assert (el3.getDataNonConstPtr() == foocont);
  assert (el3.dataID() == "");

  store.record (foocont, "foocont_t2");
  assert (el3.dataID() == "foocont_t2");
  assert (el3.key() == 0); // toPersistent should set this
  assert (el3.source() == &store);
  assert (el3.proxy()->name() == "foocont_t2");

  assert (el3.toPersistent());
  assert (el3.key() == fookey);
}


// toTransient, toPersistent
void test3 (SGTest::TestStore& store)
{
  std::cout << "test3\n";

  TestStore::sgkey_t sgkey = store.stringToKey ("foocont3", fooclid);

  FooCont* foocont3 = new FooCont;
  for (int i=0; i < 4; i++)
    foocont3->push_back (new Foo(i+200));
  store.record (foocont3, "foocont3");

  ElementLink<FooCont> el1;
  ElementLinkBase_test::setLink(el1, sgkey, 2);
  assert (el1.toTransient());
  assert (*el1.cptr() == (*foocont3)[2]);

  ElementLink<FooCont> el2;
  assert (el2.toPersistent());
  assert (!el2);
  el2 = ElementLink<FooCont> ("foocont3", 1);
  assert (*el2.cptr() == (*foocont3)[1]);
  assert (el2.toPersistent());
  assert (*el2.cptr() == (*foocont3)[1]);

  FooCont* foocont3a = new FooCont;
  for (int i=0; i < 4; i++)
    foocont3a->push_back (new Foo(i+300));
  store.record (foocont3a, "foocont3a");

  FooCont* foocont3b = new FooCont;
  for (int i=0; i < 4; i++)
    foocont3b->push_back (new Foo(i+350));
  store.record (foocont3b, "foocont3b");

  store.remap<FooCont> ("foocont3a", "foocont3b", 2, 1);

  el2 = ElementLink<FooCont> ("foocont3a", 2);
  assert (*el2.cptr() == (*foocont3a)[2]);
  assert (el2.toPersistent());
  assert (*el2.cptr() == (*foocont3b)[1]);

  ElementLink<FooCont> el3;
  el3.setElement ((*foocont3a)[2]);
  assert (!el3.toPersistent());
}


// test alt store
void test4 (SGTest::TestStore& store)
{
  std::cout << "test4\n";
  
  TestStore store2;
  FooCont* foocont4 = new FooCont;
  for (int i=0; i < 4; i++)
    foocont4->push_back (new Foo(i+400));
  store2.record (foocont4, "foocont4");

  EventContext ctx;
  ctx.setExtension( Atlas::ExtendedEventContext(&store2) );

  ElementLink<FooCont> el1 ("foocont4", 2, &store2);
  assert (*el1.cptr() == (*foocont4)[2]);
  ElementLink<FooCont> el1a ("foocont4", 2, ctx);
  assert (*el1a.cptr() == (*foocont4)[2]);

  ElementLink<FooCont> el2 ("foocont4", 2);
  assert (el2.cptr() == 0);

  assert (SG::CurrentEventStore::setStore(&store2) == &store);

  ElementLink<FooCont> el3 ("foocont4", 2);
  assert (*el3.cptr() == (*foocont4)[2]);

  assert (SG::CurrentEventStore::setStore(&store) == &store2);
}


// dummy proxy creation
void test5 (SGTest::TestStore& store)
{
  std::cout << "test5\n";

  ElementLink<FooCont> el1 ("foocont20", 2);
  assert (!el1.isDefault());
  assert (el1.dataID() == "foocont20");
  assert (el1.classID() == fooclid);
  assert (!el1.isValid());

  TestStore::sgkey_t sgkey = store.stringToKey ("foocont20", fooclid);
  assert (el1.key() == sgkey);
  FooCont* foocont20 = new FooCont;
  for (int i=0; i < 4; i++)
    foocont20->push_back (new Foo(i+500));
  store.record (foocont20, "foocont20");
  assert (el1.isValid());
  assert (*el1.cptr() == (*foocont20)[2]);
}


// comparison ops
void test6 (SGTest::TestStore& /*store*/)
{
  std::cout << "test6\n";

  ElementLink<FooCont> el1 ("foocont20", 2);
  ElementLink<FooCont> el2 ("foocont21", 1);

  assert (el1 < el2);
  assert (!(el1 > el2));
  assert (el1 != el2);
  assert (el1 == el1);

  ElementLink<FooCont> el3;
  assert (!(el3 < el3));
  assert (el3 < el1);
  assert (!(el1 < el3));

  Foo foo;
  ElementLink<FooCont> el4;
  el4.setElement(&foo);
  assert (el3 < el4);
  assert (!(el4 < el3));

  bool res = false;
  EXPECT_EXCEPTION (SG::ExcIncomparableEL, res = el4 < el1);
  EXPECT_EXCEPTION (SG::ExcIncomparableEL, res = el1 < el4);
  assert (res == false);
}


class Hell {
public:
  Hell() { };
  ElementLink<FooCont> test(FooCont& cont, ElementLink<FooCont>& link) {
    const Foo* f = *link;
    ElementLink<FooCont> ftest;
    ftest.toContainedElement(cont, f);
    return ftest;
  }

  int get( const Foo& f) {return f.x;}
};


// tests from ControlTest
void test7 (SGTest::TestStore& store)
{
  std::cout << "test7\n";

  FooCont* fooVec = new FooCont();
  Foo* f1 = new Foo(1);
  Foo* f2 = new Foo(2);
  Foo* f3 = new Foo(3);

  fooVec->push_back(f1);
  fooVec->push_back(f2);
  fooVec->push_back(f3);
  store.record (fooVec, "fooVec");
  ElementLink<FooCont> FooVecLink1;
  FooVecLink1.toContainedElement(*fooVec, f1);

  // move to persistent state and check index 
  FooVecLink1.toPersistent();
  assert(FooVecLink1.index() == 0);
  // get the pointer from ElementLink and check
  assert ((**FooVecLink1).x == 1);

  // test of indexed Element
  ElementLink <FooCont> FooVecLink2(*fooVec, 1);
  // Dereference to make sure we get Foo2 back:
  assert ((**FooVecLink2).x == 2);
  // move to persistent state and check index
  FooVecLink2.toPersistent();
  assert(FooVecLink2.index() == 1);

  ElementLink <FooCont> FooVecLink21;
  assert( FooVecLink21.toIndexedElement(*fooVec, 1) );
  // Dereference to make sure we get Foo2 back:
  assert ((**FooVecLink21).x == 2);
  // move to persistent state and check index
  FooVecLink21.toPersistent();
  assert(FooVecLink21.index() == 1);

  // test of Element Link with key
  ElementLink<FooCont> FooVecLink3("fooVec", 2);
  assert (FooVecLink3.index() == 2);
  const Foo* f = *FooVecLink3;  
  assert (f->x == 3);

  // check resetWithKeyAndIndex
  // assign el1 to el4
  ElementLink<FooCont> FooVecLink4 = FooVecLink1;
  // check index 
  assert(FooVecLink4.index() == 0);
  // get the pointer from ElementLink and check
  assert ((**FooVecLink4).x == 1);
  // reset EL
  FooVecLink4.resetWithKeyAndIndex("fooVec", 2);
  // Should be in valid state
  assert (FooVecLink4.isValid());
  // Should be in identified state
  //assert (IdentifiedState::valid(&FooVecLink4));
  // move to transient and check that it has the right values
  FooVecLink4.toTransient();
  f = *FooVecLink4;  
  assert (f->x == 3);


  // create a vector of element links and iterate over it:
  // test of ElementLink in/out of scope
  ElementLink<FooCont> linkTest;
  linkTest.toContainedElement(*fooVec, f);
  Hell h;
  ElementLink<FooCont> ftest = h.test(*fooVec, linkTest);
  // make sure that the two pointers are the same.
  assert (f == *ftest);

  // test of ElementLink setElement, setStorable
  ElementLink<FooCont> linkset;
  assert( linkset.toPersistent() );

  const Foo* pCF(f);
  linkset.setElement(pCF);  // set only element
  linkset.toPersistent(); // XXX SHOULD BE AN ERROR???

  linkset.setStorableObject(*fooVec);  // set Collection
  assert (linkset.index() == 2);
  assert ((**linkset).x == 3);
  assert( linkset.toPersistent() );

  // test2:
  std::vector<std::string> strVec;
  store.record(&strVec, " key");
  strVec.push_back("foo");
  strVec.push_back("bar");

  // Test2: test of contained Element
  ElementLink< std::vector<std::string> > strLink1;
  std::string x = "foo"; // note that you cannot pass foo directly.
  strLink1.toContainedElement(strVec, x);
  // note that the call to .index() itself calls toPersistent()
  assert (strLink1.index() == 0);
  assert (*strLink1=="foo");

  // Test2: test of indexed Element
  ElementLink< std::vector<std::string> > strLink2(strVec, 1);
  assert(*strLink2 == "bar");

  // test 3: use simple vector<objects> for test.
  std::vector<Foo>* fooVec2 = new std::vector<Foo>();
  Foo foo1(1);
  Foo foo2(2);
  fooVec2->push_back(foo1);
  fooVec2->push_back(foo2);
  store.record(fooVec2, "fooVec2");

  ElementLink< std::vector<Foo> > FooVec2Link1;
  FooVec2Link1.toContainedElement(*fooVec2, foo1);
  assert (foo1 == *FooVec2Link1.cptr());

  // Test3: test of contained Element
  // move to persistent state
  FooVec2Link1.toPersistent();
  assert(FooVec2Link1.index() == 0);

  // Test3: test of indexed Element
  ElementLink< std::vector<Foo> > FooVec2Link2(*fooVec2, 1);
  // Dereference to make sure we get Foo2 back:
  //  assert(FooVecLink2.d() == 2.0);
  // move to persistent state
  //  assert(FooVec2Link2.index() == 1);
  FooVec2Link2.toPersistent();
  // test of ElementLinkVector:

  // Starting Test 4: Testing ElementLinkVector
  typedef ElementLinkVector<FooCont> ELVec;
  ELVec linkVector;
  linkVector.push_back(FooVecLink1);
  linkVector.push_back(FooVecLink2);
  linkVector.push_back(FooVecLink3);
  const Foo* fv = *linkVector.elementCPtr(0);  //!!FIXME
  assert (fv->x == 1);
  assert ((**linkVector.elementCPtr(1)).x == 2);
  assert ((**linkVector.elementCPtr(2)).x == 3);
  // let us check the begin() end() of ElementLinkVector:
  int result = 0;
  ELVec::iterator iter(linkVector.begin()), iend(linkVector.end());
  for (; iter!=iend; ++iter) {
    result += 1;
    assert ((**iter)->x == result);
  }
}


// Test for vector with non-pointer payload
void test8 (SGTest::TestStore& store)
{
  std::cout << "test8\n";

  StrVec* strvec = new StrVec;
  strvec->push_back ("1");
  strvec->push_back ("2");
  strvec->push_back ("3");
  strvec->push_back ("4");
  store.record (strvec, "strvec");

  StrVec* strvec2 = new StrVec;
  strvec2->push_back ("11");
  strvec2->push_back ("12");
  strvec2->push_back ("13");
  strvec2->push_back ("14");
  store.record (strvec2, "strvec2");

  testit<StrVec> (store,
                  "strvec", strvec,
                  "strvec2", strvec2,
                  1, 1, 2, 2, -1);

  ElementLink<StrVec> el2 ("strvec", 2);
  assert (el2->size() == 1);
}


// Test for set
void test9 (SGTest::TestStore& store)
{
  std::cout << "test9\n";

  StrSet* strset = new StrSet;
  strset->insert ("1");
  strset->insert ("2");
  strset->insert ("3");
  strset->insert ("4");
  store.record (strset, "strset");

  StrSet* strset2 = new StrSet;
  strset2->insert ("11");
  strset2->insert ("12");
  strset2->insert ("13");
  strset2->insert ("14");
  store.record (strset2, "strset2");

  testit<StrSet> (store,
                  "strset", strset,
                  "strset2", strset2,
                  "1", "11", "2", "12", "");

  ElementLink<StrVec> el2 ("strvec", 2);
  assert (el2->size() == 1);
}


// Test for map
void test10 (SGTest::TestStore& store)
{
  std::cout << "test10\n";

  StrMap* strmap = new StrMap;
  (*strmap)["1"] = 1;
  (*strmap)["2"] = 2;
  (*strmap)["3"] = 3;
  (*strmap)["4"] = 4;
  store.record (strmap, "strmap");

  StrMap* strmap2 = new StrMap;
  (*strmap2)["11"] = 11;
  (*strmap2)["12"] = 12;
  (*strmap2)["13"] = 13;
  (*strmap2)["14"] = 14;
  store.record (strmap2, "strmap2");

  testit<StrMap> (store,
                  "strmap", strmap,
                  "strmap2", strmap2,
                  "1", "11", "2", "12", "");
}


// Test for identcont
void test11 (SGTest::TestStore& store)
{
  std::cout << "test11\n";

  IdentTest* imap = new IdentTest;
  imap->fill ("a");
  store.record (imap, "imap");

  IdentTest* imap2 = new IdentTest;
  imap2->fill ("b");
  store.record (imap2, "imap2");

  testit<IdentTest> (store,
                     "imap", imap,
                     "imap2", imap2,
                     IdentContIndex(1,1).hashAndIndex(),
                     IdentContIndex(1,1).hashAndIndex(),
                     IdentContIndex(2,2).hashAndIndex(),
                     IdentContIndex(2,2).hashAndIndex(),
                     IdentContIndex().hashAndIndex());
}


// default store setting
void test12 (SGTest::TestStore& store)
{
  std::cout << "test12\n";
  TestStore store2;

  assert (SG::CurrentEventStore::setStore(&store2) == &store);
  assert (SG::CurrentEventStore::setStore(&store) == &store2);
}


// Converting ctor.
void test13 (SGTest::TestStore& store)
{
  std::cout << "test13\n";

  // Pointer, raw element.
  BarCont* bar = new BarCont;
  bar->push_back (new Bar (1));
  bar->push_back (new Bar (2));
  bar->push_back (new Bar (3));

  size_t null_index = -1;

  ElementLink<BarCont> el1;
  el1.setElement ((*bar)[1]);
  ElementLink<FooCont> el2 (el1);
  assert (*el1.cptr() == (*bar)[1]);
  assert (*el2.cptr() == (*bar)[1]);
  assert (static_cast<const void*>(*el1.cptr()) !=
          static_cast<const void*>(*el2.cptr()));
  assert (el1.getDataPtr() == nullptr);
  assert (el2.getDataPtr() == nullptr);
  assert (el1.key() == 0);
  assert (el2.key() == 0);
  assert (el1.index() == null_index);
  assert (el2.index() == null_index);

  // Pointer, storable not in SG.
  ElementLink<BarCont> el3 (*bar, 1);
  ElementLink<FooCont> el4 (el3);
  assert (*el3.cptr() == (*bar)[1]);
  assert (*el4.cptr() == (*bar)[1]);
  assert (static_cast<const void*>(*el3.cptr()) !=
          static_cast<const void*>(*el4.cptr()));
  assert (el3.getDataPtr() == bar);
  assert (el4.getDataPtr() == bar);
  assert (static_cast<const void*>(el3.getDataPtr()) !=
          static_cast<const void*>(el4.getDataPtr()));
  assert (el3.key() == 0);
  assert (el4.key() == 0);
  assert (el3.index() == 1);
  assert (el4.index() == 1);

  // Pointer, storable in SG.
  store.record (bar, "barcont");
  ElementLink<BarCont> el5 ("barcont", 1);
  ElementLink<FooCont> el6 (el5);
  assert (*el5.cptr() == (*bar)[1]);
  assert (*el6.cptr() == (*bar)[1]);
  assert (static_cast<const void*>(*el5.cptr()) !=
          static_cast<const void*>(*el6.cptr()));
  assert (el5.getDataPtr() == bar);
  assert (el6.getDataPtr() == bar);
  assert (static_cast<const void*>(el5.getDataPtr()) !=
          static_cast<const void*>(el6.getDataPtr()));
  assert (el5.dataID() == "barcont");
  assert (el6.dataID() == "barcont");
  assert (el5.index() == 1);
  assert (el6.index() == 1);

  // Non-pointer, raw element.
  BazCont* baz = new BazCont;
  baz->insert ("1");
  baz->insert ("2");
  baz->insert ("3");

  ElementLink<BazCont> el11;
  el11.setElement (Baz ("bar1"));
  ElementLink<StrSet> el12 (el11);
  assert (*el11.cptr() == "bar1");
  assert (*el12.cptr() == "bar1");
  assert (el11.cptr()->s == "bar1asd");
  assert (el11.getDataPtr() == nullptr);
  assert (el12.getDataPtr() == nullptr);
  assert (el11.key() == 0);
  assert (el12.key() == 0);
  assert (el11.index() == "");
  assert (el12.index() == "");

  // Non-pointer, storable not in SG.
  ElementLink<BazCont> el13 (*baz, Baz("2"));
  ElementLink<StrSet> el14 (el13);
  assert (*el13.cptr() == "2");
  assert (*el14.cptr() == "2");
  assert (el13.cptr()->s == "2asd");
  assert (el13.getDataPtr() == baz);
  assert (el14.getDataPtr() == baz);
  assert (static_cast<const void*>(el13.getDataPtr()) !=
          static_cast<const void*>(el14.getDataPtr()));
  assert (el13.key() == 0);
  assert (el14.key() == 0);
  assert (el13.index() == Baz("2"));
  assert (el14.index() == "2");

  // Non-pointer, storable in SG.
  store.record (baz, "bazcont");
  ElementLink<BazCont> el15 ("bazcont", Baz("2"));
  ElementLink<StrSet> el16 (el15);
  assert (*el15.cptr() == "2");
  assert (*el16.cptr() == "2");
  assert (el15.cptr()->s == "2asd");
  assert (el16.getDataPtr() == baz);
  assert (el15.getDataPtr() == baz);
  assert (static_cast<const void*>(el15.getDataPtr()) !=
          static_cast<const void*>(el16.getDataPtr()));
  assert (el15.dataID() == "bazcont");
  assert (el16.dataID() == "bazcont");
  assert (el15.index() == Baz("2"));
  assert (el16.index() == "2");
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
  test6 (store);
  test7 (store);
  test8 (store);
  test9 (store);
  test10 (store);
  test11 (store);
  test12 (store);
  test13 (store);
  return 0;
}
