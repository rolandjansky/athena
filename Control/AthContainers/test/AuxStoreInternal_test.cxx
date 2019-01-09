/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/test/AuxStoreInternal_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2013
 * @brief Regression tests for AuxStoreInternal
 */


#undef NDEBUG
#include "AthContainers/AuxStoreInternal.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/exceptions.h"
#include "AthContainers/tools/AuxTypeVector.h"
#include "AthContainers/tools/threading.h"
#include "TestTools/expect_exception.h"
#include "CxxUtils/make_unique.h"
#ifndef ATHCONTAINERS_NO_THREADS
#include "boost/thread/shared_mutex.hpp"
#include "boost/thread/shared_lock_guard.hpp"
#endif
#include <iostream>
#include <sstream>
#include <cassert>


struct MoveTest
{
  MoveTest(int x=0) : m_v(x) {}
  MoveTest(const MoveTest& other): m_v (other.m_v) {}
  MoveTest(MoveTest&& other): m_v (std::move(other.m_v)) {}
  MoveTest& operator= (const MoveTest& other) {
    if (this != &other) m_v = other.m_v;
    return *this;
  }
  MoveTest& operator= (MoveTest&& other) {
    if (this != &other) m_v = std::move(other.m_v);
    return *this;
  }
  std::vector<int> m_v;
  bool operator== (const MoveTest& other) const { return m_v.size() == other.m_v.size(); }
};


bool wasMoved (const MoveTest& x) { return x.m_v.empty(); }


class AuxStoreInternalTest
  : public SG::AuxStoreInternal
{
public:
  using SG::AuxStoreInternal::addAuxID;
  using SG::AuxStoreInternal::addVector;
  using SG::AuxStoreInternal::getIODataInternal;
};


void test1()
{
  std::cout << "test1\n";
  AuxStoreInternalTest s;
  assert (!s.standalone());

  SG::auxid_t ityp1 = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt");
  assert (s.getData (ityp1) == 0);
  assert (s.size() == 0);

  int* i1 = reinterpret_cast<int*> (s.getData(ityp1, 10, 20));
  assert (s.size() == 10);
  i1[0] = 1;
  i1[1] = 2;

  const int* i1c = reinterpret_cast<const int*> (s.getData(ityp1));
  assert (i1c == i1);
  assert (i1c[1] == 2);

  SG::auxid_t ityp2 = SG::AuxTypeRegistry::instance().getAuxID<int> ("anotherInt");
  assert (s.getData (ityp2) == 0);
  int* i2 = reinterpret_cast<int*> (s.getData(ityp2, 10, 20));
  assert (i2 != i1);
  assert (i2[0] == 0);
  i2[0] = 11;
  i2[1] = 12;

  const int* i2c = reinterpret_cast<const int*> (s.getData(ityp2));
  assert (i2c == i2);
  assert (i2c[1] == 12);

  SG::auxid_t ftyp1 = SG::AuxTypeRegistry::instance().getAuxID<float> ("aFloat");
  assert (s.getData (ftyp1) == 0);

  float* f1 = reinterpret_cast<float*> (s.getData(ftyp1, 10, 20));
  f1[0] = 1.1;
  f1[1] = 2.5;

  const float* f1c = reinterpret_cast<const float*> (s.getData(ftyp1));
  assert (f1c == f1);
  assert (f1c[1] == 2.5);


  assert (s.size() == 10);

  s.reserve(50);
  assert (s.size() == 10);
  i2c = reinterpret_cast<const int*> (s.getData(ityp2));
  f1c = reinterpret_cast<const float*> (s.getData(ftyp1));
  assert (s.resize(40) == true);
  assert (s.size() == 40);
  assert (i2c == reinterpret_cast<const int*> (s.getData(ityp2)));
  assert (f1c == reinterpret_cast<const float*> (s.getData(ftyp1)));

  f1 = reinterpret_cast<float*> (s.getData(ftyp1, 40, 50));
  f1[39] = 5.5;

  s.shift (39, -37);
  assert (s.size() == 3);
  f1c = reinterpret_cast<const float*> (s.getData(ftyp1));
  assert (f1c[2] == 5.5);
  s.shift (1, 1);
  assert (s.size() == 4);
  i2c = reinterpret_cast<const int*> (s.getData(ityp2));
  assert (i2c[0] == 11);
  assert (i2c[1] == 0);
  assert (i2c[2] == 12);
  i1c = reinterpret_cast<const int*> (s.getData(ityp1));
  assert (i1c[0] == 1);
  assert (i1c[1] == 0);
  assert (i1c[2] == 2);

  const std::vector<int>* v1 =
    reinterpret_cast<const std::vector<int>*> (s.getIOData(ityp1));
  assert (&*v1->begin() == i1c);
  assert (s.getIOData(ityp1) == s.getIODataInternal(ityp1, false));

  const SG::auxid_set_t& ids = s.getAuxIDs();
  assert (ids.size() == 3);
  assert (ids.find (ityp1) != ids.end());
  assert (ids.find (ityp2) != ids.end());
  assert (ids.find (ftyp1) != ids.end());

  assert (s.getAuxIDs() == s.getWritableAuxIDs());

  const SG::auxid_set_t& dids = s.getDynamicAuxIDs();
  std::vector<SG::auxid_t> vi1 (ids.begin(), ids.end());
  std::vector<SG::auxid_t> vi2 (dids.begin(), dids.end());
  std::sort (vi1.begin(), vi1.end());
  std::sort (vi2.begin(), vi2.end());
  assert (vi1 == vi2);

  assert (s.getIOType(ityp1) == &typeid(std::vector<int>));

  SG::auxid_t btyp1 = SG::AuxTypeRegistry::instance().getAuxID<bool> ("aBool");
  s.getData(btyp1, 4, 50);
  assert (s.getIOType(btyp1) == &typeid(std::vector<char>));

  assert (s.getAuxIDs().size() == 4);
  s.addAuxID (999);
  const SG::auxid_set_t& ids2 = s.getAuxIDs();
  assert (ids2.size() == 5);
  assert (ids2.find (ityp1) != ids2.end());
  assert (ids2.find (ityp2) != ids2.end());
  assert (ids2.find (ftyp1) != ids2.end());
  assert (ids2.find (btyp1) != ids2.end());
  assert (ids2.find (999) != ids2.end());

  assert (s.size() == 4);

  SG::AuxStoreInternal s2 (s);
  assert (s2.size() == 4);

  const SG::auxid_set_t& ids3 = s2.getAuxIDs();
  assert (ids3.size() == 5);
  assert (ids3.find (ityp1) != ids2.end());
  assert (ids3.find (ityp2) != ids2.end());
  assert (ids3.find (ftyp1) != ids2.end());
  assert (ids3.find (btyp1) != ids2.end());
  assert (ids3.find (999) != ids2.end());

  i2c = reinterpret_cast<const int*> (s2.getData(ityp2));
  assert (i2c[0] == 11);
  assert (i2c[1] == 0);
  assert (i2c[2] == 12);
  i1c = reinterpret_cast<const int*> (s2.getData(ityp1));
  assert (i1c[0] == 1);
  assert (i1c[1] == 0);
  assert (i1c[2] == 2);

  assert (s.resize(1000) == false);
  assert (s.resize(500) == true);
  assert (s.resize(1000) == true);
}


// Test decorations / locking
void test2()
{
  std::cout << "test2\n";
  SG::AuxStoreInternal s;

  SG::auxid_t ityp1 = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt");
  SG::auxid_t ityp2 = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt2");
  SG::auxid_t ityp9 = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt9");

  int* i1 = reinterpret_cast<int*> (s.getData(ityp1, 10, 20));
  int* i2 = reinterpret_cast<int*> (s.getDecoration(ityp2, 10, 20));
  assert (i1 != 0);
  assert (i2 != 0);
  assert (i1 != i2);

  assert (!s.isDecoration (ityp1));
  assert (!s.isDecoration (ityp2));
  assert (!s.isDecoration (ityp9));

  s.lock();

  SG::auxid_set_t idset {ityp1, ityp2};
  assert (s.getAuxIDs() == idset);

  assert (i1 == s.getData(ityp1));
  //EXPECT_EXCEPTION (SG::ExcStoreLocked, s.getData(ityp1, 10, 20));
  assert (i1 == reinterpret_cast<int*> (s.getData(ityp1, 10, 20)));
  EXPECT_EXCEPTION (SG::ExcStoreLocked, s.getData(ityp9, 10, 20));
  EXPECT_EXCEPTION (SG::ExcStoreLocked, s.getDecoration(ityp1, 10, 20));

  assert (!s.isDecoration (ityp1));
  assert (!s.isDecoration (ityp2));
  assert (!s.isDecoration (ityp9));

  SG::auxid_t ityp3 = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt3");
  int* i3 = reinterpret_cast<int*> (s.getDecoration(ityp3, 10, 20));
  assert (i3 != 0);
  assert (i3 != i1);
  assert (i3 != i2);

  assert (i3 == s.getDecoration (ityp3, 10, 20));

  idset.insert (ityp3);
  assert (idset.size() == 3);
  assert (s.getAuxIDs() == idset);

  assert (!s.isDecoration (ityp1));
  assert (!s.isDecoration (ityp2));
  assert (!s.isDecoration (ityp9));
  assert ( s.isDecoration (ityp3));

  assert (s.clearDecorations() == true);
  idset.erase (ityp3);
  assert (idset.size() == 2);
  assert (s.getAuxIDs() == idset);
  assert (s.getData(ityp3) == 0);
  assert (s.getData(ityp1) == i1);
  assert (s.getData(ityp2) == i2);

  assert (!s.isDecoration (ityp1));
  assert (!s.isDecoration (ityp2));
  assert (!s.isDecoration (ityp9));
  assert (!s.isDecoration (ityp3));

  assert (s.clearDecorations() == false);
  assert (s.getAuxIDs() == idset);

  i3 = reinterpret_cast<int*> (s.getDecoration(ityp3, 10, 20));
  assert (i3 != 0);
  assert (i3 == s.getDecoration (ityp3, 10, 20));
  assert (!s.isDecoration (ityp1));
  assert (!s.isDecoration (ityp2));
  assert (!s.isDecoration (ityp9));
  assert ( s.isDecoration (ityp3));

  s.lockDecoration (ityp3);
  EXPECT_EXCEPTION (SG::ExcStoreLocked, s.getDecoration (ityp3, 10, 20));
  assert (i3 == s.getData (ityp3));

  assert (!s.isDecoration (ityp1));
  assert (!s.isDecoration (ityp2));
  assert (!s.isDecoration (ityp9));
  assert (!s.isDecoration (ityp3));

  EXPECT_EXCEPTION (SG::ExcStoreLocked, s.resize(100));
  EXPECT_EXCEPTION (SG::ExcStoreLocked, s.reserve(100));
  EXPECT_EXCEPTION (SG::ExcStoreLocked, s.shift(1, 2));
}


// Test setOption / getIOType
void test3()
{
  std::cout << "test3\n";
  SG::AuxStoreInternal s;
  SG::auxid_t ityp1 = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt");
  SG::auxid_t ityp2 = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt2");
  int* i1 = reinterpret_cast<int*> (s.getData(ityp1, 10, 20));
  i1[0] = 1;
  i1[1] = 2;

  assert (!s.setOption (ityp2, SG::AuxDataOption ("opt", 1)));
  assert (!s.setOption (ityp1, SG::AuxDataOption ("opt", 1)));
  assert (s.getIOType (ityp1) == &typeid(std::vector<int>));
  assert (s.setOption (ityp1, SG::AuxDataOption ("nbits", 29)));
  assert (s.getIOType (ityp1) == &typeid(SG::PackedContainer<int>));
  const SG::PackedContainer<int>* pvec =
    reinterpret_cast<const SG::PackedContainer<int>*> (s.getIOData (ityp1));
  assert (typeid(*pvec) == typeid(SG::PackedContainer<int>));
  assert (pvec->parms().nbits() == 29);
  assert (s.setOption (ityp1, SG::AuxDataOption ("nbits", 23)));
  assert (pvec->parms().nbits() == 23);

  assert (s.getIOType (ityp2) == &typeid(std::vector<int>));
  assert (s.setOption (ityp2, SG::AuxDataOption ("nbits", 28)));
  assert (s.getIOType (ityp2) == &typeid(SG::PackedContainer<int>));
  pvec =
    reinterpret_cast<const SG::PackedContainer<int>*> (s.getIOData (ityp2));
  assert (typeid(*pvec) == typeid(SG::PackedContainer<int>));
  assert (pvec->parms().nbits() == 28);
  assert (s.setOption (ityp2, SG::AuxDataOption ("nbits", 27)));
  assert (pvec->parms().nbits() == 27);

  SG::auxid_t styp1 = SG::AuxTypeRegistry::instance().getAuxID<std::string> ("aString");
  s.getData(styp1, 10, 20);
  assert (!s.setOption (styp1, SG::AuxDataOption ("nbits", 26)));
}


// Test addVector
void test4()
{
  std::cout << "test4\n";

  AuxStoreInternalTest s;
  SG::auxid_t ityp1 = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt");
  SG::auxid_t ityp2 = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt2");
  SG::auxid_t ityp3 = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt3");

  assert (s.size() == 0);
  auto vec1 = CxxUtils::make_unique<SG::AuxTypeVector<int> > (10, 10);
  SG::IAuxTypeVector* vec1ptr = vec1.get();
  s.addVector (ityp1, std::move(vec1), false);
  assert (s.size() == 10);
  assert (s.getIOData(ityp1) == vec1ptr->toVector());
  assert (s.getData(ityp1) == vec1ptr->toPtr());
  assert (vec1ptr->size() == 10);

  auto vec2 = CxxUtils::make_unique<SG::AuxTypeVector<int> > (5, 5);
  SG::IAuxTypeVector* vec2ptr = vec2.get();
  s.addVector (ityp2, std::move(vec2), true);
  assert (s.size() == 10);
  assert (vec2ptr->size() == 10);
  assert (s.getIOData(ityp2) == vec2ptr->toVector());
  assert (s.getData(ityp2) == vec2ptr->toPtr());

  s.lock();
  auto vec3 = CxxUtils::make_unique<SG::AuxTypeVector<int> > (5, 5);
  EXPECT_EXCEPTION (SG::ExcStoreLocked, s.addVector (ityp3, std::move(vec3), false));
  EXPECT_EXCEPTION (SG::ExcStoreLocked, s.getDecoration (ityp1, 10, 10));
  s.getDecoration (ityp2, 10, 10);
}


// Test insertMove
void test5()
{
  std::cout << "test5\n";
  SG::auxid_t ityp1 = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt");
  SG::auxid_t ityp2 = SG::AuxTypeRegistry::instance().getAuxID<int> ("anotherInt");
  SG::auxid_t ityp3 = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt3");
  SG::auxid_t ityp4 = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt4");
  SG::auxid_t mtyp1 = SG::AuxTypeRegistry::instance().getAuxID<MoveTest> ("moveTest");
  SG::AuxStoreInternal s1;
  s1.reserve(20);
  s1.resize(5);

  int* i1 = reinterpret_cast<int*> (s1.getData(ityp1, 5, 20));
  int* i2 = reinterpret_cast<int*> (s1.getData(ityp2, 5, 20));
  MoveTest* m1 = reinterpret_cast<MoveTest*> (s1.getData(mtyp1, 5, 20));

  for (int i=0; i<5; i++) {
    i1[i] = i;
    i2[i] = i+100;
    m1[i] = MoveTest(i);
  }

  SG::AuxStoreInternal s2;
  s2.resize(5);

  int* i1_2 = reinterpret_cast<int*> (s2.getData(ityp1, 5, 5));
  int* i3_2 = reinterpret_cast<int*> (s2.getData(ityp3, 5, 5));
  int* i4_2 = reinterpret_cast<int*> (s2.getData(ityp4, 5, 5));
  MoveTest* m1_2 = reinterpret_cast<MoveTest*> (s2.getData(mtyp1, 5, 5));
  for (int i=0; i<5; i++) {
    i1_2[i] = i+10;
    i3_2[i] = i+110;
    i4_2[i] = i+210;
    m1_2[i] = MoveTest(i+10);
  }

  SG::auxid_set_t ignore { ityp4 };

  assert (! s1.insertMove (3, s2, ignore)); // false due to added vbl
  assert (s1.size() == 10);
  s1.reserve(20);
  assert (s1.getData(ityp4) == nullptr);
  const int* i3 = reinterpret_cast<const int*> (s1.getData(ityp3));
  assert (i3 != 0);
  for (int i=0; i<3; i++) {
    assert (i1[i] == i);
    assert (i2[i] == i+100);
    assert (i3[i] == 0);
    assert (m1[i] == MoveTest(i));
  }
  for (int i=0; i<5; i++) {
    assert (i1[3+i] == i+10);
    assert (i2[3+i] == 0);
    assert (i3[3+i] == i+110);
    assert (m1[3+i] == MoveTest(i+10));
  }
  for (int i=3; i<5; i++) {
    assert (i1[5+i] == i);
    assert (i2[5+i] == i+100);
    assert (i3[5+i] == 0);
    assert (m1[5+i] == MoveTest(i));
  }
  for (int i=0; i<5; i++) {
    assert (wasMoved (m1_2[i]));
  }

  for (int i=0; i<5; i++) {
    i1_2[i] = i+20;
    i3_2[i] = i+120;
    m1_2[i] = MoveTest(i+20);
  }
  assert (s1.insertMove (10, s2, ignore));
  assert (s1.size() == 15);
  for (int i=0; i<3; i++) {
    assert (i1[i] == i);
    assert (i2[i] == i+100);
    assert (i3[i] == 0);
    assert (m1[i] == MoveTest(i));
  }
  for (int i=0; i<5; i++) {
    assert (i1[3+i] == i+10);
    assert (i2[3+i] == 0);
    assert (i3[3+i] == i+110);
    assert (m1[3+i] == MoveTest(i+10));
  }
  for (int i=3; i<5; i++) {
    assert (i1[5+i] == i);
    assert (i2[5+i] == i+100);
    assert (i3[5+i] == 0);
    assert (m1[5+i] == MoveTest(i));
  }
  for (int i=0; i<5; i++) {
    assert (i1[10+i] == i+20);
    assert (i2[10+i] == 0);
    assert (i3[10+i] == i+120);
    assert (m1[10+i] == MoveTest(i+20));
  }
  for (int i=0; i<5; i++) {
    assert (wasMoved (m1_2[i]));
  }
}


class ThreadingTest
{
public:
  ThreadingTest();
  void worker();

  void nonThreadedTest();
  void threadedTest();

  struct testThread
  {
    testThread (ThreadingTest& test)
      : m_test(test) {}
    void operator()()
    {
#ifndef ATHCONTAINERS_NO_THREADS
      boost::shared_lock_guard<boost::shared_mutex> guard (m_test.m_sm);
#endif // not ATHCONTAINERS_NO_THREADS
      m_test.worker();
    }

    ThreadingTest& m_test;
  };

#ifndef ATHCONTAINERS_NO_THREADS
  boost::shared_mutex m_sm;
#endif // not ATHCONTAINERS_NO_THREADS
  std::vector<SG::auxid_t> m_ids;
  SG::AuxStoreInternal m_store;
  size_t m_nids;
  size_t m_nelt;
};


ThreadingTest::ThreadingTest()
  : m_nids (10000),
    m_nelt (1000)
{
  SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
  for (int i=0; i < 10000; i++) {
    std::ostringstream ss;
    ss << "xint" << i;
    SG::auxid_t id = r.getAuxID<int> (ss.str());
    m_ids.push_back (id);
  }
}


void ThreadingTest::worker()
{
  for (SG::auxid_t id : m_ids) {
    int* data = reinterpret_cast<int*> (m_store.getData (id, m_nelt, m_nelt));
    assert (m_store.getData (id) == data);
    data[0] = id;
    assert (m_store.getAuxIDs().count (id) == 1);
  }

  const SG::auxid_set_t& ids = m_store.getAuxIDs();
  assert (ids.size() == m_ids.size());
  for (SG::auxid_t id : m_ids) {
    const int* data = reinterpret_cast<const int*> (m_store.getData (id));
    assert (data[0] == static_cast<int>(id));
    assert (ids.count (id) == 1);
  }
}


void ThreadingTest::nonThreadedTest()
{
  worker();
}


void ThreadingTest::threadedTest()
{
#ifndef ATHCONTAINERS_NO_THREADS
  int nthread = 10;
  AthContainers_detail::thread threads[10];
  m_sm.lock();
  for (int i=0; i < nthread; i++)
    threads[i] = AthContainers_detail::thread (testThread (*this));
  // Try to get the threads starting as much at the same time as possible.
  m_sm.unlock();
  for (int i=0; i < nthread; i++)
    threads[i].join();
#endif
}


void test_threading()
{
  std::cout << "test_threading\n";

  ThreadingTest test;
  test.nonThreadedTest();
  for (int i=0; i < 100; i++)
    test.threadedTest();
}


int main()
{
  test1();
  test2();
  test3();
  test4();
  test5();
  test_threading();
  return 0;
}
