/*
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file DBLock/test/DBLock_test.xcx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2021
 * @brief Unit tests for DBLock.
 */


#undef NDEBUG
#include "DBLock/DBLock.h"
#include <iostream>
#include <thread>
#include <atomic>
#include <cassert>
#include <unistd.h>


struct Thread
{
  Thread (std::atomic<int>& x) : m_x (x) {}
  void operator()();
  std::atomic<int>& m_x;
};


void Thread::operator()()
{
  Athena::DBLock dblock;
  m_x = 1;
}


void test1()
{
  std::cout << "test1\n";
  std::thread th;
  std::atomic<int> x = 0;
  std::unique_ptr<Athena::DBLock> lockptr;
  {
    Athena::DBLock dblock;
    th = std::thread (Thread (x));
    assert (x == 0);

    Athena::DBLock dblock2 (dblock);
    lockptr = std::make_unique<Athena::DBLock> (dblock);
    sleep (1);
    assert (x == 0);
  }
  sleep (1);
  assert (x == 0);
  lockptr.reset();

  th.join();
  assert (x == 1);
}


int main()
{
  std::cout << "DBLock/DBLock_test\n";
  test1();
  return 0;
}

