/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/test/AuxVectorData_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2013
 * @brief Regression tests for AuxVectorData
 */

// Disable this test in standalone mode:
#ifndef XAOD_STANDALONE

#undef NDEBUG
#include "AthContainers/AuxVectorData.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/AuxStoreInternal.h"
#include "AthContainers/exceptions.h"
#include "SGTools/TestStore.h"
#include "TestTools/expect_exception.h"
#include "CxxUtils/checker_macros.h"
#ifndef ATHCONTAINERS_NO_THREADS
#include "boost/thread/shared_mutex.hpp"
#include "boost/thread/shared_lock_guard.hpp"
#endif
#include <iostream>
#include <sstream>
#include <cassert>


#include "auxid_set_equal.icc"


namespace SG {


class AuxVectorData_test
  : public AuxVectorData
{
public:
  using AuxVectorData::setStore;
  using AuxVectorData::s_minCacheLen;

  virtual size_t size_v() const { return 10; }
  virtual size_t capacity_v() const { return 20; }
};


} // namespace SG



using SG::AuxVectorData;
using SG::AuxVectorData_test;


void test_get_data()
{
  std::cout << "test_get_data\n";
  AuxVectorData_test b1;
  const AuxVectorData& cb1 = b1;
  SG::auxid_t ityp = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt");
  EXPECT_EXCEPTION (SG::ExcNoAuxStore, b1.getData<int> (ityp, 0));
  EXPECT_EXCEPTION (SG::ExcNoAuxStore, cb1.getDataArray (ityp));
  EXPECT_EXCEPTION (SG::ExcNoAuxStore, cb1.getDataArrayAllowMissing (ityp));

  SG::AuxStoreInternal store;
  SG::IConstAuxStore* cstore = &store;

  assert (!b1.hasNonConstStore());
  assert (!b1.hasStore());
  assert (!b1.isAvailable (ityp));
  assert (!b1.isAvailableWritable (ityp));
  assert (!b1.isAvailableWritableAsDecoration (ityp));
  assert (!b1.isAvailable<int> ("anInt"));
  assert (!b1.isAvailableWritable<int> ("anInt"));
  assert (!b1.isAvailableWritableAsDecoration<int> ("anInt"));
  assert (b1.getConstStore() == 0);
  assert (b1.getStore() == 0);
  b1.setStore (cstore);
  assert (b1.getConstStore() == cstore);
  assert (b1.getStore() == 0);
  assert (!b1.hasNonConstStore());
  assert (b1.hasStore());
  assert (!b1.isAvailable (ityp));
  assert (!b1.isAvailableWritable (ityp));
  assert (!b1.isAvailableWritableAsDecoration (ityp));
  assert (!b1.isAvailable<int> ("anInt"));
  assert (!b1.isAvailableWritable<int> ("anInt"));
  assert (!b1.isAvailableWritableAsDecoration<int> ("anInt"));
  EXPECT_EXCEPTION (SG::ExcConstAuxData, b1.getData<int> (ityp, 0));
  EXPECT_EXCEPTION (SG::ExcBadAuxVar, cb1.getData<int> (ityp, 0));   
  EXPECT_EXCEPTION (SG::ExcBadAuxVar, cb1.getDataArray (ityp));
  assert (cb1.getDataArrayAllowMissing (ityp) == nullptr);

  b1.setStore (&store);
  assert (b1.getConstStore() == &store);
  assert (b1.getStore() == &store);
  assert (b1.hasNonConstStore());
  assert (b1.hasStore());
  assert (b1.getData<int> (ityp, 0) == 0);
  assert (b1.getData<int> (ityp, 1) == 0);
  b1.getData<int> (ityp, 0) = 1;
  b1.getData<int> (ityp, 1) = 2;
  assert (b1.getData<int> (ityp, 0) == 1);
  assert (b1.getData<int> (ityp, 1) == 2);
  assert (b1.isAvailable (ityp));
  assert (b1.isAvailableWritable (ityp));
  assert (b1.isAvailableWritableAsDecoration (ityp));
  assert (b1.isAvailable<int> ("anInt"));
  assert (b1.isAvailableWritable<int> ("anInt"));
  assert (b1.isAvailableWritableAsDecoration<int> ("anInt"));
  assert (reinterpret_cast<const int*>(cb1.getDataArray (ityp))[1] == 2);
  assert (reinterpret_cast<const int*>(cb1.getDataArrayAllowMissing (ityp))[1] == 2);

  b1.setStore (cstore);
  assert (!b1.hasNonConstStore());
  assert (cb1.getData<int> (ityp, 0) == 1);
  assert (cb1.getData<int> (ityp, 1) == 2);
  assert (b1.isAvailable (ityp));
  assert (!b1.isAvailableWritable (ityp));
  assert (b1.isAvailableWritableAsDecoration (ityp));
  assert (b1.isAvailable<int> ("anInt"));
  assert (!b1.isAvailableWritable<int> ("anInt"));
  assert (b1.isAvailableWritableAsDecoration<int> ("anInt"));

  SG::auxid_t ftyp = SG::AuxTypeRegistry::instance().getAuxID<float> ("aFloat");
  float* ff = reinterpret_cast<float*> (store.getData (ftyp, 10, 20));
  ff[0] = 1.5;
  ff[1] = 2.5;
  assert (cb1.getData<float> (ftyp, 0) == 1.5);
  assert (cb1.getData<float> (ftyp, 1) == 2.5);
}


void test_swap()
{
  std::cout << "test_swap\n";
  SG::auxid_t ityp = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt");

  AuxVectorData_test b1;
  SG::AuxStoreInternal store1;
  b1.setStore (&store1);

  AuxVectorData_test b2;
  SG::AuxStoreInternal store2;
  b2.setStore (&store2);

  b1.getData<int> (ityp, 0) = 1;
  b2.getData<int> (ityp, 0) = 11;
  b2.getData<int> (ityp, 1) = 12;

  b1.swap (b2);
  assert (b1.getData<int> (ityp, 0) == 11);
  assert (b1.getData<int> (ityp, 1) == 12);
  assert (b2.getData<int> (ityp, 0) == 1);
}


void test_get_types()
{
  std::cout << "test_get_types\n";
  AuxVectorData_test b1;

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

  assert (b1.getWritableAuxIDs() == b1.getAuxIDs());
}


void test_decoration()
{
  std::cout << "test_decoration\n";

  AuxVectorData_test b1;
  const AuxVectorData& cb1 = b1;
  SG::AuxStoreInternal store;
  SG::IConstAuxStore* cstore = &store;

  SG::auxid_t ityp1 = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt1");
  SG::auxid_t ityp2 = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt2");
  SG::auxid_t ityp3 = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt3");

  EXPECT_EXCEPTION (SG::ExcNoAuxStore, b1.getDecoration<int> (ityp1, 0));
  b1.setStore (cstore);
  //EXPECT_EXCEPTION (SG::ExcConstAuxData, b1.getDecoration<int> (ityp1, 0));
  b1.getDecoration<int> (ityp1, 0) = 9;
  assert (cb1.getData<int> (ityp1, 0) == 9);
  b1.setStore (&store);

  b1.getData<int> (ityp1, 0) = 10;
  cb1.getDecoration<int> (ityp2, 0) = 11;
  assert (cb1.getData<int> (ityp1, 0) == 10);
  assert (cb1.getData<int> (ityp2, 0) == 11);

  b1.lock();

  assert (cb1.getData<int> (ityp1, 0) == 10);
  //EXPECT_EXCEPTION (SG::ExcStoreLocked, b1.getData<int> (ityp1, 0));
  EXPECT_EXCEPTION (SG::ExcStoreLocked, b1.getDecoration<int> (ityp1, 0));

  cb1.getDecoration<int> (ityp3, 0) = 14;
  assert (cb1.getData<int> (ityp1, 0) == 10);
  assert (cb1.getData<int> (ityp2, 0) == 11);
  assert (cb1.getData<int> (ityp3, 0) == 14);
  assert (cb1.getDecoration<int> (ityp3, 0) == 14);

  assert (b1.clearDecorations() == true);
  assert (b1.clearDecorations() == false);
  assert (cb1.getData<int> (ityp1, 0) == 10);
  assert (cb1.getData<int> (ityp2, 0) == 11);
  EXPECT_EXCEPTION (SG::ExcBadAuxVar, cb1.getData<int> (ityp3, 0));
  cb1.getDecoration<int> (ityp3, 0) = 15;
}


void test_move()
{
  std::cout << "test_move\n";

#if __cplusplus > 201100
  AuxVectorData_test b1;
  SG::AuxStoreInternal store;
  b1.setStore (&store);
  
  SG::auxid_t ityp = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt");
  b1.getData<int> (ityp, 0) = 1;
  b1.getData<int> (ityp, 1) = 2;
  assert (b1.getData<int> (ityp, 0) == 1);
  assert (b1.getData<int> (ityp, 1) == 2);

  AuxVectorData_test b2 (std::move (b1));
  assert (b1.getConstStore() == 0);
  assert (b1.getStore() == 0);
  assert (b2.getConstStore() == &store);
  assert (b2.getStore() == &store);
  assert (b2.getData<int> (ityp, 0) == 1);
  assert (b2.getData<int> (ityp, 1) == 2);

  AuxVectorData_test b3;
  b3 = std::move (b2);
  assert (b2.getConstStore() == 0);
  assert (b2.getStore() == 0);
  assert (b3.getConstStore() == &store);
  assert (b3.getStore() == &store);
  assert (b3.getData<int> (ityp, 0) == 1);
  assert (b3.getData<int> (ityp, 1) == 2);
#endif
}


class ThreadingTest
{
public:
  ThreadingTest();
  void worker (AuxVectorData& b, size_t istart);
  void worker_c (const AuxVectorData& b, size_t istart);

  void nonThreadedTest();
  void threadedTest();

  struct testThread
  {
    testThread (ThreadingTest& test, AuxVectorData& b, int iworker)
      : m_test(test), m_b (b), m_iworker(iworker) {}
    void operator()()
    {
#ifndef ATHCONTAINERS_NO_THREADS
      boost::shared_lock_guard<boost::shared_mutex> guard (m_test.m_sm);
#endif // not ATHCONTAINERS_NO_THREADS
      size_t istart = (m_iworker * 10) % m_test.m_nelt;
      if (m_iworker&1)
        m_test.worker (m_b, istart);
      else
        m_test.worker_c (m_b, istart);
    }

    ThreadingTest& m_test;
    AuxVectorData& m_b;
    int m_iworker;
  };

  size_t m_nelt;
  SG::AuxStoreInternal m_store;
  std::vector<SG::auxid_t> m_ids;
#ifndef ATHCONTAINERS_NO_THREADS
  boost::shared_mutex m_sm;
#endif // not ATHCONTAINERS_NO_THREADS
};


ThreadingTest::ThreadingTest()
  : m_nelt (1000)
{
  SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
  for (int i=0; i < 10000; i++) {
    std::ostringstream ss;
    ss << "xint" << i;
    SG::auxid_t id = r.getAuxID<int> (ss.str());
    m_ids.push_back (id);
    int* data = reinterpret_cast<int*> (m_store.getData (id, m_nelt, m_nelt));
    for (size_t j = 0; j < m_nelt; j++)
      data[j] = id*1234 + j;
  }
}


void ThreadingTest::worker (AuxVectorData& b, size_t istart)
{
  for (SG::auxid_t id : m_ids) {
    assert (b.getData<int> (id, istart) == static_cast<int>(id*1234 + istart));
    ++istart;
    if (istart >= m_nelt) istart = 0;
  }
}


void ThreadingTest::worker_c (const AuxVectorData& b, size_t istart)
{
  for (SG::auxid_t id : m_ids) {
    assert (b.getData<int> (id, istart) == static_cast<int>(id*1234 + istart));
    ++istart;
    if (istart >= m_nelt) istart = 0;
  }
}


void ThreadingTest::nonThreadedTest()
{
  AuxVectorData_test b;
  b.setStore (&m_store);
  worker (b, 0);
  worker_c (b, 10);
}


void ThreadingTest::threadedTest()
{
#ifndef ATHCONTAINERS_NO_THREADS
  AuxVectorData_test b;
  b.setStore (&m_store);

  int nthread = 10;
  AthContainers_detail::thread threads[10];
  m_sm.lock();
  for (int i=0; i < nthread; i++)
    threads[i] = AthContainers_detail::thread (testThread (*this, b, i));
  // Try to get the threads starting as much at the same time as possible.
  m_sm.unlock();
  for (int i=0; i < nthread; i++)
    threads[i].join();
#endif
}


class TestStore
  : public SG::IAuxStore
{
public:
  TestStore()  {}
  virtual const void* getData (SG::auxid_t) const { std::abort(); }
  virtual void* getDecoration (SG::auxid_t, size_t, size_t) { std::abort(); }
  virtual const SG::auxid_set_t& getAuxIDs() const { std::abort(); }
  virtual void lock() { std::abort(); }
  virtual void lockDecoration (SG::auxid_t) { std::abort(); }
  virtual bool clearDecorations() { std::abort(); }
  virtual size_t size() const { std::abort(); }
  virtual void* getData (SG::auxid_t, size_t, size_t) { std::abort(); }
  virtual const SG::auxid_set_t& getWritableAuxIDs() const { std::abort(); }
  virtual bool resize (size_t) { std::abort(); }
  virtual void reserve (size_t) { std::abort(); }
  virtual void shift (size_t, ptrdiff_t) { std::abort(); }
  virtual bool insertMove (size_t, IAuxStore&, const SG::auxid_set_t&) { std::abort(); }

  virtual bool setOption (SG::auxid_t auxid, const SG::AuxDataOption&  option)
  {
    lastid = auxid;
    lastopt = option;
    return true;
  }

  static SG::auxid_t lastid ATLAS_THREAD_SAFE;
  static SG::AuxDataOption lastopt ATLAS_THREAD_SAFE;
};


SG::auxid_t TestStore::lastid = 0;
SG::AuxDataOption TestStore::lastopt ("", 0);


void test_setoption()
{
  std::cout << "test_setoption\n";
  AuxVectorData_test b;
  assert (!b.setOption (SG::null_auxid, SG::AuxDataOption ("opt", 1)));
  b.setStore (new TestStore);
  assert (!b.setOption (SG::null_auxid, SG::AuxDataOption ("opt", 1)));
  assert (b.setOption (1, SG::AuxDataOption ("opt", 1)));
  assert (TestStore::lastid == 1);
  assert (TestStore::lastopt.name() == "opt");
  assert (TestStore::lastopt.intVal() == 1);

  SG::auxid_t xtyp = SG::AuxTypeRegistry::instance().getAuxID<int> ("xint");
  assert (b.setOption ("xint", SG::AuxDataOption ("opt2", 2)));
  assert (TestStore::lastid == xtyp);
  assert (TestStore::lastopt.name() == "opt2");
  assert (TestStore::lastopt.intVal() == 2);

  SG::auxid_t xtyp2 = SG::AuxTypeRegistry::instance().getAuxID<int> ("xint2",
                                                                     "xcls");
  assert (b.setOption ("xint2", "xcls", SG::AuxDataOption ("opt3", 2.5)));
  assert (TestStore::lastid == xtyp2);
  assert (TestStore::lastopt.name() == "opt3");
  assert (TestStore::lastopt.floatVal() == 2.5);

  assert (b.setOption (1, "opt", 1));
  assert (TestStore::lastid == 1);
  assert (TestStore::lastopt.name() == "opt");
  assert (TestStore::lastopt.intVal() == 1);

  assert (b.setOption ("xint", "opt2", 2));
  assert (TestStore::lastid == xtyp);
  assert (TestStore::lastopt.name() == "opt2");
  assert (TestStore::lastopt.intVal() == 2);

  assert (b.setOption ("xint2", "xcls", "opt3", 2.5));
  assert (TestStore::lastid == xtyp2);
  assert (TestStore::lastopt.name() == "opt3");
  assert (TestStore::lastopt.floatVal() == 2.5);
}


void test_storelink()
{
  std::cout << "test_storelink\n";

  AuxVectorData_test b1;
  assert (!b1.hasStore());

  b1.setStore (DataLink<SG::IConstAuxStore> ("foo"));
  assert (b1.getConstStoreLink().dataID() == "foo");
}


void test_threading()
{
  std::cout << "test_threading\n";

  ThreadingTest test;
  test.nonThreadedTest();
  for (int i=0; i < 1000; i++)
    test.threadedTest();
}


// These aren't called; present just to check generation of optimized code.
double test_code (SG::auxid_t auxid, AuxVectorData_test& b)
{
  return b.getData<double> (auxid, 0) + b.getData<double> (auxid, 1);
}  
double test_code (SG::auxid_t auxid, const AuxVectorData_test& b)
{
  return b.getData<double> (auxid, 0) + b.getData<double> (auxid, 1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  SGTest::initTestStore();

  // Make reallocations more frequent (the better to exercise them).
  AuxVectorData_test::s_minCacheLen = 1;
  test_get_data();
  test_swap();
  test_get_types();
  test_decoration();
  test_move();
  test_setoption();
  test_storelink();
  test_threading();
  return 0;
}

#else

int main() {
   return 0;
}

#endif // not XAOD_STANDALONE
