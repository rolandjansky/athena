/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/test/AuxVectorBase_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2013
 * @brief Regression tests for AuxVectorBase
 */

#undef NDEBUG
#include <exception>
#include <string>
#include <sstream>


//************************************************************************


class dv_test_err
  : public std::exception
{
public:
  explicit dv_test_err (const char* file,
                        int line,
                        const std::string& what);
  virtual ~dv_test_err() throw() {}
  virtual const char* what() const throw() { return m_what.c_str(); }
private:
  std::string m_what;
};


dv_test_err::dv_test_err (const char* file,
                          int line,
                          const std::string& what)
{
  std::ostringstream os;
  os << file << ":" << line << " myassertion failure: " << what;
  m_what = os.str();
}


void throw_dv_test_err (const char* file, int line, const char* what)
{
  throw (dv_test_err (file, line, what));
}

#define myassert(X) do {     \
    if (!(X)) { \
      throw_dv_test_err (__FILE__, __LINE__, #X); \
    } \
  } while (0)


#define ATHCONTAINERS_ASSERT(X) myassert(X)


//************************************************************************


#include "AthContainers/AuxVectorBase.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/AuxStoreInternal.h"
#include "AthContainers/exceptions.h"
#include "TestTools/random.h"
#include "CxxUtils/checker_macros.h"
#include <cassert>
#include <vector>
#include <iostream>



//*************************************************************************


struct A
{
  int x;
  A(int the_x=0) : x(the_x) {}
};
struct B
  : public SG::AuxElement
{
  int x;
  B(int the_x=0) : x(the_x) {}
};

struct C
  : public A, public SG::AuxElement
{
  C(int the_x=0) : A(the_x) {}
};

struct ACont
{
  static const bool has_virtual = false;
  typedef std::vector<A*> BaseContainer;
  typedef A base_value_type;
  typedef A base_type;
};
struct BCont
{
  static const bool has_virtual = false;
  typedef std::vector<B*> BaseContainer;
  typedef B base_value_type;
  typedef B base_type;
};
struct CCont
{
  static const bool has_virtual = false;
  typedef std::vector<A*> BaseContainer;
  typedef C base_value_type;
  typedef A base_type;
};


template <class BASE>
std::vector<BASE*> make_v()
{
  std::vector<BASE*> v;
  for (int i=0; i < 10; i++)
    v.push_back (new BASE(i));
  return v;
}


template <class BASE, class D>
std::vector<BASE*> make_v2()
{
  std::vector<BASE*> v;
  for (int i=0; i < 10; i++)
    v.push_back (new D(i));
  return v;
}


namespace SG {


class AuxVectorBase_test
  : public AuxVectorBase
{
public:
  using AuxVectorBase::initAuxVectorBase;
  using AuxVectorBase::setIndices;
  using AuxVectorBase::clearIndex;
  using AuxVectorBase::clearIndices;
  using AuxVectorBase::reserve;
  using AuxVectorBase::resize;
  using AuxVectorBase::shift; 
  using AuxVectorBase::moveAux;
  using AuxVectorBase::swapElementsAux;
  using AuxVectorBase::resortAux;
  using AuxVectorBase::swap;

  virtual size_t size_v() const { return 10; }
  virtual size_t capacity_v() const { return 20; }

  template <class Iterator>
  void check_ordered (Iterator beg, Iterator end, size_t index = 0)
  {
    while (beg < end) {
      assert ((*beg)->index() == index);
      assert ((*beg)->container() == this);
      ++index;
      ++beg;
    }
  }


  template <class Cont>
  void check_ordered (Cont& c, size_t index = 0)
  {
    check_ordered (c.begin(), c.end(), index);
  }


  template <class Iterator>
  void check_clear (Iterator beg, Iterator end)
  {
    while (beg < end) {
      assert ((*beg)->index() == 0);
      assert ((*beg)->container() == 0);
      ++beg;
    }
  }


  template <class Container>
  void check_clear (Container& c)
  {
    check_clear (c.begin(), c.end());
  }


  template <class Iterator>
  void order (Iterator beg, Iterator end, size_t index = 0)
  {
    initAuxVectorBase<B> (SG::OWN_ELEMENTS, SG::DEFAULT_TRACK_INDICES);
    setIndices (beg, end, index);
  }

  template <class Cont>
  void order (Cont& c, size_t index = 0)
  {
    order (c.begin(), c.end(), index);
  }

  static
  void copyAux (SG::AuxElement& e1, const SG::AuxElement& e2)
  {
    e1.copyAux (e2);
  }

  static
  void clearAux (SG::AuxElement& e)
  {
    e.clearAux();
  }

  void setIndexTest (SG::AuxElement* p, size_t index)
  {
    p->setIndex (index, this);
  }
};


template <>
struct AuxStore_traits<std::vector<B*> > : public AuxStore_traits_AuxDefault {};
template <>
struct AuxStore_traits<std::vector<C*> > : public AuxStore_traits_AuxDefault {};



class AuxStoreInternal_test
  : public AuxStoreInternal
{
public:
  using AuxStoreInternal::AuxStoreInternal;

  virtual const void* getData (SG::auxid_t auxid) const override
  {
    m_gets.push_back (auxid);
    return AuxStoreInternal::getData (auxid);
  }

  virtual void* getData (SG::auxid_t auxid, size_t size, size_t capacity)
    override
  {
    m_gets.push_back (auxid);
    return AuxStoreInternal::getData (auxid, size, capacity);
  }

  mutable std::vector<SG::auxid_t> m_gets ATLAS_THREAD_SAFE;
};


} // namespace SG


void test1()
{
  SG::AuxVectorBase_test b1;
  assert (!b1.trackIndices());
  assert (b1.size_v() == 10);
  assert (b1.capacity_v() == 20);
}


void test_init()
{
  SG::AuxVectorBase_test b1;
  assert (!b1.trackIndices());
  b1.initAuxVectorBase<A> (SG::OWN_ELEMENTS, SG::DEFAULT_TRACK_INDICES);
  assert (!b1.trackIndices());

  b1.initAuxVectorBase<B> (SG::OWN_ELEMENTS, SG::DEFAULT_TRACK_INDICES);
  assert (b1.trackIndices());
  b1.initAuxVectorBase<B> (SG::OWN_ELEMENTS, SG::NEVER_TRACK_INDICES);
  assert (!b1.trackIndices());
  b1.initAuxVectorBase<B> (SG::OWN_ELEMENTS, SG::ALWAYS_TRACK_INDICES);
  assert (b1.trackIndices());

  b1.initAuxVectorBase<B> (SG::VIEW_ELEMENTS, SG::DEFAULT_TRACK_INDICES);
  assert (!b1.trackIndices());
  b1.initAuxVectorBase<B> (SG::VIEW_ELEMENTS, SG::NEVER_TRACK_INDICES);
  assert (!b1.trackIndices());
  b1.initAuxVectorBase<B> (SG::VIEW_ELEMENTS, SG::ALWAYS_TRACK_INDICES);
  assert (b1.trackIndices());

  SG::AuxVectorBase_test b2;
  assert (!b2.trackIndices());
  b1.swap (b2);
  assert (!b1.trackIndices());
  assert (b2.trackIndices());

  SG::AuxStoreInternal store;
  b2.setStore (&store);
  b2.initAuxVectorBase<B> (SG::OWN_ELEMENTS, SG::DEFAULT_TRACK_INDICES);

  bool caught = false;
  try {
    b2.initAuxVectorBase<B> (SG::VIEW_ELEMENTS, SG::NEVER_TRACK_INDICES);
  }
  catch (const SG::ExcUntrackedSetStore&) {
    caught = true;
  }
  assert (caught);

  caught = false;
  try {
    b2.initAuxVectorBase<A> (SG::OWN_ELEMENTS, SG::DEFAULT_TRACK_INDICES);
  }
  catch (const SG::ExcUntrackedSetStore&) {
    caught = true;
  }
  assert (caught);
}


void test_set_store()
{
  std::cout << "test_set_store\n";
  SG::AuxVectorBase_test b;
  SG::AuxStoreInternal store;
  assert (!b.trackIndices());
  assert (!b.hasStore());
  assert (!b.hasNonConstStore());
  bool caught = false;
  try {
    b.setStore (&store);
  }
  catch (const SG::ExcUntrackedSetStore&) {
    caught = true;
  }
  assert (caught);

  caught = false;
  try {
    b.setStore ((SG::IConstAuxStore*)&store);
  }
  catch (const SG::ExcUntrackedSetStore&) {
    caught = true;
  }
  assert (caught);

  b.initAuxVectorBase<B> (SG::OWN_ELEMENTS, SG::DEFAULT_TRACK_INDICES);
  b.setStore ((SG::IConstAuxStore*)&store);
  assert (b.trackIndices());
  assert (b.hasStore());
  assert (!b.hasNonConstStore());
  b.setStore (&store);
  assert (b.trackIndices());
  assert (b.hasStore());
  assert (b.hasNonConstStore());
}


void test_set_store2()
{
  std::cout << "test_set_store2\n";
  SG::AuxVectorBase_test b;
  SG::AuxStoreInternal store;
  assert (!b.trackIndices());
  assert (!b.hasStore());
  assert (!b.hasNonConstStore());
  bool caught = false;
  try {
    b.setNonConstStore (&store);
  }
  catch (const SG::ExcUntrackedSetStore&) {
    caught = true;
  }
  assert (caught);

  caught = false;
  try {
    b.setConstStore (&store);
  }
  catch (const SG::ExcUntrackedSetStore&) {
    caught = true;
  }
  assert (caught);

  b.initAuxVectorBase<B> (SG::OWN_ELEMENTS, SG::DEFAULT_TRACK_INDICES);
  b.setConstStore (&store);
  assert (b.trackIndices());
  assert (b.hasStore());
  assert (!b.hasNonConstStore());
  b.setNonConstStore (&store);
  assert (b.trackIndices());
  assert (b.hasStore());
  assert (b.hasNonConstStore());
}


void test_set_indices()
{
  std::vector<A*> va = make_v<A>();
  std::vector<B*> vb = make_v<B>();

  SG::AuxVectorBase_test b1;
  b1.initAuxVectorBase<A> (SG::OWN_ELEMENTS, SG::DEFAULT_TRACK_INDICES);
  b1.setIndices (va.begin(), va.end(), 3);

  b1.initAuxVectorBase<B> (SG::VIEW_ELEMENTS, SG::DEFAULT_TRACK_INDICES);
  b1.setIndices (vb.begin(), vb.end(), 3);
  b1.check_clear (vb);

  b1.initAuxVectorBase<B> (SG::OWN_ELEMENTS, SG::DEFAULT_TRACK_INDICES);
  b1.setIndices (vb.begin(), vb.begin()+5, 3);
  b1.check_ordered (vb.begin(), vb.begin()+5, 3);
  b1.check_clear (vb.begin()+5, vb.end());
}


void test_clear_index()
{
  std::vector<A*> va = make_v<A>();
  std::vector<B*> vb = make_v<B>();

  SG::AuxVectorBase_test b1;
  b1.order (vb);
  b1.check_ordered (vb);

  b1.initAuxVectorBase<A> (SG::OWN_ELEMENTS, SG::DEFAULT_TRACK_INDICES);
  b1.clearIndex (va.begin());

  b1.initAuxVectorBase<B> (SG::VIEW_ELEMENTS, SG::DEFAULT_TRACK_INDICES);
  b1.clearIndex (vb.begin());
  b1.check_ordered (vb);

  b1.initAuxVectorBase<B> (SG::OWN_ELEMENTS, SG::DEFAULT_TRACK_INDICES);
  b1.clearIndex (vb.begin());
  b1.check_clear (vb.begin(), vb.begin()+1);
  b1.check_ordered (vb.begin()+1, vb.end(), 1);
}


void test_clear_indices()
{
  std::vector<A*> va = make_v<A>();
  std::vector<B*> vb = make_v<B>();

  SG::AuxVectorBase_test b1;
  b1.order (vb);
  b1.check_ordered (vb);

  b1.initAuxVectorBase<A> (SG::OWN_ELEMENTS, SG::DEFAULT_TRACK_INDICES);
  b1.clearIndices (va.begin(), va.begin()+5);

  b1.initAuxVectorBase<B> (SG::VIEW_ELEMENTS, SG::DEFAULT_TRACK_INDICES);
  b1.clearIndices (vb.begin(), vb.begin()+5);
  b1.check_ordered (vb);

  b1.initAuxVectorBase<B> (SG::VIEW_ELEMENTS, SG::ALWAYS_TRACK_INDICES);
  assert (b1.trackIndices());
  b1.clearIndices (vb.begin(), vb.begin()+5);
  b1.check_clear (vb.begin(), vb.begin()+5);
  b1.check_ordered (vb.begin()+5, vb.end(), 5);
}


void test_get_data()
{
  std::cout << "test_get_data\n";
  SG::AuxVectorBase_test b1;
  b1.initAuxVectorBase<B> (SG::OWN_ELEMENTS, SG::DEFAULT_TRACK_INDICES);
  const SG::AuxVectorBase& cb1 = b1;
  SG::auxid_t ityp = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt");
  bool caught = false;
  try {
    b1.getData<int> (ityp, 0);
  }
  catch (const SG::ExcNoAuxStore&) {
    caught = true;
  }
  assert (caught);

  SG::AuxStoreInternal store;
  SG::IConstAuxStore* cstore = &store;

  b1.setStore (cstore);
  caught = false;
  try {
    assert (b1.getData<int> (ityp, 0) == 0);
  }
  catch (const SG::ExcConstAuxData&) {
    caught = true;
  }
  assert (caught);
  caught = false;
  try {
    assert (cb1.getData<int> (ityp, 0) == 0);
  }
  catch (const SG::ExcBadAuxVar&) {
    caught = true;
  }
  assert (caught);

  b1.setStore (&store);
  assert (b1.getData<int> (ityp, 0) == 0);
  assert (b1.getData<int> (ityp, 1) == 0);
  b1.getData<int> (ityp, 0) = 1;
  b1.getData<int> (ityp, 1) = 2;
  assert (b1.getData<int> (ityp, 0) == 1);
  assert (b1.getData<int> (ityp, 1) == 2);

  b1.setStore (cstore);
  assert (cb1.getData<int> (ityp, 0) == 1);
  assert (cb1.getData<int> (ityp, 1) == 2);

  SG::auxid_t ftyp = SG::AuxTypeRegistry::instance().getAuxID<float> ("aFloat");
  float* ff = reinterpret_cast<float*> (store.getData (ftyp, 10, 20));
  ff[0] = 1.5;
  ff[1] = 2.5;
  assert (cb1.getData<float> (ftyp, 0) == 1.5);
  assert (cb1.getData<float> (ftyp, 1) == 2.5);
}


void test_reserve_resize()
{
  std::cout << "test_reserve_resize\n";
  SG::AuxVectorBase_test b1;
  b1.initAuxVectorBase<B> (SG::OWN_ELEMENTS, SG::DEFAULT_TRACK_INDICES);
  SG::AuxStoreInternal_test store;
  //SG::IConstAuxStore* cstore = &store;

  SG::auxid_t ityp = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt");
  b1.setStore (&store);
  b1.getData<int> (ityp, 1) = 10;

  b1.reserve<BCont> (50);
  int* ip = &b1.getData<int> (ityp, 1);
  b1.resize<BCont> (40);
  assert (ip == &b1.getData<int> (ityp, 1));
  assert (b1.getData<int> (ityp, 1) == 10);

  bool caught = false;
  try {
    b1.reserve<ACont> (1);
  }
  catch (dv_test_err&) {
    caught = true;
  }
  assert (caught);

  caught = false;
  try {
    b1.resize<ACont> (1);
  }
  catch (dv_test_err&) {
    caught = true;
  }
  assert (caught);

  SG::AuxVectorBase_test b2;
  b2.initAuxVectorBase<B> (SG::VIEW_ELEMENTS, SG::DEFAULT_TRACK_INDICES);
  b2.reserve<ACont> (1);
  b2.resize<ACont> (1);

  store.m_gets.clear();
  b1.getData<int> (ityp, 1) = 10;
  assert (store.m_gets.empty());
  // Should not clear the cache.
  b1.resize<BCont> (20);
  b1.getData<int> (ityp, 1) = 10;
  assert (store.m_gets.empty());
  // Should clear the cache.
  b1.resize<BCont> (1000);
  b1.getData<int> (ityp, 1) = 10;
  assert (store.m_gets == std::vector<SG::auxid_t>{ityp});
  store.m_gets.clear();
  // Below should not clear the cache.
  b1.resize<BCont> (500);
  b1.getData<int> (ityp, 1) = 10;
  assert (store.m_gets.empty());
  b1.resize<BCont> (1000);
  b1.getData<int> (ityp, 1) = 10;
  assert (store.m_gets.empty());
}


void test_shift()
{
  std::cout << "test_shift\n";
  SG::AuxVectorBase_test b1;
  b1.initAuxVectorBase<B> (SG::OWN_ELEMENTS, SG::DEFAULT_TRACK_INDICES);
  SG::AuxStoreInternal store;

  SG::auxid_t ityp = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt");
  b1.setStore (&store);
  std::vector<B*> vb = make_v<B>();
  b1.order (vb);
  for (int i=0; i < 10; i++)
    b1.getData<int> (ityp, i) = i+1;

  for (int i=0; i < 3; i++)
    vb.insert (vb.begin()+5, new B);
  b1.shift (vb, 5, 3);
  // 1 2 3 4 5 0 0 0 6 7 8 9 10
  for (int i=0; i<5; i++)
    assert (b1.getData<int> (ityp, i) == i+1);
  for (int i=5; i<8; i++)
    assert (b1.getData<int> (ityp, i) == 0);
  for (int i=8; i<13; i++)
    assert (b1.getData<int> (ityp, i) == i-2);

  b1.check_ordered (vb.begin(), vb.begin()+5, 0);
  b1.check_ordered (vb.begin()+8, vb.end(), 8);
  b1.order (vb);

  for (int i=0; i < 2; i++)
    vb.erase (vb.begin()+1);
  b1.shift (vb, 3, -2);
  // 1 4 5 0 0 0 6 7 8 9 10
  for (int i=1; i<3; i++)
    assert (b1.getData<int> (ityp, i) == i+3);
  for (int i=3; i<6; i++)
    assert (b1.getData<int> (ityp, i) == 0);
  for (int i=6; i<11; i++)
    assert (b1.getData<int> (ityp, i) == i);
  b1.check_ordered (vb);

  std::vector<A*> va = make_v<A>();
  bool caught = false;
  try {
    b1.shift (va, 1, 1);
  }
  catch (dv_test_err&) {
    caught = true;
  }
  assert (caught);

  SG::AuxVectorBase_test b2;
  b2.initAuxVectorBase<B> (SG::VIEW_ELEMENTS, SG::DEFAULT_TRACK_INDICES);
  b2.shift (va, 1, 1);
}


void test_get_types()
{
  std::cout << "test_get_types\n";
  SG::AuxVectorBase_test b1;
  b1.initAuxVectorBase<B> (SG::OWN_ELEMENTS, SG::DEFAULT_TRACK_INDICES);

  assert (b1.getAuxIDs().size() == 0);

  SG::AuxStoreInternal store;
  b1.setStore (&store);

  SG::auxid_t ityp1 = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt1");
  SG::auxid_t ityp2 = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt2");
  b1.getData<int> (ityp1, 1) = 10;
  b1.getData<int> (ityp2, 1) = 10;

  assert (b1.getAuxIDs().size() == 2);
  assert (b1.getAuxIDs().find(ityp1) != b1.getAuxIDs().end());
  assert (b1.getAuxIDs().find(ityp2) != b1.getAuxIDs().end());
}


void test_copy_aux()
{
  std::cout << "test_copy_aux\n";
  SG::AuxVectorBase_test b1;
  SG::AuxStoreInternal store1;
  b1.initAuxVectorBase<B> (SG::OWN_ELEMENTS, SG::DEFAULT_TRACK_INDICES);
  b1.setStore (&store1);

  SG::AuxVectorBase_test b2;
  b2.initAuxVectorBase<B> (SG::OWN_ELEMENTS, SG::DEFAULT_TRACK_INDICES);
  SG::AuxStoreInternal store2;
  b2.setStore (&store2);

  SG::auxid_t ityp1 = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt1");
  SG::auxid_t ityp2 = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt2");
  SG::auxid_t ityp3 = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt3");
  SG::auxid_t ftyp = SG::AuxTypeRegistry::instance().getAuxID<float> ("aFloat");
  b1.getData<int> (ityp1, 1) = 10;
  b1.getData<int> (ityp2, 1) = 20;
  b1.getData<float> (ftyp, 1) = 1.5;

  SG::AuxElement elt;
  b1.setIndexTest (&elt, 1);
  assert (elt.index() == 1);
  assert (elt.container() == &b1);

  b2.getData<int> (ityp1, 3) = 100;
  b2.getData<int> (ityp3, 3) = 200;
  b2.moveAux (3, &elt);
  assert (b2.getData<int> (ityp1, 3) == 10);
  assert (b2.getData<int> (ityp2, 3) == 20);
  assert (b2.getData<int> (ityp3, 3) == 0);
  assert (b2.getData<float> (ftyp, 3) == 1.5);
  assert (elt.index() == 3);
  assert (elt.container() == &b2);

  b2.moveAux (3, (SG::AuxElement*)0);
  assert (b2.getData<int> (ityp1, 3) == 0);
  assert (b2.getData<int> (ityp2, 3) == 0);
  assert (b2.getData<int> (ityp3, 3) == 0);
  assert (b2.getData<float> (ftyp, 3) == 0);

  b1.setIndexTest (&elt, 1);
  assert (elt.index() == 1);
  assert (elt.container() == &b1);

  SG::AuxElement elt2;
  b2.setIndexTest (&elt2, 2);

  SG::AuxVectorBase_test::copyAux (elt2, elt);
  assert (b2.getData<int> (ityp1, 2) == 10);
  assert (b2.getData<int> (ityp2, 2) == 20);
  assert (b2.getData<int> (ityp3, 2) == 0);
  assert (b2.getData<float> (ftyp, 2) == 1.5);
  assert (elt.index() == 1);
  assert (elt.container() == &b1);

  b2.moveAux (2, &elt, true);
  assert (b1.getData<int> (ityp1, 1) == 0);

  assert (b2.getData<int> (ityp1, 2) == 10);
  assert (b2.getData<int> (ityp2, 2) == 20);
  assert (b2.getData<int> (ityp3, 2) == 0);
  assert (b2.getData<float> (ftyp, 2) == 1.5);

  SG::AuxElement elt3;
  b2.moveAux (2, &elt3, false, true);
  assert (b2.getData<int> (ityp1, 2) == 10);
  assert (b2.getData<int> (ityp2, 2) == 20);
  assert (b2.getData<int> (ityp3, 2) == 0);
  assert (b2.getData<float> (ftyp, 2) == 1.5);

  SG::AuxElement elt4;
  b2.moveAux (2, &elt4);
  assert (b2.getData<int> (ityp1, 2) == 0);
  assert (b2.getData<int> (ityp2, 2) == 0);
  assert (b2.getData<int> (ityp3, 2) == 0);
  assert (b2.getData<float> (ftyp, 2) == 0);
}


void test_copy_base_aux()
{
  std::cout << "test_copy_base_aux\n";
  SG::AuxVectorBase_test b1;
  SG::AuxStoreInternal store1;
  b1.initAuxVectorBase<C> (SG::OWN_ELEMENTS, SG::DEFAULT_TRACK_INDICES);
  b1.setStore (&store1);

  SG::auxid_t ityp1 = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt1");

  C pc;
  b1.setIndexTest (&pc, 0);
  b1.getData<int> (ityp1, 0) = 10;

  SG::AuxVectorBase_test b2;
  b2.initAuxVectorBase<B> (SG::OWN_ELEMENTS, SG::DEFAULT_TRACK_INDICES);
  SG::AuxStoreInternal store2;
  b2.setStore (&store2);
}


void test_clear_aux()
{
  std::cout << "test_clear_aux\n";
  SG::AuxVectorBase_test b1;
  SG::AuxStoreInternal store1;
  b1.initAuxVectorBase<B> (SG::OWN_ELEMENTS, SG::DEFAULT_TRACK_INDICES);
  b1.setStore (&store1);

  SG::auxid_t ityp1 = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt1");
  SG::auxid_t ityp2 = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt2");
  SG::auxid_t ftyp = SG::AuxTypeRegistry::instance().getAuxID<float> ("aFloat");
  b1.getData<int> (ityp1, 1) = 10;
  b1.getData<int> (ityp2, 1) = 20;
  b1.getData<float> (ftyp, 1) = 1.5;

  SG::AuxElement elt;
  b1.setIndexTest (&elt, 1);
  assert (elt.index() == 1);
  assert (elt.container() == &b1);

  SG::AuxVectorBase_test::clearAux (elt);

  assert (b1.getData<int> (ityp1, 3) == 0);
  assert (b1.getData<int> (ityp2, 3) == 0);
  assert (b1.getData<float> (ftyp, 3) == 0);
}


void test_swap_elements_aux()
{
  std::cout << "test_swap_elements_aux\n";

  SG::AuxVectorBase_test b1;
  SG::AuxStoreInternal store1;
  b1.initAuxVectorBase<C> (SG::OWN_ELEMENTS, SG::DEFAULT_TRACK_INDICES);
  b1.setStore (&store1);

  SG::AuxVectorBase_test b2;
  SG::AuxStoreInternal store2;
  b2.initAuxVectorBase<C> (SG::OWN_ELEMENTS, SG::DEFAULT_TRACK_INDICES);
  b2.setStore (&store2);

  SG::auxid_t ityp = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt1");
  SG::auxid_t ftyp = SG::AuxTypeRegistry::instance().getAuxID<float> ("aFloat");

  C* pc1 = new C;
  b1.setIndexTest (pc1, 1);
  b1.getData<int> (ityp, 1) = 10;
  b1.getData<float> (ftyp, 1) = 1.5;

  C* pc2 = new C;
  b2.setIndexTest (pc2, 2);
  b2.getData<int> (ityp, 2) = 20;
  b2.getData<float> (ftyp, 2) = 2.5;

  b1.swapElementsAux(1, 2, pc2, pc1, &b2);

  assert (pc1->index() == 2);
  assert (pc1->container() == &b2);
  assert (b1.getData<int> (ityp, 1) == 20);
  assert (b1.getData<float> (ftyp, 1) == 2.5);

  assert (pc2->index() == 1);
  assert (pc2->container() == &b1);
  assert (b2.getData<int> (ityp, 2) == 10);
  assert (b2.getData<float> (ftyp, 2) == 1.5);

  b2.swapElementsAux(2, 1, 0, pc1, &b1);

  assert (pc1->index() == 1);
  assert (pc1->container() == &b1);
  assert (b1.getData<int> (ityp, 1) == 10);
  assert (b1.getData<float> (ftyp, 1) == 1.5);

  assert (pc2->index() == 1);
  assert (pc2->container() == &b1);
  assert (b2.getData<int> (ityp, 2) == 0);
  assert (b2.getData<float> (ftyp, 2) == 0);

  b1.setIndexTest (pc1, 1);
  b1.getData<int> (ityp, 1) = 10;
  b1.getData<float> (ftyp, 1) = 1.5;
  b2.setIndexTest (pc2, 2);
  b2.getData<int> (ityp, 2) = 20;
  b2.getData<float> (ftyp, 2) = 2.5;

  b1.swapElementsAux(1, 2, pc2, 0, &b2);

  assert (pc1->index() == 1);
  assert (pc1->container() == &b1);
  assert (b1.getData<int> (ityp, 1) == 20);
  assert (b1.getData<float> (ftyp, 1) == 2.5);

  assert (pc2->index() == 1);
  assert (pc2->container() == &b1);
  assert (b2.getData<int> (ityp, 2) == 0);
  assert (b2.getData<float> (ftyp, 2) == 0);
}


template <class T, class BASE>
void test_resort_aux_check (SG::AuxVectorBase_test& b,
                            std::vector<BASE*>& v,
                            bool checkaux = true)
{
  SG::auxid_t ityp = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt");
  SG::auxid_t ftyp = SG::AuxTypeRegistry::instance().getAuxID<float> ("aFloat");

  for (size_t i = 0; i < v.size(); i++) {
    T* vv = static_cast<T*> (v[i]);
    assert (vv->container() == &b);
    assert (vv->index() == i);
    if (checkaux) {
      assert (b.getData<int>(ityp, i) == v[i]->x);
      assert (b.getData<float>(ftyp, i) == v[i]->x+0.5);
    }
  }
}
template <class T, class CONT>
void test_resort_aux1()
{
  //typedef typename CONT::base_type BASE;
  typedef T BASE;
  Athena_test::RNG stlrand;
  stlrand.seed = 1;

  SG::auxid_t ityp = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt");
  SG::auxid_t ftyp = SG::AuxTypeRegistry::instance().getAuxID<float> ("aFloat");

  std::vector<BASE*> v = make_v2<BASE, T>();
  SG::AuxVectorBase_test b;
  b.initAuxVectorBase<T> (SG::OWN_ELEMENTS, SG::DEFAULT_TRACK_INDICES);
  for (size_t i = 0; i < v.size(); i++)
    b.setIndexTest (static_cast<T*>(v[i]), i);
  test_resort_aux_check<T,BASE> (b, v, false);
  SG::AuxStoreInternal store;
  b.setStore (&store);
  for (size_t i = 0; i < v.size(); i++) {
    b.getData<int> (ityp, i) = v[i]->x;
    b.getData<float> (ftyp, i) = v[i]->x + 0.5;
  }

  for (int jj=0; jj<10; jj++) {
    std::random_shuffle (v.begin(), v.end(), stlrand);
    b.resortAux (0, v.begin(), v.end());
    test_resort_aux_check<T,BASE> (b, v);
  }
  for (int jj=0; jj<10; jj++) {
    std::random_shuffle (v.begin()+2, v.end()-2, stlrand);
    b.resortAux (2, v.begin()+2, v.end()-2);
    test_resort_aux_check<T,BASE> (b, v);
  }

  std::vector<BASE*> v2 = v;
  SG::AuxVectorBase_test b2;
  b2.initAuxVectorBase<T> (SG::VIEW_ELEMENTS, SG::DEFAULT_TRACK_INDICES);
  for (int jj=0; jj<10; jj++) {
    std::random_shuffle (v2.begin(), v2.end(), stlrand);
    b2.resortAux (0, v.begin(), v.end());
    test_resort_aux_check<T,BASE> (b, v);
  }

  b.setStore((SG::IAuxStore*)0);
  for (int jj=0; jj<10; jj++) {
    std::random_shuffle (v.begin(), v.end(), stlrand);
    b.resortAux (0, v.begin(), v.end());
    test_resort_aux_check<T,BASE> (b, v, false);
  }
}
void test_resort_aux()
{
  std::cout << "test_resort_aux\n";

  std::vector<A*> va = make_v<A>();
  SG::AuxVectorBase_test b1;
  b1.initAuxVectorBase<A> (SG::OWN_ELEMENTS, SG::DEFAULT_TRACK_INDICES);
  b1.setIndices (va.begin(), va.end(), 0);
  b1.resortAux (0, va.begin(), va.end());

  test_resort_aux1<B, BCont>();
  test_resort_aux1<C, CCont>();
}


void test_move()
{
  std::cout << "test_move\n";

  SG::AuxVectorBase_test b1;
  b1.initAuxVectorBase<B> (SG::OWN_ELEMENTS, SG::DEFAULT_TRACK_INDICES);
  SG::AuxStoreInternal store;
  b1.setStore (&store);
  SG::auxid_t ityp = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt");
  b1.getData<int> (ityp, 0) = 1;
  b1.getData<int> (ityp, 1) = 2;
  assert (b1.getData<int> (ityp, 0) == 1);
  assert (b1.getData<int> (ityp, 1) == 2);

  SG::AuxVectorBase_test b2 (std::move (b1));
  assert (b1.getConstStore() == 0);
  assert (b1.getStore() == 0);
  assert (b2.getConstStore() == &store);
  assert (b2.getStore() == &store);
  assert (b2.getData<int> (ityp, 0) == 1);
  assert (b2.getData<int> (ityp, 1) == 2);

  SG::AuxVectorBase_test b3;
  b3 = std::move (b2);
  assert (b2.getConstStore() == 0);
  assert (b2.getStore() == 0);
  assert (b3.getConstStore() == &store);
  assert (b3.getStore() == &store);
  assert (b3.getData<int> (ityp, 0) == 1);
  assert (b3.getData<int> (ityp, 1) == 2);
}


int main()
{
  test1();
  test_init();
  test_set_store();
  test_set_store2();
  test_set_indices();
  test_clear_index();
  test_clear_indices();
  test_get_data();
  test_reserve_resize();
  test_shift();
  test_get_types();
  test_copy_aux();
  test_copy_base_aux();
  test_clear_aux();
  test_swap_elements_aux();
  test_resort_aux();
  test_move();
  return 0;
}
