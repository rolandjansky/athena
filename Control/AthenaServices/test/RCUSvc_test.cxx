/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  AthenaServices/test/RCUSvc_test.cxx
 * @author scott snyder
 * @date Aug 2016
 * @brief Regression test for RCUSvc
 */


#undef NDEBUG
#include "AthenaBaseComps/AthService.h"
#include "AthenaKernel/IRCUSvc.h"
#include "AthenaKernel/errorcheck.h"
#include "TestTools/initGaudi.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IHiveWhiteBoard.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "boost/thread/shared_mutex.hpp"
#include "boost/thread/shared_lock_guard.hpp"
#include <iostream>
#include <cassert>


using Athena::IRCUSvc;
using Athena::RCUObject;
using Athena::RCURead;
using Athena::RCUUpdate;


const int nslots = 4;
const int nobjs = 3;


class TestWhiteBoard
  : public extends<AthService, IHiveWhiteBoard>
{
public:
  TestWhiteBoard (const std::string& name, ISvcLocator* svc)
    : extends<AthService, IHiveWhiteBoard> (name, svc)
  {}
  
  virtual StatusCode selectStore(size_t /*partitionIndex*/) override { std::abort(); }
  virtual StatusCode clearStore(size_t /*partitionIndex*/) override { std::abort(); }
  virtual StatusCode setNumberOfStores(size_t /*slots*/) override { std::abort(); }
  virtual StatusCode getNewDataObjects(DataObjIDColl& /*products*/) override { std::abort(); }
  virtual bool newDataObjectsPresent() override { std::abort(); }
  virtual size_t allocateStore( int /*evtnumber*/ ) override { std::abort(); }
  virtual StatusCode freeStore( size_t /*partitionIndex*/ ) override { std::abort(); }
  virtual size_t getPartitionNumber(int /*eventnumber*/) const override { std::abort(); }

  virtual size_t getNumberOfStores() const override { return nslots; }
};


DECLARE_SERVICE_FACTORY( TestWhiteBoard )


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
  static bool dolog;

private:
  static std::vector<int> m_dlog;
  static std::mutex m_mutex;
};

std::atomic<int> Payload::ninstance;
bool Payload::dolog = true;
std::vector<int> Payload::m_dlog;
std::mutex Payload::m_mutex;


void test1 (IRCUSvc& svc)
{
  std::cout << "test1\n";
  std::unique_ptr<RCUObject<Payload> > rcuo = svc.newrcu<Payload> (10);
  {
    RCURead<Payload> r (*rcuo);
    r->check(10);
  }
  {
    RCUUpdate<Payload> u (*rcuo);
    u->check(10);
    u.update (std::make_unique<Payload> (2));
    u->check(2);
  }
  {
    RCURead<Payload> r (*rcuo);
    r->check(2);
  }
  assert (Payload::getlog().empty());

  IIncidentListener& listener = dynamic_cast<IIncidentListener&> (svc);
  listener.handle (Incident ("test", IncidentType::EndEvent,
                             EventContext (0, 0)));
  assert (Payload::getlog().empty());
  listener.handle (Incident ("test", IncidentType::EndEvent,
                             EventContext (0, 1)));
  listener.handle (Incident ("test", IncidentType::EndEvent,
                             EventContext (0, 2)));
  assert (Payload::getlog().empty());

  listener.handle (Incident ("test", IncidentType::EndEvent,
                             EventContext (0, 3)));
  assert (Payload::getlog() == std::vector<int>{10});
}


class ThreadedTest
{
public:
  ThreadedTest (IRCUSvc& svc);
  void runtest();

  struct testThread
  {
    testThread (ThreadedTest& test, int iworker, IRCUSvc& svc)
      : m_test(test), m_iworker(iworker), m_svc(svc) {}
    void operator()();

    ThreadedTest& m_test;
    int m_iworker;
    IRCUSvc& m_svc;
  };

private:
  boost::shared_mutex m_sm;
  std::vector<std::unique_ptr<RCUObject<Payload> > > m_objs;
  IRCUSvc& m_svc;
};


ThreadedTest::ThreadedTest (IRCUSvc& svc)
  : m_svc (svc)
{
  for (int i=0; i < nobjs; i++)
    m_objs.push_back (svc.newrcu<Payload>());
}

void ThreadedTest::runtest()
{
  std::thread threads[nslots];
  m_sm.lock();
  for (int i=0; i < nslots; i++)
    threads[i] = std::thread (testThread (*this, i, m_svc));
  // Try to get the threads starting as much at the same time as possible.
  m_sm.unlock();
  for (int i=0; i < nslots; i++)
    threads[i].join();
  IIncidentListener& listener = dynamic_cast<IIncidentListener&> (m_svc);
  for (int i=0; i < nslots; i++) {
    listener.handle (Incident ("test", IncidentType::EndEvent,
                               EventContext (0, i)));
  }
}


void ThreadedTest::testThread::operator()()
{
  boost::shared_lock_guard<boost::shared_mutex> guard (m_test.m_sm);
  Gaudi::Hive::setCurrentContextId (m_iworker);
  IIncidentListener& listener = dynamic_cast<IIncidentListener&> (m_svc);

  for (int i=0; i < 10000; i++) {
    RCUObject<Payload>& obj = *m_test.m_objs[i%nobjs];
    if (i%5 == 0) {
      auto r = obj.reader();
      r->check();
    }
    else if (i%29 == 0) {
      if (i%2 == 0) {
        auto r = obj.readerQuiesce (EventContext (0, m_iworker));
        r->check();
      }
      else {
        auto r = obj.readerQuiesce();
        r->check();
      }
    }
    else if (i%17 == 0) {
      RCUObject<Payload>::Update_t u (obj);
      u.update (std::make_unique<Payload> (u->a));
    }

    if (i%13 == 0) {
      listener.handle (Incident ("test", IncidentType::EndEvent,
                                 EventContext (0, m_iworker)));
    }
  }
}


void test2 (IRCUSvc& svc)
{
  std::cout << "test2\n";
  Payload::dolog = false;
  assert (Payload::ninstance == 0);
  {
    ThreadedTest test (svc);
    for (int i=0; i < 1000; i++)
      test.runtest();
    assert (Payload::ninstance == nobjs);
  }
  assert (Payload::ninstance == 0);
}

  
int main()
{
  errorcheck::ReportMessage::hideErrorLocus();
  ISvcLocator* svcloc = 0;
  if (!Athena_test::initGaudi("RCUSvc_test.txt", svcloc)) {
    std::cerr << "This test can not be run" << std::endl;
    return 1;
  }  
  assert(svcloc);

  ServiceHandle<IRCUSvc> svc ("Athena::RCUSvc", "test");
  if (svc.retrieve().isFailure()) {
    std::cerr << "Can't retrieve service." << std::endl;
    return 1;
  }

  test1 (*svc);
  test2 (*svc);

  return 0;
}
