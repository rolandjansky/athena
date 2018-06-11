/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file CxxUtils/atomic_fetch_minmax_test.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2017
 * @brief Unit tests for atomic_fetch_min/max.
 */


#undef NDEBUG
#include "CxxUtils/atomic_fetch_minmax.h"
#include "TestTools/random.h"
#include <mutex>
#include <thread>
#include <shared_mutex>
#include <limits>
#include <iostream>
#include <cassert>

using CxxUtils::atomic_fetch_max;
using CxxUtils::atomic_fetch_min;


template <class T>
void test1_test()
{
  std::atomic<T> v;
  v = 10;
  assert (atomic_fetch_max (&v, static_cast<T>(20)) == 10);
  assert (v == 20);
  assert (atomic_fetch_max (&v, static_cast<T>(15)) == 20);
  assert (v == 20);
  assert (atomic_fetch_min (&v, static_cast<T>(10)) == 20);
  assert (v == 10);
  assert (atomic_fetch_min (&v, static_cast<T>(15)) == 10);
  assert (v == 10);
}


void test1()
{
  std::cout << "test1\n";

  test1_test<char>();
  test1_test<signed char>();
  test1_test<unsigned char>();
  test1_test<short>();
  test1_test<unsigned short>();
  test1_test<int>();
  test1_test<unsigned int>();
  test1_test<long>();
  test1_test<unsigned long>();
  test1_test<long long>();
  test1_test<unsigned long long>();
  test1_test<char16_t>();
  test1_test<char32_t>();
  test1_test<wchar_t>();
}


std::shared_timed_mutex start_mutex;


template <class T>
class test2_Thread
{
public:
  test2_Thread (int ithread, std::atomic<T>& xmin, std::atomic<T>& xmax);
  void operator()();

private:
  int m_ithread;
  uint32_t m_seed;
  std::atomic<T>& m_xmin;
  std::atomic<T>& m_xmax;
};


template <class T>
test2_Thread<T>::test2_Thread (int ithread, std::atomic<T>& xmin, std::atomic<T>& xmax)
  : m_ithread (ithread),
    m_seed (ithread * 123),
    m_xmin (xmin),
    m_xmax (xmax)
{
}


template <class T>
void test2_Thread<T>::operator()()
{
  std::shared_lock<std::shared_timed_mutex> lock (start_mutex);

  const int niter = 1000000;

  for (int i=0; i < niter; i++) {
    T val = static_cast<T> (Athena_test::rng_seed (m_seed));
    T omax = atomic_fetch_max (&m_xmax, val);
    T omin = atomic_fetch_min (&m_xmin, val);
    assert (omax <= m_xmax);
    assert (omin >= m_xmin);
    assert (m_xmax >= val);
    assert (m_xmin <= val);
  }
}


template <class T>
void test2_test()
{
  const int nthread = 4;
  std::thread threads[nthread];
  start_mutex.lock();

  std::atomic<T> xmin;
  std::atomic<T> xmax;

  xmin = std::numeric_limits<T>::max();
  xmax = std::numeric_limits<T>::min();

  threads[0] = std::thread (test2_Thread<T> (0, xmin, xmax));
  threads[1] = std::thread (test2_Thread<T> (1, xmin, xmax));
  threads[2] = std::thread (test2_Thread<T> (2, xmin, xmax));
  threads[3] = std::thread (test2_Thread<T> (3, xmin, xmax));

  // Try to get the threads starting as much at the same time as possible.
  start_mutex.unlock();
  for (int i=0; i < nthread; i++)
    threads[i].join();
}


void test2()
{
  std::cout << "test2\n";

  test2_test<char>();
  test2_test<signed char>();
  test2_test<unsigned char>();
  test2_test<short>();
  test2_test<unsigned short>();
  test2_test<int>();
  test2_test<unsigned int>();
  test2_test<long>();
  test2_test<unsigned long>();
  test2_test<long long>();
  test2_test<unsigned long long>();
  test2_test<char16_t>();
  test2_test<char32_t>();
  test2_test<wchar_t>();
}


int main()
{
  test1();
  test2();
  return 0;
}
