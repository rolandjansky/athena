/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */

// $Id$
/**
 * @file AthLinks/test/GeneicElementLinkBase_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2017
 * @brief Regression tests for CachedValue
 */


#undef NDEBUG
#include "AthLinks/tools/CachedValue.h"
#include <vector>
#include <iostream>
#include <cassert>
#include <thread>
#include <shared_mutex>


// Attempting to test atomic updating.
struct Payload
{
  static const size_t SIZE=10000000;
  Payload (int x=0) : m_x(SIZE) { set(x); }
  void set (int x);
  void check (int x) const;
  bool valid() const { return !m_x.empty(); }
  operator int() const { return m_x[0]; }
  std::vector<int> m_x;
};


void Payload::set (int x)
{
  m_x.resize (SIZE);
  for (size_t i=0; i < SIZE; i++)
    m_x[i] = x;
}


void Payload::check (int x) const
{
  for (int i=SIZE-1; i >= 0; i--) {
    assert (m_x.size() == SIZE);
    int xx = m_x[i];
    assert (xx == x);
  }
}


// Basic tests.
void test1()
{
  std::cout << "test1\n";
  SG::CachedValue<Payload> cv1;
  assert (!cv1.isValid());
  cv1.store (Payload(1));
  assert (cv1.isValid());
  cv1.ptr()->check(1);
  cv1.reset();
  assert (!cv1.isValid());

  SG::CachedValue<Payload> cv2 (Payload (2));
  assert (cv2.isValid());
  cv2.ptr()->check(2);

  cv1 = cv2;
  assert (cv1.isValid());
  cv1.ptr()->check(2);

  cv2.reset();
  assert (!cv2.isValid());
  cv1 = cv2;
  assert (!cv1.isValid());

  const SG::CachedValue<Payload> cv3 (cv1);
  assert (!cv3.isValid());
  cv3.set (Payload(4));
  assert (cv3.isValid());
  cv3.ptr()->check(4);
  const SG::CachedValue<Payload> cv4 (cv3);
  assert (cv4.isValid());
  cv4.ptr()->check(4);
}


// Move operations.
void test2()
{
  std::cout << "test2\n";

  Payload p1(1);
  assert (p1.valid());
  
  SG::CachedValue<Payload> cv1 (std::move (p1));
  assert (cv1.isValid());
  cv1.ptr()->check(1);
  assert (!p1.valid());

  SG::CachedValue<Payload> cv2 (std::move (cv1));
  assert (cv2.isValid());
  cv2.ptr()->check(1);
  assert (!cv1.isValid());
  assert (!cv1.ptr()->valid());

  SG::CachedValue<Payload> cv3 (std::move (cv1));
  assert (!cv3.isValid());

  cv1 = std::move (cv2);
  assert (cv1.isValid());
  cv1.ptr()->check(1);
  assert (!cv2.isValid());
  assert (!cv2.ptr()->valid());

  cv1 = std::move (cv3);
  assert (!cv1.isValid());
  assert (!cv3.isValid());

  Payload p2(2);
  cv1.store (std::move (p2));
  assert (cv1.isValid());
  cv1.ptr()->check(2);
  assert (!p2.valid());

  Payload p3(3);
  cv2.set (std::move (p3));
  assert (cv2.isValid());
  cv2.ptr()->check(3);
  assert (!p3.valid());
}


class ThreadingTest
{
public:
  static const int NVAL = 10;
  SG::CachedValue<Payload> m_vals[10];
  std::shared_timed_mutex m_sm;

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
    Payload p(i+1);
    if (i%3 == 0) {
      m_test.m_vals[i].set (std::move (p));
    }
    else {
      m_test.m_vals[i].set (p);
    }
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
      if (!checked[i] && m_test.m_vals[i].isValid()) {
        m_test.m_vals[i].ptr()->check (i+1);
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
    m_vals[i].reset();
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


// Conversion test.
void test3()
{
  std::cout << "test3\n";
  SG::CachedValue<Payload> cv1 (Payload (1));
  SG::CachedValue<int> cv2 (cv1);
  assert (cv2.isValid());
  assert (*cv2.ptr() == 1);
}


// Threading test.
void test4()
{
  std::cout << "test4\n";

  ThreadingTest test;
  for (int i=0; i < 20; i++)
    test.threadedTest();
}


int main()
{
  test1();
  test2();
  test3();
  test4();
  return 0;
}
