/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */

// $Id$
/**
 * @file CxxUtils/test/CachedUniquePtr_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date mar, 2019
 * @brief Unit tests for CachedUniquePtr
 */


#undef NDEBUG
#include "CxxUtils/CachedUniquePtr.h"
#include <atomic>
#include <iostream>
#include <cassert>
#include <thread>
#include <shared_mutex>


struct P
{
  P(int x) : m_x(x) { ++s_count; }
  ~P() { --s_count; }

  int m_x;
  static std::atomic<int> s_count;
};

std::atomic<int> P::s_count = 0;


// Basic tests.
void test1()
{
  std::cout << "test1\n";
  //int x1 = 0;
  //int x2 = 0;
  //int x4 = 0;

  CxxUtils::CachedUniquePtr<P> cp1;
  assert (!cp1.get());
  assert (!cp1);
  assert (P::s_count == 0);
  cp1.store (std::make_unique<P>(1));
  assert (P::s_count == 1);
  assert (cp1.get()->m_x == 1);
  assert (cp1);
  assert (cp1->m_x == 1);
  assert ((*cp1).m_x == 1);
  cp1.store (nullptr);
  assert (!cp1.get());
  assert (P::s_count == 0);

  CxxUtils::CachedUniquePtr<P> cp2 (std::make_unique<P>(2));
  assert (cp2->m_x == 2);
  assert (P::s_count == 1);

  cp1 = std::move(cp2);
  assert (cp1->m_x == 2);
  assert (!cp2);
  assert (P::s_count == 1);

  {
    const CxxUtils::CachedUniquePtr<P> cp3 (std::move (cp1));
    assert (cp3->m_x == 2);
    assert (!cp1);
    assert (P::s_count == 1);
  }
  assert (P::s_count == 0);

  cp1.set (std::make_unique<P>(3));
  assert (cp1->m_x == 3);
  assert (P::s_count == 1);

  cp1.set (std::make_unique<P>(4));
  assert (cp1->m_x == 3);
  assert (P::s_count == 1);

  std::unique_ptr<const P> up = cp1.release();
  assert (!cp1);
  assert (up->m_x == 3);
  assert (P::s_count == 1);
}


class ThreadingTest
{
public:
  static const int NVAL = 10;
  CxxUtils::CachedUniquePtr<P> m_vals[10];
  std::shared_timed_mutex m_sm;
  int m_x[NVAL] = {0};

  void threadedTest();

  struct writerThread
  {
    writerThread (ThreadingTest& test, int iworker)
      : m_test (test), m_iworker (iworker) {}
    void operator()();
    ThreadingTest& m_test;
    int m_iworker;
  };

  
  struct readerThread
  {
    readerThread (ThreadingTest& test, int iworker)
      : m_test (test), m_iworker (iworker) {}
    void operator()();
    ThreadingTest& m_test;
    int m_iworker;
  };
};


void ThreadingTest::writerThread::operator()()
{
  int i = m_iworker;
  do {
    m_test.m_vals[i].set (std::make_unique<const P>(i));
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
      if (!checked[i] && m_test.m_vals[i]) {
        assert (m_test.m_vals[i]->m_x == i);
        checked[i] = true;
        ++nchecked;
      }
      i++;
      if (i >= NVAL) i = 0;
    } while (i != m_iworker);
  }
}


void ThreadingTest::threadedTest()
{
  for (int i=0; i < NVAL; i++) {
    m_vals[i].store (nullptr);
  }

  const int nthread = 10;
  std::thread threads[nthread];
  m_sm.lock();

  for (int i=0; i < nthread; i++) {
    if (i < 3) {
      threads[i] = std::thread (writerThread (*this, i));
    }
    else {
      threads[i] = std::thread (readerThread (*this, i));
    }
  }

  // Try to get the threads starting as much at the same time as possible.
  m_sm.unlock();
  for (int i=0; i < nthread; i++)
    threads[i].join();
}


// Threading test.
void test2()
{
  std::cout << "test2\n";

  ThreadingTest test;
#if 0
  for (int i=0; i < 20; i++)
    test.threadedTest();
#endif
}


int main()
{
  std::cout << "CachedUniquePtr_test\n";
  test1();
  test2();
  return 0;
}
