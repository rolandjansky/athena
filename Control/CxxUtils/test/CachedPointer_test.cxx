/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */

// $Id$
/**
 * @file CxxUtils/CachedPointer_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2017
 * @brief Unit tests for CachedPointer
 */


#undef NDEBUG
#include "CxxUtils/CachedPointer.h"
#include <vector>
#include <iostream>
#include <cassert>
#include <thread>
#include <shared_mutex>


// Basic tests.
void test1()
{
  std::cout << "test1\n";
  int x1 = 0;
  int x2 = 0;
  int x4 = 0;

  CxxUtils::CachedPointer cp1;
  assert (!cp1.get());
  cp1.store (&x1);
  assert (cp1.get() == &x1);
  assert (*cp1.ptr() == &x1);
  cp1.store (nullptr);
  assert (!cp1.get());

  CxxUtils::CachedPointer cp2 (&x2);
  assert (cp2.get() == &x2);
  assert (*cp2.ptr() == &x2);

  cp1 = cp2;
  assert (cp1.get() == &x2);
  assert (*cp1.ptr() == &x2);

  cp2.store (nullptr);
  assert (!cp2.get());
  cp1 = cp2;
  assert (!cp1.get());

  const CxxUtils::CachedPointer cp3 (cp1);
  assert (!cp3.get());
  cp3.set (&x4);
  assert (cp3.get() == &x4);
  assert (*cp3.ptr() == &x4);
  const CxxUtils::CachedPointer cp4 (cp3);
  assert (cp4.get() == &x4);
  assert (*cp4.ptr() == &x4);
}


class ThreadingTest
{
public:
  static const int NVAL = 10;
  CxxUtils::CachedPointer m_vals[10];
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
    m_test.m_vals[i].set (&m_test.m_x[i]);
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
      if (!checked[i] && m_test.m_vals[i].get() != nullptr) {
        assert (m_test.m_vals[i].get() == &m_test.m_x[i]);
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
  for (int i=0; i < 20; i++)
    test.threadedTest();
}


int main()
{
  test1();
  test2();
  return 0;
}
