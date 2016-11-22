/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/test/threading_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2013
 * @brief Regression tests for threading.
 */


#undef NDEBUG
#include "AthContainers/tools/threading.h"
#include <iostream>
#include <cassert>


class test_mutex
{
public:
  void lock_shared() const { std::cout << "lock_shared\n"; }
  void unlock_shared() const { std::cout << "unlock_shared\n"; }
  void lock_upgrade() { std::cout << "lock_upgrade\n"; }
  void unlock_upgrade() { std::cout << "unlock_upgrade\n"; }
  void lock() { std::cout << "lock\n"; }
  void unlock() { std::cout << "unlock\n"; }
  void unlock_upgrade_and_lock() { std::cout << "unlock_upgrade_and_lock\n"; }
};


void test1()
{
  using AthContainers_detail::strict_shared_lock;

  std::cout << "test1\n";
  test_mutex m;
  strict_shared_lock<test_mutex> lock (m);
}


void test2()
{
  using AthContainers_detail::upgrading_lock;
  using AthContainers_detail::upgrade_mutex;

  std::cout << "test2\n";
  test_mutex m;
  {
    upgrading_lock<test_mutex> lock (m);
  }
  {
    upgrading_lock<test_mutex> lock (m);
    lock.upgrade();
  }
  {
    upgrading_lock<test_mutex> lock (m);
    lock.upgrade();
    lock.upgrade();
  }

  upgrade_mutex mm;
  upgrading_lock<upgrade_mutex> lock (mm);
  lock.upgrade();
  lock.upgrade();
}


void test3()
{
  using AthContainers_detail::fence_acq_rel;
  using AthContainers_detail::fence_seq_cst;
  using AthContainers_detail::mutex;
  using AthContainers_detail::lock_guard;

  std::cout << "test3\n";

  fence_acq_rel();
  fence_seq_cst();

  mutex m;
  {
    lock_guard<mutex> g (m);
  }
}


struct Payload
{
  Payload(int i) : m_i(i) { std::cout << "ctor " << i << "\n"; }
  ~Payload() { std::cout << "dtor " << m_i << "\n"; }
  Payload& operator= (const Payload&) = default;
  int m_i;
};

void test4()
{
  using AthContainers_detail::thread_specific_ptr;

  std::cout << "test4\n";
  
  thread_specific_ptr<Payload> p;
  assert (p.get() == 0);
  assert (p.release() == 0);
  p.reset (new Payload(1));
  assert (p.get()->m_i == 1);
  assert (p->m_i == 1);
  assert ((*p).m_i == 1);
  Payload* pp = p.release();
  assert (pp->m_i == 1);
  assert (p.get() == 0);
  p.reset (new Payload(2));
}


int main()
{
  test1();
  test2();
  test3();
  test4();
  return 0;
}
