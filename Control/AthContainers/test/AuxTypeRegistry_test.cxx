/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/test/AuxBaseRegistry_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2013
 * @brief Regression tests for AuxBaseRegistry
 */


#undef NDEBUG
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/exceptions.h"
#include "AthLinks/ElementLink.h"
#include "CxxUtils/checker_macros.h"
#include <iostream>
#include <cassert>


#ifndef XAOD_STANDALONE
#include "SGTools/TestStore.h"
#include "SGTools/StringPool.h"
#include "SGTools/CLASS_DEF.h"
CLASS_DEF (std::vector<int*>, 28374627, 0)
using namespace SGTest;


class TestStringPool
  : public IStringPool
{
public:
  virtual
  sgkey_t stringToKey (const std::string& str, CLID clid) override
  {
    return m_pool.stringToKey (str, clid);
  }

  virtual
  const std::string* keyToString (sgkey_t key) const override
  {
    return m_pool.keyToString (key);
  }

  virtual
  const std::string* keyToString (sgkey_t key,
                                  CLID& clid) const override
  {
    return m_pool.keyToString (key, clid);
  }

  virtual
  void registerKey (sgkey_t key,
                    const std::string& str,
                    CLID clid) override
  {
    m_pool.registerKey (key, str, clid);
  }

  SG::StringPool m_pool;
};
#else
namespace SGTest {
class TestStore {};
}
#endif // not XAOD_STANDALONE


struct Payload
{
  Payload (int x = 0) : m_x (x) {}
  Payload& operator= (const Payload&) = default;
  int m_x;
  bool operator== (const Payload& other)
  { return m_x == other.m_x; }
};


template <class T>
T makeT(int x=0) { return T(x); }

bool makeT(int x=0) { return (x&1) != 0; }


template <class T>
void test_type(const std::string& typname,
               const std::string& name,
               const std::string& clsname = "")
{
  SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
  assert (SG::null_auxid == r.findAuxID (name, clsname));
  SG::auxid_t auxid = r.getAuxID<T> (name, clsname);
  assert (auxid == r.getAuxID<T> (name, clsname));
  assert (auxid == r.findAuxID (name, clsname));
  assert (auxid == r.getAuxID (typeid(T), name, clsname));

  bool caught = false;
  try {
    r.getAuxID<char> (name, clsname);
  }
  catch (const SG::ExcAuxTypeMismatch& m) {
    caught = true;
  }
  assert (caught);

  r.getAuxID<char> (name, "otherclass");

  assert (r.getName (auxid) == name);
  assert (r.getClassName (auxid) == clsname);
  assert (r.getType (auxid) == &typeid(T));
  assert (r.getTypeName (auxid) == typname);
  if (typeid(T) == typeid(bool)) {
    assert (r.getVecType (auxid) == &typeid(std::vector<char>));
    assert (r.getVecTypeName (auxid) == "std::vector<char>");
    assert (r.getEltSize (auxid) == sizeof(char));
  }
  else {
    assert (r.getVecType (auxid) == &typeid(std::vector<T>));
    assert (r.getVecTypeName (auxid) == "std::vector<" + typname + ">");
    assert (r.getEltSize (auxid) == sizeof(T));
  }

  assert (r.getName (999) == "");
  assert (r.getType (999) == 0);
  assert (r.getTypeName (999) == "");
  assert (r.getVecTypeName (999) == "");

  SG::IAuxTypeVector* v = r.makeVector (auxid, 10, 20);
  T* ptr = reinterpret_cast<T*> (v->toPtr());
  ptr[0] = makeT(0);
  ptr[1] = makeT(1);

  v->reserve (50);
  ptr = reinterpret_cast<T*> (v->toPtr());
  v->resize (40);
  assert (ptr == reinterpret_cast<T*> (v->toPtr()));
  ptr[49] = makeT(123);

  SG::IAuxTypeVector* v2 = r.makeVector (auxid, 10, 20);
  T* ptr2 = reinterpret_cast<T*> (v2->toPtr());
  r.copy (auxid, ptr2, 0, ptr, 1);
  r.copyForOutput (auxid, ptr2, 1, ptr, 0);

  assert (ptr2[0] == makeT(1));
  assert (ptr2[1] == makeT(0));

  r.clear (auxid, ptr2, 0);
  assert (ptr2[0] == makeT());
  assert (ptr2[1] == makeT(0));

  ptr2[0] = makeT(10);
  ptr2[1] = makeT(11);
  r.swap (auxid, ptr, 0, ptr2, 1);
  assert (ptr[0] == makeT(11));
  assert (ptr[1] == makeT(1));
  assert (ptr2[0] == makeT(10));
  assert (ptr2[1] == makeT(0));

  SG::IAuxTypeVector* v3 = r.makeVector (auxid, 10, 10);
  ptr = reinterpret_cast<T*> (v3->toPtr());
  for (int i=0; i<10; i++)
    ptr[i] = makeT(i+1);

  v3->shift (5, 3);
  // 1 2 3 4 5 0 0 0 6 7 8 9 10
  ptr = reinterpret_cast<T*> (v3->toPtr());
  for (int i=0; i<5; i++)
    assert (ptr[i] == makeT(i+1));
  for (int i=5; i<8; i++)
    assert (ptr[i] == makeT(0));
  for (int i=8; i<13; i++)
    assert (ptr[i] == makeT(i-2));

  v3->shift (3, -2);
  // 1 4 5 0 0 0 6 7 8 9 10
  ptr = reinterpret_cast<T*> (v3->toPtr());
  assert (ptr[0] == makeT(1));
  for (int i=1; i<3; i++)
    assert (ptr[i] == makeT(i+3));
  for (int i=3; i<6; i++)
    assert (ptr[i] == makeT(0));
  for (int i=6; i<11; i++)
    assert (ptr[i] == makeT(i));

  delete v;
  delete v2;
  delete v3;
}


template <class T>
void test_type_extlock(const std::string& typname,
                       const std::string& name,
                       const std::string& clsname = "")
{
  SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
  SG::AuxTypeRegistry::lock_t lock (r);

  SG::auxid_t auxid = r.getAuxID<T> (name, clsname);

  assert (r.getName (lock, auxid) == name);
  assert (r.getClassName (lock, auxid) == clsname);
  assert (r.getType (lock, auxid) == &typeid(T));
  assert (r.getTypeName (lock, auxid) == typname);
  if (typeid(T) == typeid(bool)) {
    assert (r.getVecType (lock, auxid) == &typeid(std::vector<char>));
    assert (r.getVecTypeName (lock, auxid) == "std::vector<char>");
  }
  else {
    assert (r.getVecType (lock, auxid) == &typeid(std::vector<T>));
    assert (r.getVecTypeName (lock, auxid) == "std::vector<" + typname + ">");
  }

  assert (r.getName (lock, 999) == "");
  assert (r.getType (lock, 999) == 0);
  assert (r.getTypeName (lock, 999) == "");
  assert (r.getVecTypeName (lock, 999) == "");

  SG::IAuxTypeVector* v = r.makeVector (lock, auxid, 10, 20);
  T* ptr = reinterpret_cast<T*> (v->toPtr());
  ptr[0] = makeT(0);
  ptr[1] = makeT(1);

  v->reserve (50);
  ptr = reinterpret_cast<T*> (v->toPtr());
  v->resize (40);
  assert (ptr == reinterpret_cast<T*> (v->toPtr()));
  ptr[49] = makeT(123);

  SG::IAuxTypeVector* v2 = r.makeVector (lock, auxid, 10, 20);
  T* ptr2 = reinterpret_cast<T*> (v2->toPtr());
  r.copy (lock, auxid, ptr2, 0, ptr, 1);
  r.copy (lock, auxid, ptr2, 1, ptr, 0);
  r.copyForOutput (lock, auxid, ptr2, 1, ptr, 0);

  assert (ptr2[0] == makeT(1));
  assert (ptr2[1] == makeT(0));

  r.clear (lock, auxid, ptr2, 0);
  assert (ptr2[0] == makeT());
  assert (ptr2[1] == makeT(0));

  ptr2[0] = makeT(10);
  ptr2[1] = makeT(11);
  r.swap (lock, auxid, ptr, 0, ptr2, 1);
  assert (ptr[0] == makeT(11));
  assert (ptr[1] == makeT(1));
  assert (ptr2[0] == makeT(10));
  assert (ptr2[1] == makeT(0));

  delete v;
  delete v2;
}


template <class T>
void test_makeVector (const std::string& name)
{
  SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
  SG::auxid_t auxid = r.getAuxID<T> (name);

  typedef typename SG::AuxTypeVectorHolder<T>::vector_type vector_type;
  vector_type* vec1 = new vector_type;
  vec1->push_back (makeT(1));
  vec1->push_back (makeT(2));
  vec1->push_back (makeT(3));
  SG::IAuxTypeVector* v1 = r.makeVectorFromData (auxid, vec1, false, true);
  assert (v1->size() == 3);
  T* ptr1 = reinterpret_cast<T*> (v1->toPtr());
  assert (ptr1[0] == makeT(1));
  assert (ptr1[1] == makeT(2));
  assert (ptr1[2] == makeT(3));

  SG::PackedContainer<T>* vec2 = new SG::PackedContainer<T>;
  vec2->push_back (makeT(3));
  vec2->push_back (makeT(2));
  vec2->push_back (makeT(1));
  SG::IAuxTypeVector* v2 = r.makeVectorFromData (auxid, vec2, true, true);
  assert (v2->size() == 3);
  T* ptr2 = reinterpret_cast<T*> (v2->toPtr());
  assert (ptr2[0] == makeT(3));
  assert (ptr2[1] == makeT(2));
  assert (ptr2[2] == makeT(1));

  delete v1;
  delete v2;
}


void test2()
{
  std::cout << "test2\n";
  test_type<int> ("int", "anInt");
  test_type<float> ("float", "aFloat");
  test_type<double> ("double", "aFloat", "xclass");
  test_type<bool> ("bool", "aBool");
  test_type<Payload> ("Payload", "aPayload");
  test_makeVector<int> ("anInt");

  test_type_extlock<int> ("int", "anInt");
}


void test_placeholder()
{
  std::cout << "test_placeholder\n";

  SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
  SG::auxid_t auxid = r.getAuxID<SG::AuxTypePlaceholder> ("placeholder");
  assert (r.findAuxID("placeholder") == auxid);
  assert (r.getType(auxid) == &typeid(SG::AuxTypePlaceholder));

  auxid = r.getAuxID<int> ("placeholder");
  assert (r.getType(auxid) == &typeid(int));
}


struct FacTest1 {};
struct FacTest1DynFac
  : public SG::AuxTypeVectorFactory<FacTest1>
{
public:
  virtual bool isDynamic() const { return true; }
};


void test_factories()
{
  std::cout << "test_factories\n";

  SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
  const SG::IAuxTypeVectorFactory* fac = r.getFactory (typeid(char));
  assert (fac->getEltSize() == sizeof(char));
  assert (fac->tiVec() == &typeid(std::vector<char>));

  assert (r.getFactory (typeid (FacTest1)) == 0);

  SG::IAuxTypeVectorFactory* fac1 = new FacTest1DynFac;
  r.addFactory (typeid (FacTest1), fac1);
  assert (r.getFactory (typeid (FacTest1)) == fac1);
  SG::IAuxTypeVectorFactory* fac2 = new SG::AuxTypeVectorFactory<FacTest1>;
  r.addFactory (typeid (FacTest1), fac2);
  assert (r.getFactory (typeid (FacTest1)) == fac2);
  SG::IAuxTypeVectorFactory* fac3 = new SG::AuxTypeVectorFactory<FacTest1>;
  r.addFactory (typeid (FacTest1), fac3);
  assert (r.getFactory (typeid (FacTest1)) == fac2);
}


struct FacTest2 {};
struct FacTest2DynFac
  : public SG::AuxTypeVectorFactory<FacTest2>
{
public:
  virtual bool isDynamic() const { return true; }
};


void test_factories_extlock()
{
  std::cout << "test_factories_extlock\n";

  SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();

  {
    SG::AuxTypeRegistry::lock_t lock (r);

    const SG::IAuxTypeVectorFactory* fac = r.getFactory (lock, typeid(char));
    assert (fac->getEltSize() == sizeof(char));
    assert (fac->tiVec() == &typeid(std::vector<char>));

    assert (r.getFactory (lock, typeid (FacTest2)) == 0);
  }
}


struct FacTest3 {};


void test_get_by_ti()
{
  std::cout << "test_get_by_ti\n";

  SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
  SG::auxid_t auxid = r.getAuxID (typeid(short), "aShort");
  assert (auxid != SG::null_auxid);
  assert (r.getType (auxid) == &typeid(short));
  assert (r.getTypeName (auxid) == "short");

  assert (r.getAuxID (typeid(FacTest3), "aTest3") == SG::null_auxid);
  r.addFactory (typeid(FacTest3), new SG::AuxTypeVectorFactory<FacTest3>);
  auxid = r.getAuxID (typeid(FacTest3), "aTest3");
  assert (auxid != SG::null_auxid);
  assert (r.getType (auxid) == &typeid(FacTest3));
  assert (r.getTypeName (auxid) == "FacTest3");
}


void test_copyForOutput (SGTest::TestStore& store)
{
  std::cout << "test_copyForOutput\n";

#ifndef XAOD_STANDALONE
  typedef ElementLink<std::vector<int*> > EL;
  EL el1 (123, 10);
  EL el2;

  SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
  SG::auxid_t auxid = r.getAuxID<EL> ("EL");
  SG::auxid_t auxid_v = r.getAuxID<std::vector<EL> > ("ELV");

  r.copyForOutput (auxid, &el2, 0, &el1, 0);
  assert (el2.key() == 123);
  assert (el2.index() == 10);

  std::vector<EL> v1;
  v1.push_back (EL (123, 5));
  v1.push_back (EL (123, 6));
  std::vector<EL> v2;
  r.copyForOutput (auxid_v, &v2, 0, &v1, 0);
  assert (v2[0].key() == 123);
  assert (v2[0].index() == 5);
  assert (v2[1].key() == 123);
  assert (v2[1].index() == 6);

  store.remap (123, 456, 10, 20);

  r.copyForOutput (auxid, &el2, 0, &el1, 0);
  assert (el2.key() == 456);
  assert (el2.index() == 20);

  store.remap (123, 456, 6, 12);
  r.copyForOutput (auxid_v, &v2, 0, &v1, 0);
  assert (v2[0].key() == 123);
  assert (v2[0].index() == 5);
  assert (v2[1].key() == 456);
  assert (v2[1].index() == 12);
#endif
}


#ifndef XAOD_STANDALONE
void addto_renameMap (Athena::IInputRename::InputRenameMap_t& map,
                      IStringPool& pool,
                      const char* from,
                      const char* to)
{
  CLID clid = 123;
  sgkey_t from_key = pool.stringToKey (from, clid);
  sgkey_t to_key   = pool.stringToKey (to, clid);
  map[from_key] = to_key;
}
#endif // NOT XAOD_STANDALONE


void test_renameMap()
{
  std::cout << "test_renameMap\n";

#ifndef XAOD_STANDALONE
  TestStringPool pool;
  Athena::IInputRename::InputRenameMap_t map;

  addto_renameMap (map, pool, "a", "a_renamed");
  addto_renameMap (map, pool, "b.c", "b.c_renamed");
  addto_renameMap (map, pool, "b.d", "b");

  SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
  r.setInputRenameMap (&map, pool);

  assert (r.inputRename ("a", "b") == "b");
  assert (r.inputRename ("b", "c") == "c_renamed");
#endif
}


int main ATLAS_NOT_THREAD_SAFE ()
{
#ifndef XAOD_STANDALONE
  initTestStore();
#endif

  test2();
  test_placeholder();
  test_factories();
  test_get_by_ti();
  test_copyForOutput (SGTest::store);
  test_renameMap();
  return 0;
}
