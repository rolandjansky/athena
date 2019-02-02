/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  RCUObject_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2016
 * @brief Regression tests for RCUObject.
 */

#undef NDEBUG

#include "AthenaKernel/RCUObject.h"
#include "AthenaKernel/IRCUSvc.h"
#include "CxxUtils/checker_macros.h"
#include "boost/thread/shared_mutex.hpp"
#include "boost/thread/shared_lock_guard.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>


using Athena::IRCUObject;
using Athena::RCUObject;
using Athena::IRCUSvc;
using Athena::RCURead;
using Athena::RCUReadQuiesce;
using Athena::RCUUpdate;


const int nslots = 4;


class TestRCUSvc
  : public IRCUSvc
{
public:
  virtual StatusCode remove (IRCUObject* obj) override
  {
    m_removed = obj;
    return StatusCode::SUCCESS;
  }
  virtual size_t getNumSlots() const override
  { return nslots; }
  virtual void add (IRCUObject* /*obj*/) override
  { }

  virtual unsigned long addRef() override { std::abort(); }
  virtual unsigned long release() override { std::abort(); }
  virtual StatusCode queryInterface(const InterfaceID &/*ti*/, void** /*pp*/) override { std::abort(); }

  IRCUObject* m_removed = nullptr;
};


struct Payload
{
  Payload (int x=0)
    : a(x), b(x), c(x), d(x)
  {
    ++ninstance;
  }
  Payload& operator= (const Payload&) = default;
  ~Payload()
  {
    --ninstance;
    if (dolog) {
      std::lock_guard<std::mutex> g (m_mutex);
      m_dlog.push_back(a);
    }
  }
  static
  std::vector<int> getlog()
  {
    std::lock_guard<std::mutex> g (m_mutex);
    std::vector<int> log;
    log.swap (m_dlog);
    return log;
  }
  void check(int x) const
  {
    assert (a == x);
    assert (b == x);
    assert (c == x);
    assert (d == x);
  }
  void check() const
  {
    check(a);
  }
  int a, b, c, d;

  static std::atomic<int> ninstance;
  static std::atomic<bool> dolog;

private:
  static std::vector<int> m_dlog ATLAS_THREAD_SAFE;
  static std::mutex m_mutex ATLAS_THREAD_SAFE;
};

std::atomic<int> Payload::ninstance;
std::atomic<bool> Payload::dolog (true);
std::vector<int> Payload::m_dlog;
std::mutex Payload::m_mutex;


void test1()
{
  std::cout << "test1\n";

  TestRCUSvc svc;
  {
    RCUObject<Payload>* optr = nullptr;
    {
      RCUObject<Payload> rcuo1 (svc, 3);
      assert (svc.m_removed == nullptr);
      RCUObject<Payload> rcuo (std::move (rcuo1));
      assert (svc.m_removed == &rcuo1);
      svc.m_removed = nullptr;
      
      RCURead<Payload> r (rcuo);
      r->check(3);
      
      optr = &rcuo;
      assert (svc.m_removed == nullptr);
    }
    // cppcheck-suppress invalidLifetime
    // cppcheck-suppress deadpointer
    assert (svc.m_removed == optr);
  }
  Payload::getlog();
}


void test2()
{
  std::cout << "test2\n";
  TestRCUSvc svc;


  RCUObject<Payload> rcuo (svc);
  {
    RCURead<Payload> r (rcuo);
    r->check(0);
  }
  {
    EventContext ctx (0, 1);
    RCUUpdate<Payload> u (rcuo, ctx);
    u->check(0);
    u.update (std::make_unique<Payload> (2));
    u->check(2);
  }
  {
    RCURead<Payload> r (rcuo);
    r->check(2);
  }
  {
    EventContext ctx (0, 2);
    RCUReadQuiesce<Payload> r (rcuo, ctx);
    r->check(2);
  }
  assert (Payload::getlog().empty());
  rcuo.quiescent (EventContext (0, 0));
  assert (Payload::getlog().empty());
  Gaudi::Hive::setCurrentContextId(3);
  rcuo.quiescent ();
  assert (Payload::getlog() == std::vector<int>{0});

  rcuo.discard (std::make_unique<Payload> (3));
  {
    RCURead<Payload> r (rcuo);
    r->check(2);
  }
  assert (Payload::getlog().empty());
  rcuo.quiescent (EventContext (0, 1));
  assert (Payload::getlog().empty());
  rcuo.quiescent (EventContext (0, 2));
  assert (Payload::getlog().empty());
  rcuo.quiescent (EventContext (0, 3));
  assert (Payload::getlog().empty());
  rcuo.quiescent (EventContext (0, 0));
  assert (Payload::getlog() == std::vector<int>{3});
}


class ThreadedTest
{
public:
  ThreadedTest (TestRCUSvc& svc);
  void runtest();

  struct testThread
  {
    testThread (ThreadedTest& test, int iworker)
      : m_test(test), m_iworker(iworker) {}
    void operator()();

    ThreadedTest& m_test;
    int m_iworker;
  };

private:
  boost::shared_mutex m_sm;
  RCUObject<Payload> m_rcuobj;
};


ThreadedTest::ThreadedTest (TestRCUSvc& svc)
  : m_rcuobj(svc)
{
}

void ThreadedTest::runtest()
{
  std::thread threads[nslots];
  m_sm.lock();
  for (int i=0; i < nslots; i++)
    threads[i] = std::thread (testThread (*this, i));
  // Try to get the threads starting as much at the same time as possible.
  m_sm.unlock();
  for (int i=0; i < nslots; i++)
    threads[i].join();
  for (int i=0; i < nslots; i++)
    m_rcuobj.quiescent (EventContext (0, i));
}


void ThreadedTest::testThread::operator()()
{
  boost::shared_lock_guard<boost::shared_mutex> guard (m_test.m_sm);
  Gaudi::Hive::setCurrentContextId (m_iworker);

  for (int i=0; i < 10000; i++) {
    if (i%5 == 0) {
      auto r = m_test.m_rcuobj.reader();
      r->check();
    }
    else if (i%29 == 0) {
      if (i%2 == 0) {
        EventContext ctx (0, m_iworker);
        auto r = m_test.m_rcuobj.readerQuiesce (ctx);
        r->check();
      }
      else {
        auto r = m_test.m_rcuobj.readerQuiesce();
        r->check();
      }
    }
    else if (i%17 == 0) {
      EventContext ctx (0, m_iworker);
      RCUObject<Payload>::Update_t u (m_test.m_rcuobj, ctx);
      u.update (std::make_unique<Payload> (u->a));
    }
    else if (i%29 == 0) {
      m_test.m_rcuobj.discard (std::make_unique<Payload> (i));
    }

    if (i%13 == 0)
      m_test.m_rcuobj.quiescent (EventContext (0, m_iworker));
  }
}


void test3()
{
  std::cout << "test3\n";

  Payload::dolog = false;
  assert (Payload::ninstance == 0);
  {
    TestRCUSvc svc;
    ThreadedTest test (svc);
    for (int i=0; i < 1000; i++)
      test.runtest();
    assert (Payload::ninstance == 1);
  }
  assert (Payload::ninstance == 0);
}


int main()
{
  test1();
  test2();
  test3();
  return 0;
}

