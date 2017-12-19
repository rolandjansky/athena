/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/test/AuxElement_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2013
 * @brief Regression tests for AuxElement
 */

#undef NDEBUG
#include "AthContainers/AuxElement.h"
#include "AthContainers/AuxStoreInternal.h"
#include "AthContainers/exceptions.h"
#include "TestTools/expect_exception.h"
#include "CxxUtils/make_unique.h"
#include <iostream>
#include <cassert>
#include <memory>
#include <vector>
#include <map>


#include "auxid_set_equal.icc"


namespace SG {

class AuxVectorBase
  : public SG::AuxVectorData
{
public:
  virtual size_t size_v() const { return 10; }
  virtual size_t capacity_v() const { return 10; }

  using SG::AuxVectorData::setStore;
  void set (SG::AuxElement& b, size_t index)
  {
    b.setIndex (index, this);
  }
  void clear (SG::AuxElement& b)
  {
    b.setIndex (0, 0);
  }

  static
  void clearAux (SG::AuxElement& b)
  {
    b.clearAux();
  }

  static
  void copyAux (SG::AuxElement& a, const SG::AuxElement& b)
  {
    a.copyAux (b);
  }

  static
  void testAuxElementCtor (SG::AuxVectorData* container,
                           size_t index)
  {
    SG::AuxElement bx (container, index);
    assert (bx.index() == index);
    assert (bx.container() == container);
  }
};


} // namespace SG


class ConstAuxStoreTest
  : public SG::IConstAuxStore
{
public:
  virtual const void* getData (SG::auxid_t auxid) const;
  virtual const SG::auxid_set_t& getAuxIDs() const { return m_set; }
  virtual void* getDecoration (SG::auxid_t /*auxid*/, size_t /*size*/, size_t /*capacity*/) { std::abort(); }
  virtual void lock() { std::abort(); }
  virtual bool clearDecorations() { std::abort(); }
  virtual size_t size() const { std::abort(); }
  virtual void lockDecoration (SG::auxid_t) { std::abort(); }

  void add (SG::auxid_t auxid, std::unique_ptr<std::vector<float> > vec);

  SG::auxid_set_t m_set;
  typedef std::vector<float> vec_t;
  typedef std::map<SG::auxid_t, std::unique_ptr<vec_t> > map_t;
  map_t m_vecs;
};


const void* ConstAuxStoreTest::getData (SG::auxid_t auxid) const
{
  map_t::const_iterator it = m_vecs.find (auxid);
  if (it != m_vecs.end())
    return it->second->data();
  return 0;
}

void ConstAuxStoreTest::add (SG::auxid_t auxid,
                             std::unique_ptr<std::vector<float> > vec)
{
  m_vecs[auxid] = std::move(vec);
}



void test1()
{
  std::cout << "test1\n";

  SG::AuxElement::ConstAccessor<int> ityp1_c ("anInt");
  SG::AuxElement::Accessor<int> ityp1 ("anInt");
  SG::AuxElement::Accessor<float> ftyp1 ("aFloat");

  SG::AuxElement b;
  assert (b.index() == 0);
  assert (b.container() == 0);
  assert (b.getAuxIDs().empty());

  assert (!ityp1.isAvailable(b));
  assert (!ityp1_c.isAvailable(b));
  assert (!ityp1.isAvailableWritable(b));

  SG::AuxVectorBase v;
  v.set (b, 5);
  assert (b.index() == 5);
  assert (b.container() == &v);

  assert (!ityp1.isAvailable(b));
  assert (!ityp1.isAvailableWritable(b));

  SG::AuxVectorBase::testAuxElementCtor (&v, 10);

  SG::AuxStoreInternal store;
  v.setStore (&store);

  assert (!ityp1.isAvailable(b));
  assert (!ftyp1.isAvailable(b));
  assert (!ityp1.isAvailableWritable(b));
  assert (!ftyp1.isAvailableWritable(b));

  ityp1(b) = 3;
  ftyp1(b) = 1.5;
  assert (ityp1(b) == 3);
  assert (ityp1_c(b) == 3);
  assert (ftyp1(b) == 1.5);
  assert (b.auxdata<int>("anInt") == 3);
  assert (b.auxdataConst<int>("anInt") == 3);
  b.auxdata<int>("anInt") = 1;
  assert (ityp1(b) == 1);

  SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
  SG::auxid_t ityp1_id = r.getAuxID<int> ("anInt");
  SG::auxid_t ftyp1_id = r.getAuxID<float> ("aFloat");
  SG::auxid_set_t auxids;
  auxids.insert (ityp1_id);
  auxids.insert (ftyp1_id);
  assert (b.getAuxIDs() == auxids);

  assert (ityp1_c.auxid() == ityp1_id);
  assert (ityp1.auxid() == ityp1_id);
  assert (ftyp1.auxid() == ftyp1_id);

  assert (ityp1(v, 5) == 1);
  ityp1(v, 5) = 2;
  assert (ityp1(v, 5) == 2);
  assert (ityp1.getDataArray(v)[5] == 2);
  ityp1.getDataArray(v)[5] = 1;
  assert (ityp1.getDataArray(v)[5] == 1);

  assert (ityp1.isAvailable(b));
  assert (ftyp1.isAvailable(b));
  assert (ityp1.isAvailableWritable(b));
  assert (ftyp1.isAvailableWritable(b));

  const SG::AuxElement& cb = b;
  assert (ityp1(cb) == 1);
  assert (ftyp1(cb) == 1.5);

  const SG::AuxVectorBase& cv = v;
  assert (ityp1(cv, 5) == 1);
  assert (ityp1.getDataArray(cv)[5] == 1);

  SG::AuxElement b2 = b;
  assert (b2.index() == 0);
  assert (b2.container() == 0);

  SG::AuxElement b3;
  SG::AuxVectorBase v3;
  v3.set (b3, 6);
  SG::AuxStoreInternal store3;
  v3.setStore (&store3);
  assert (b3.index() == 6);
  assert (b3.container() == &v3);

  b3 = b;
  assert (b3.index() == 6);
  assert (b3.container() == &v3);
  assert (ityp1(b3) == 1);
  assert (ftyp1(b3) == 1.5);

  ityp1(b) = 21;
  b3 = std::move(b);
  assert (b3.index() == 6);
  assert (b3.container() == &v3);
  assert (ityp1(b3) == 21);
  assert (ftyp1(b3) == 1.5);

  SG::AuxElement b4 = std::move(b);
  assert (b4.index() == 0);
  assert (b4.container() == 0);

  ityp1.set (b3, 22);
  assert (ityp1(b3) == 22);

  v.setStore ((SG::IConstAuxStore*)&store);
  assert (ftyp1(cb) == 1.5);

  SG::AuxElement::TypelessConstAccessor ftyp1a ("aFloat");
  assert (ftyp1a.isAvailable (b3));
  assert (*reinterpret_cast<const float*>(ftyp1a (b3)) == 1.5);
  assert (*reinterpret_cast<const float*>(ftyp1a (v3, 6)) == 1.5);
  assert ((reinterpret_cast<const float*>(ftyp1a.getDataArray (v3)))[6] == 1.5);

  assert (ftyp1a.auxid() == ftyp1_id);

  EXPECT_EXCEPTION (SG::ExcUnknownAuxItem,
                    SG::AuxElement::TypelessConstAccessor ("adsasd"));
  SG::AuxElement::TypelessConstAccessor x1 (typeid(int), "adsasd");
  EXPECT_EXCEPTION (SG::ExcUnknownAuxItem,
                    SG::AuxElement::TypelessConstAccessor (typeid(SG::AuxVectorBase),
                                                           "x2"));
}


class Elt
  : public SG::AuxElement
{
public:
  Elt() : x(0) { makePrivateStore(); }
  Elt(int the_x) : x(the_x) { makePrivateStore(); }
  Elt(const Elt& other)
    : SG::AuxElement(other),
      x(other.x)
  {
    makePrivateStore(other);
  }

  Elt(const Elt* other)
    : SG::AuxElement(*other),
      x(other->x)
  {
    makePrivateStore(other);
  }

  Elt& operator= (const Elt&) = default;

  int& ityp1()
  {
    static const Accessor<int> acc ("anInt");
    return acc (*this);
  }

  int ityp1() const
  {
    static const Accessor<int> acc ("anInt");
    return acc (*this);
  }

  float& ftyp1()
  {
    static const Accessor<float> acc ("aFloat");
    return acc (*this);
  }

  float ftyp1() const
  {
    static const Accessor<float> acc ("aFloat");
    return acc (*this);
  }

  int x;
};


void test2()
{
  std::cout << "test2\n";
  Elt elt;
  assert (elt.x == 0);
  assert (elt.usingPrivateStore());

  Elt elt2 (10);
  assert (elt2.x == 10);

  Elt::Accessor<int> ityp2 ("anotherInt");

  elt.ityp1() = 1;
  ityp2(elt) = 2;
  elt.ftyp1() = 1.5;

  assert (elt.ityp1() == 1);
  assert (ityp2(elt) == 2);
  assert (elt.ftyp1() == 1.5);

  SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
  SG::auxid_t ityp1_id = r.getAuxID<int> ("anInt");
  SG::auxid_t ftyp1_id = r.getAuxID<float> ("aFloat");
  SG::auxid_t ityp2_id = r.getAuxID<int> ("anotherInt");
  SG::auxid_set_t auxids;
  auxids.insert (ityp1_id);
  auxids.insert (ftyp1_id);
  auxids.insert (ityp2_id);
  assert (elt.getAuxIDs() == auxids);

  elt.x = 20;
  Elt elt3 (elt);
  assert (elt3.x == 20);
  assert (elt3.ityp1() == 1);
  assert (ityp2(elt3) == 2);
  assert (elt3.ftyp1() == 1.5);
  assert (elt3.container() != elt.container());

  Elt elt3a (&elt);
  assert (elt3a.x == 20);
  assert (elt3a.ityp1() == 1);
  assert (ityp2(elt3a) == 2);
  assert (elt3a.ftyp1() == 1.5);
  assert (elt3a.container() != elt.container());

  elt.ityp1() = 31;
  assert (elt3.ityp1() == 1);
  elt3 = elt;
  assert (elt3.ityp1() == 31);
  assert (elt3.container() != elt.container());

  elt.ityp1() = 41;
  assert (elt3.ityp1() == 31);
  elt3 = std::move(elt);
  assert (elt3.ityp1() == 41);
  assert (elt3.container() != elt.container());

  Elt elt4 (std::move(elt));
  assert (elt4.x == 20);
  assert (elt4.ityp1() == 41);
  assert (ityp2(elt4) == 2);
  assert (elt4.ftyp1() == 1.5);
  assert (elt4.container() != elt.container());

  elt3.ityp1() = 41;
  elt.releasePrivateStore();
  assert (!elt.usingPrivateStore());
  assert (elt.container() == 0);
  assert (elt3.x == 20);
  assert (elt3.ityp1() == 41);
  assert (ityp2(elt3) == 2);
  assert (elt3.ftyp1() == 1.5);

  SG::AuxElement elt5;
  elt5.makePrivateStore();
  EXPECT_EXCEPTION (SG::ExcBadPrivateStore, elt5.makePrivateStore());

  elt5.releasePrivateStore();
  EXPECT_EXCEPTION (SG::ExcBadPrivateStore, elt5.releasePrivateStore());
}


void test_clear()
{
  std::cout << "test_clear\n";
  Elt elt1;
  assert (elt1.usingPrivateStore());
  elt1.ityp1() = 1;
  elt1.ftyp1() = 1.5;

  assert (elt1.ityp1() == 1);
  assert (elt1.ftyp1() == 1.5);

  SG::AuxVectorBase::clearAux (elt1);
  assert (elt1.ityp1() == 0);
  assert (elt1.ftyp1() == 0);

  elt1.releasePrivateStore();
  SG::AuxVectorBase::clearAux (elt1);

  SG::AuxVectorBase dv;
  dv.set (elt1, 1);
  ConstAuxStoreTest store;
  dv.setStore (&store);

  EXPECT_EXCEPTION (SG::ExcConstAuxData, SG::AuxVectorBase::clearAux (elt1));

  dv.setStore ((SG::IConstAuxStore*)0);
  SG::AuxVectorBase::clearAux (elt1);
}


void test_copy()
{
  std::cout << "test_copy\n";

  Elt::Accessor<int> ityp2 ("anotherInt");

  Elt elt1;
  Elt elt2;
  SG::AuxVectorBase dv1;
  dv1.set (elt1, 1);
  dv1.set (elt2, 2);
  SG::AuxStoreInternal store1;
  dv1.setStore (&store1);

  elt1.ityp1() = 3;
  elt1.ftyp1() = 3.5;
  ityp2(elt1) = 4;

  elt2.ityp1() = 7;
  elt2.ftyp1() = 7.5;
  ityp2(elt2) = 5;

  Elt elt3;
  SG::AuxVectorBase dv3;
  dv3.set (elt3, 3);
  SG::AuxStoreInternal store3;
  dv3.setStore (&store3);

  elt3.ityp1() = 10;
  elt3.ftyp1() = 10.5;

  SG::AuxVectorBase::copyAux (elt2, elt3);
  assert (elt2.ityp1() == 10);
  assert (elt2.ftyp1() == 10.5);
  assert (ityp2(elt2) == 0);

  const Elt& celt2 = elt2;
  Elt::Accessor<int> ityp3 ("yetAnotherInt");
  Elt elt5;
  SG::AuxVectorBase dv5;
  dv5.set (elt5, 1);
  ConstAuxStoreTest store5;
  dv5.setStore (&store5);
  SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
  SG::auxid_t ityp1_id = r.getAuxID<int> ("anInt");
  SG::auxid_t ftyp1_id = r.getAuxID<float> ("aFloat");
  SG::auxid_t ityp3_id = ityp3.auxid();
  store5.m_set.insert (ityp1_id);
  store5.m_set.insert (ityp3_id);
  store5.m_set.insert (ftyp1_id);
  auto vptr = CxxUtils::make_unique<std::vector<float> >();
  vptr->resize(5);
  (*vptr)[1] = 3.5;
  store5.add (ftyp1_id, std::move(vptr));
  EXPECT_EXCEPTION (SG::ExcBadAuxVar, ityp3(celt2));
  SG::AuxVectorBase::copyAux (elt2, elt5);
  assert (elt2.ityp1() == 0);
  assert (elt2.ftyp1() == 3.5);
  assert (ityp3(celt2) == 0);

  Elt elt4;
  elt4.releasePrivateStore();
  SG::AuxVectorBase::copyAux (elt1, elt4);
  assert (elt1.ityp1() == 0);
  assert (elt1.ftyp1() == 0);
  assert (ityp2(elt1) == 0);

  ConstAuxStoreTest store;
  dv1.setStore (&store);

  EXPECT_EXCEPTION(SG::ExcConstAuxData, SG::AuxVectorBase::copyAux(elt1, elt4));

  dv1.setStore ((SG::IConstAuxStore*)0);
  SG::AuxVectorBase::copyAux (elt1, elt4);
  SG::AuxVectorBase::copyAux (elt4, elt1);
}


void test_standalone()
{
  std::cout << "test_standalone\n";
  SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
  SG::auxid_t ityp1 = r.getAuxID<int> ("anInt");
  SG::AuxElement::Accessor<int> int1 ("anInt");
  SG::AuxStoreInternal store1;
  int* i1 = reinterpret_cast<int*> (store1.getData(ityp1, 1, 1));
  i1[0] = 11;

  SG::AuxVectorBase v;

  SG::AuxElement elt1;
  elt1.clearCache();
  assert (!elt1.usingPrivateStore());
  assert (!elt1.usingStandaloneStore());
  assert (!elt1.hasStore());
  assert (!elt1.hasNonConstStore());
  assert (elt1.getStore() == 0);
  assert (elt1.getConstStore() == 0);

  elt1.setStore (&store1);
  elt1.clearCache();
  assert (!elt1.usingPrivateStore());
  assert (elt1.usingStandaloneStore());
  assert (elt1.hasStore());
  assert (elt1.hasNonConstStore());
  assert (elt1.getStore() == &store1);
  assert (elt1.getConstStore() == &store1);
  assert (int1(elt1) == 11);
  int1(elt1) = 12;

  EXPECT_EXCEPTION (SG::ExcBadPrivateStore, elt1.makePrivateStore());
  EXPECT_EXCEPTION (SG::ExcBadPrivateStore, v.set(elt1, 2));

  elt1.setStore ((SG::IAuxStore*)0);
  assert (!elt1.usingPrivateStore());
  assert (!elt1.usingStandaloneStore());
  assert (!elt1.hasStore());
  assert (!elt1.hasNonConstStore());
  assert (elt1.getStore() == 0);
  assert (elt1.getConstStore() == 0);
  assert (i1[0] == 12);

  v.set (elt1, 1);

  EXPECT_EXCEPTION (SG::ExcBadPrivateStore, elt1.setStore (&store1));
  EXPECT_EXCEPTION (SG::ExcBadPrivateStore, elt1.setStore ((SG::IAuxStore*)0));

  assert (!elt1.usingPrivateStore());
  assert (!elt1.usingStandaloneStore());
  assert (!elt1.hasStore());
  assert (!elt1.hasNonConstStore());
  assert (elt1.getStore() == 0);
  assert (elt1.getConstStore() == 0);

  SG::AuxElement elt2;
  elt2.makePrivateStore();

  EXPECT_EXCEPTION (SG::ExcBadPrivateStore, elt1.setStore (&store1));
  EXPECT_EXCEPTION (SG::ExcBadPrivateStore, elt1.setStore ((SG::IAuxStore*)0));

  assert (elt2.usingPrivateStore());
  assert (!elt2.usingStandaloneStore());
  assert (elt2.hasStore());
  assert (elt2.hasNonConstStore());
  assert (elt2.getStore() != 0);
  assert (elt2.getConstStore() != 0);

  elt2.releasePrivateStore();
  elt2.setNonConstStore (&store1);
  assert (elt2.getStore() == &store1);
  assert (elt2.getConstStore() == &store1);
  elt2.setConstStore (&store1);
  assert (elt2.getStore() == 0);
  assert (elt2.getConstStore() == &store1);
}


void test_decoration()
{
  std::cout << "test_decoration\n";

  SG::AuxElement b;
  SG::AuxVectorBase v;
  v.set (b, 5);

  SG::AuxStoreInternal store;
  v.setStore (&store);

  SG::AuxElement::Accessor<int>  ityp1 ("anInt1");
  SG::AuxElement::Decorator<int> ityp2 ("anInt2");

  SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
  SG::auxid_t ityp2_id = r.getAuxID<int> ("anInt2");
  assert (ityp2.auxid() == ityp2_id);

  ityp1(b) = 10;
  ityp2(b) = 11;

  assert (10 == ityp1(b));
  assert (11 == ityp2(b));

  v.lock();
  const SG::AuxElement& cb = b;

  SG::AuxElement::Decorator<int> ityp3 ("anInt3");
  ityp3(cb) = 12;
  assert (ityp3.getDecorationArray (v)+5 == &ityp3(cb));

  assert (10 == ityp1(cb));
  assert (12 == ityp3(cb));

  cb.auxdecor<int> ("anInt3") = 19;
  assert (19 == ityp3(cb));

  EXPECT_EXCEPTION (SG::ExcStoreLocked, ityp2(cb) = 14);

#if 0
  assert (ityp3.isAvailable(cb));
  assert (cb.clearDecorations());
  assert (ityp1.isAvailable(cb));
  assert (!ityp3.isAvailable(cb));
#endif

  SG::AuxElement::Accessor<int>  ityp4 ("anInt4");

  SG::AuxElement b2;
  b2.makePrivateStore();
  ityp1(b2) = 10;

  b2.getStore()->lock();
  EXPECT_EXCEPTION (SG::ExcStoreLocked, ityp4(b2) = 12);
  assert (ityp1.isAvailable(b2));
  assert (!ityp4.isAvailable(b2));
  assert (ityp1.isAvailableWritable(b2));

  const SG::AuxElement& cb2 = b2;

  ityp3(cb2) = 14;
  assert (ityp3(cb2) == 14);
  assert (ityp3.isAvailable(b2));

  ityp3.set(cb2, 15);
  assert (ityp3(cb2) == 15);
  assert (ityp3.isAvailable(b2));

  assert (cb2.clearDecorations() == true);
  assert (ityp1.isAvailable(b2));
  assert (!ityp3.isAvailable(b2));

  assert (cb2.clearDecorations() == false);
  assert (ityp1.isAvailable(b2));
  assert (!ityp3.isAvailable(b2));
}


void test_private_store()
{
  std::cout << "test_private_store\n";
  Elt elt;
  assert (elt.hasStore());
  assert (elt.hasNonConstStore());
  assert (elt.usingPrivateStore());
  assert (!elt.usingStandaloneStore());

  SG::AuxVectorBase v;
  v.set (elt, 1);
  assert (!elt.hasStore());
  assert (!elt.hasNonConstStore());
  assert (!elt.usingPrivateStore());
  assert (!elt.usingStandaloneStore());

  v.clear (elt);
  assert (elt.hasStore());
  assert (elt.hasNonConstStore());
  assert (elt.usingPrivateStore());
  assert (!elt.usingStandaloneStore());
}


int main()
{
  test1();
  test2();
  test_clear();
  test_copy();
  test_standalone();
  test_decoration();
  test_private_store();
  return 0;
}

//  LocalWords:  anInt4
