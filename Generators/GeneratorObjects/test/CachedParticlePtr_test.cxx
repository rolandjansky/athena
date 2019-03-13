/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file GeneratorObjects/CachedParticlePtr_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2019
 * @brief Unit tests for CachedParticlePtr
 */


#undef NDEBUG
#include "GeneratorObjects/CachedParticlePtr.h"
#include "HepMC/GenParticle.h"
#include "SGTools/TestStore.h"
#include "SGTools/CurrentEventStore.h"
#include <thread>
#include <shared_mutex>
#include <iostream>
#include <cassert>


void test1 (const IProxyDict* store)
{
  std::cout << "test1\n";
  HepMC::GenParticle gp1;

  const IProxyDict* sg = nullptr;
  GeneratorObjects::CachedParticlePtr p1;
  assert (p1.get (sg) == nullptr);
  assert (sg == store);
  p1.set (sg, &gp1);
  assert (p1.get (sg) == &gp1);
  assert (sg == nullptr);

  SGTest::TestStore store2;
  GeneratorObjects::CachedParticlePtr p2 (&store2);
  assert (p2.get (sg) == nullptr);
  assert (sg == &store2);

  GeneratorObjects::CachedParticlePtr p3 (&gp1);
  assert (p3.get (sg) == &gp1);
  assert (sg == nullptr);

  GeneratorObjects::CachedParticlePtr p4 (p1);
  assert (p4.get (sg) == &gp1);
  assert (sg == nullptr);

  p2 = p4;
  assert (p2.get (sg) == &gp1);
  assert (sg == nullptr);

  p1.store (&store2);
  assert (p1.get (sg) == nullptr);
  assert (sg == &store2);
}


class ThreadingTest
{
public:
  static const int NVAL = 10;
  GeneratorObjects::CachedParticlePtr m_vals[NVAL];
  std::shared_timed_mutex m_sm;
  HepMC::GenParticle m_parts[NVAL];

  void threadedTest (const IProxyDict* store);

  struct writerThread
  {
    writerThread (ThreadingTest& test, int iworker, const IProxyDict* store)
      : m_test (test), m_iworker (iworker), m_store (store) {}
    void operator()();
    ThreadingTest& m_test;
    int m_iworker;
    const IProxyDict* m_store;
  };

  
  struct readerThread
  {
    readerThread (ThreadingTest& test, int iworker, const IProxyDict* store)
      : m_test (test), m_iworker (iworker), m_store (store) {}
    void operator()();
    ThreadingTest& m_test;
    int m_iworker;
    const IProxyDict* m_store;
  };
};


void ThreadingTest::writerThread::operator()()
{
  int i = m_iworker;
  do {
    m_test.m_vals[i].set (m_store, &m_test.m_parts[i]);
    i++;
    if (i >= NVAL) i = 0;
  } while (i != m_iworker);
}


void ThreadingTest::readerThread::operator()()
{
  bool checked[NVAL] = {false};
  int nchecked = 0;
  while (nchecked < NVAL) {
    int i = m_iworker;
    do {
      if (!checked[i]) {
        const IProxyDict* sg = nullptr;
        const HepMC::GenParticle* p = m_test.m_vals[i].get (sg);
        if (p != nullptr) {
          assert (p == &m_test.m_parts[i]);
          assert (sg == nullptr);
          checked[i] = true;
          ++nchecked;
        }
        else {
          assert (sg == m_store);
        }
      }
      i++;
      if (i >= NVAL) i = 0;
    } while (i != m_iworker);
  }
}


void ThreadingTest::threadedTest (const IProxyDict* store)
{
  const int nthread = 10;
  std::thread threads[nthread];
  m_sm.lock();

  for (int i=0; i < nthread; i++) {
    if (i < 3) {
      threads[i] = std::thread (writerThread (*this, i, store));
    }
    else {
      threads[i] = std::thread (readerThread (*this, i, store));
    }
  }

  // Try to get the threads starting as much at the same time as possible.
  m_sm.unlock();
  for (int i=0; i < nthread; i++)
    threads[i].join();
}


// Threading test.
void test2 (const IProxyDict* store)
{
  std::cout << "test2\n";

  ThreadingTest test;
  for (int i=0; i < 20; i++)
    test.threadedTest (store);
}


int main()
{
  std::cout << "CachedParticlePtr_test\n";

  SGTest::TestStore store;
  SG::CurrentEventStore::setStore (&store);

  test1 (&store);
  test2 (&store);

  return 0;
}
