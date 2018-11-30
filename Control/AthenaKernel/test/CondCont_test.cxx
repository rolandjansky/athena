/*
 * Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file AthenaKernel/test/CondCont_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2017
 * @brief Tests for CondCont.
 */


#undef NDEBUG
#include "AthenaKernel/CondCont.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/getMessageSvc.h"
#include "TestTools/random.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "GaudiKernel/Service.h"
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <cassert>
#include <iostream>
#include <atomic>


namespace SG {
class DataProxy {};
}


const int nslots = 4;
const int nwrites = 10000;
const int ninflight = 10;


class TestRCUSvc
  : public Athena::IRCUSvc
{
public:
  virtual StatusCode remove (Athena::IRCUObject* obj) override
  {
    m_removed = obj;
    return StatusCode::SUCCESS;
  }
  virtual size_t getNumSlots() const override
  { return nslots; }
  virtual void add (Athena::IRCUObject* /*obj*/) override
  { }

  virtual unsigned long addRef() override { std::abort(); }
  virtual unsigned long release() override { std::abort(); }
  virtual StatusCode queryInterface(const InterfaceID &/*ti*/, void** /*pp*/) override { std::abort(); }

  Athena::IRCUObject* m_removed = nullptr;
};


class ConditionsCleanerTest
  : public extends<Service, Athena::IConditionsCleanerSvc>
{
public:
  ConditionsCleanerTest (const std::string& name,
                         ISvcLocator* svcloc)
    : base_class (name, svcloc)
  {}
  virtual StatusCode event (const EventContext& ctx,
                            bool allowAsync) override;
  virtual StatusCode condObjAdded (const EventContext& ctx,
                                   CondContBase& cc) override;

  virtual StatusCode printStats() const override
  { std::abort(); }
  virtual StatusCode reset() override
  { std::abort(); }

  static std::atomic<int> s_nobj;
};

std::atomic<int> ConditionsCleanerTest::s_nobj;


StatusCode ConditionsCleanerTest::event (const EventContext& /*ctx*/,
                                         bool /*allowAsync*/)
{
  return StatusCode::SUCCESS;
}


StatusCode ConditionsCleanerTest::condObjAdded (const EventContext& /*ctx*/,
                                                CondContBase& /*cc*/)
{
  ++s_nobj;
  return StatusCode::SUCCESS;
}


DECLARE_COMPONENT( ConditionsCleanerTest )


class B
{
public:
  B(int x) : m_x(x) {}
  int m_x;
};


class Pad
{
public:
  int m_pad = 0;
};


class D
  : public Pad, public B
{
public:
  D(int x) : B(x*10) {}
};


CONDCONT_BASE(D, B);


CLASS_DEF(CondCont<B>, 932847546, 0)
CLASS_DEF(CondCont<D>, 932847547, 0)


EventIDBase runlbn (int run, int lbn)
{
  return EventIDBase (run,
                      EventIDBase::UNDEFEVT,  // event
                      EventIDBase::UNDEFNUM,  // timestamp
                      EventIDBase::UNDEFNUM,  // timestamp ns
                      lbn);
}


EventIDBase timestamp (int t)
{
  return EventIDBase (EventIDBase::UNDEFNUM,  // run
                      EventIDBase::UNDEFEVT,  // event
                      t);
}


const EventIDRange r1 (runlbn (10, 15), runlbn (10, 20));
const EventIDRange r2 (runlbn (20, 17), runlbn (EventIDBase::UNDEFNUM/2, EventIDBase::UNDEFNUM/2));
const EventIDRange r3 (timestamp (123), timestamp (456));


template <class T>
void fillit (CondCont<T>& cc_rl, CondCont<T>& cc_ts, std::vector<T*> & ptrs)
{
  int nsave = ConditionsCleanerTest::s_nobj;
  assert (cc_rl.entries() == 0);
  assert (cc_rl.ranges().empty());
  cc_rl.setProxy (nullptr);
  assert (cc_rl.proxy() == nullptr);

  assert (cc_ts.entries() == 0);
  assert (cc_ts.ranges().empty());
  cc_ts.setProxy (nullptr);
  assert (cc_ts.proxy() == nullptr);

  std::ostringstream ss1;
  cc_rl.list (ss1);
  assert (ss1.str() == "id:  ( 'UNKNOWN_CLASS:cls' , 'key' )   proxy: 0 [0] entries\n");

  std::ostringstream ss2;
  cc_ts.list (ss2);
  assert (ss2.str() == "id:  ( 'UNKNOWN_CLASS:cls' , 'key' )   proxy: 0 [0] entries\n");

  assert (cc_rl.clid() == ClassID_traits<CondCont<T> >::ID());
  assert (cc_ts.clid() == ClassID_traits<CondCont<T> >::ID());

  ptrs.push_back (new T(1));
  ptrs.push_back (new T(2));
  ptrs.push_back (new T(3));

  assert( cc_rl.typelessInsert (r1, ptrs[0]).isSuccess() );
  assert( cc_rl.typelessInsert (r2, ptrs[1]).isSuccess() );
  assert( cc_ts.insert (r3, std::unique_ptr<T> (ptrs[2])).isSuccess() );
  {
    StatusCode sc = cc_ts.insert (r3, std::make_unique<T> (99));
    assert (sc.isSuccess());
    assert (CondContBase::Category::isDuplicate (sc));
  }
  assert (cc_ts.entries() == 1);
  assert (cc_rl.entries() == 2);

  std::ostringstream ss3;
  cc_rl.list (ss3);
  std::ostringstream exp3;
  exp3 << "id:  ( 'UNKNOWN_CLASS:cls' , 'key' )   proxy: 0 [2] entries\n"
       << "{[10,l:15] - [10,l:20]} " << ptrs[0] << "\n"
       << "{[20,l:17] - [2147483647,l:2147483647]} " << ptrs[1] << "\n";
  //std::cout << "ss3: " << ss3.str() << "\nexp3: " << exp3.str() << "\n";
  assert (ss3.str() == exp3.str());

  std::ostringstream ss4;
  cc_ts.list (ss4);
  std::ostringstream exp4;
  exp4 << "id:  ( 'UNKNOWN_CLASS:cls' , 'key' )   proxy: 0 [1] entries\n"
       << "{[t:123] - [t:456]} " << ptrs[2] << "\n";
  //std::cout << "ss4: " << ss4.str() << "\nexp4: " << exp4.str() << "\n";
  assert (ss4.str() == exp4.str());

  auto t4 = std::make_unique<T> (4);
  assert( ! cc_rl.insert (EventIDRange (runlbn (40, 2), timestamp (543)), std::move(t4)).isSuccess() );
  assert (ConditionsCleanerTest::s_nobj - nsave == 3);
}


template <class T>
void checkit (const CondCont<T>& cc_rl,
              const CondCont<T>& cc_ts,
              const std::vector<T*>& ptrs)
{
  std::vector<EventIDRange> rvec = cc_rl.ranges();
  assert (rvec.size() == 2);
  assert (rvec[0] == r1);
  assert (rvec[1] == r2);

  rvec = cc_ts.ranges();
  assert (rvec.size() == 1);
  assert (rvec[0] == r3);

  assert (cc_rl.valid (runlbn (10, 17)));
  assert (cc_rl.valid (runlbn (100, 200)));
  assert (cc_ts.valid (timestamp (234)));
  assert (!cc_rl.valid (runlbn (15, 17)));
  assert (!cc_ts.valid (timestamp (999)));

  const T* t = nullptr;
  assert (cc_rl.find (runlbn (10, 17), t));
  assert (t == ptrs[0]);

  t = nullptr;
  assert (cc_rl.find (runlbn (100, 200), t));
  assert (t == ptrs[1]);

  EventIDRange r;
  const EventIDRange* rp = nullptr;
  t = nullptr;
  assert (cc_ts.find (timestamp (234), t, &rp));
  assert (t == ptrs[2]);
  assert (*rp == r3);

  assert (!cc_rl.find (runlbn (15, 17), t));
  assert (!cc_ts.find (timestamp (999), t));

  assert (cc_rl.range (runlbn (100, 200), r));
  assert (r == r2);
  assert (cc_ts.range (timestamp (234), r));
  assert (r == r3);
  assert (!cc_rl.range (runlbn (15, 17), r));
  assert (!cc_ts.range (timestamp (999), r));
}


void test1 (TestRCUSvc& rcusvc)
{
  std::cout << "test1\n";
  SG::DataProxy proxy;
  DataObjID id ("cls", "key");
  CondCont<B> cc_rl (rcusvc, id, &proxy);
  assert (cc_rl.proxy() == &proxy);
  assert (cc_rl.id() == id);
  assert (cc_rl.keyType() == CondContBase::KeyType::SINGLE);

  CondCont<B> cc_ts (rcusvc, id, &proxy);
  assert (cc_ts.proxy() == &proxy);
  assert (cc_ts.id() == id);
  assert (cc_ts.keyType() == CondContBase::KeyType::SINGLE);

  std::vector<B*> bptrs;
  fillit (cc_rl, cc_ts, bptrs);
  checkit (cc_rl, cc_ts, bptrs);

  assert (cc_rl.nInserts() == 2);
  assert (cc_rl.maxSize() == 2);

  assert (cc_ts.nInserts() == 1);
  assert (cc_ts.maxSize() == 1);

  const EventIDRange r4 (timestamp (800), timestamp (899));
  assert( cc_ts.typelessInsert (r4, new B(4)).isSuccess() );
  assert (cc_ts.entries() == 2);

  std::vector<CondCont<B>::key_type> keys1 =
    { CondContBase::keyFromRunLBN (r2.start()) };
  std::vector<CondCont<B>::key_type> keys2 =
    { CondContBase::keyFromTimestamp (r4.start()) };
  assert (cc_rl.trim (keys1) == 1);
  assert (cc_ts.trim (keys2) == 1);
  assert (cc_rl.entries() == 1);
  assert (cc_ts.entries() == 1);

  assert (cc_rl.trim (keys1) == 0);
  assert (cc_ts.trim (keys2) == 0);

  //*** Test errors from erase().
  assert (cc_rl.erase (timestamp (800)).isFailure());
  assert (cc_ts.erase (runlbn (100, 200)).isFailure());

  CondCont<B> cc_empty (rcusvc, id, &proxy);
  assert (cc_empty.erase (timestamp (800)).isSuccess());
  assert (cc_empty.erase (runlbn (100, 200)).isSuccess());

  assert (cc_rl.erase (runlbn (20, 17)).isSuccess());
  assert (cc_ts.erase (timestamp (800)).isSuccess());

  assert (cc_rl.entries() == 0);
  assert (cc_ts.entries() == 0);

  //*** Test errors from insert().
  assert (cc_rl.insert (EventIDRange (timestamp (800), timestamp (900)),
                        std::make_unique<B> (10)).isFailure());
  assert (cc_ts.insert (EventIDRange (runlbn (10, 20), runlbn (10, 30)),
                        std::make_unique<B> (20)).isFailure());

  StatusCode sc;
  sc = cc_rl.insert (EventIDRange (runlbn (10, 20), runlbn (10, 40)),
                     std::make_unique<B> (30));
  assert (sc.isSuccess()); 
  assert (!CondContBase::Category::isDuplicate (sc));
  assert (!CondContBase::Category::isOverlap (sc));

  sc = cc_rl.insert (EventIDRange (runlbn (10, 35), runlbn (10, 39)),
                     std::make_unique<B> (40));
  assert (sc.isSuccess()); 
  assert (!CondContBase::Category::isDuplicate (sc));
  assert (CondContBase::Category::isOverlap (sc));

  assert (cc_ts.insert (EventIDRange (timestamp (100), timestamp (200)),
                        std::make_unique<B> (50)).isSuccess());

  //*** Test errors from find().
  const B* b = nullptr;
  assert (!cc_rl.find (timestamp (100), b));
  assert (!cc_ts.find (runlbn (5, 6), b));
  assert (!cc_empty.find (timestamp (100), b));
  assert (!cc_empty.find (runlbn (5, 6), b));

  //*** Test errors from extendLastRange().
  assert (cc_rl.extendLastRange (EventIDRange (timestamp (800), timestamp (900))).isFailure());
  assert (cc_ts.extendLastRange (EventIDRange (runlbn (50, 10), runlbn (50, 20))).isFailure());
}


void test2 (TestRCUSvc& rcusvc)
{
  std::cout << "test2\n";
  SG::DataProxy proxy;
  DataObjID id ("cls", "key");

  CondCont<D> cc_rl (rcusvc, id, &proxy);
  assert (cc_rl.proxy() == &proxy);
  assert (cc_rl.id() == id);

  CondCont<D> cc_ts (rcusvc, id, &proxy);
  assert (cc_ts.proxy() == &proxy);
  assert (cc_ts.id() == id);

  std::vector<D*> dptrs;
  fillit (cc_rl, cc_ts, dptrs);
  checkit (cc_rl, cc_ts, dptrs);

  std::vector<B*> bptrs (dptrs.begin(), dptrs.end());
  CondCont<B> &bcc_rl = cc_rl;
  CondCont<B> &bcc_ts = cc_ts;
  checkit (bcc_rl, bcc_ts, bptrs);

  auto b4 = std::make_unique<B> (4);
  assert( ! bcc_rl.insert (EventIDRange (runlbn (40, 2), runlbn (40, 5)), std::move(b4)).isSuccess() );
}


std::string dump_cc (const CondCont<B>& cc)
{
  std::ostringstream ss;
  for (const EventIDRange& r : cc.ranges()) {
    const B* p = nullptr;
    assert (cc.find (r.start(), p));
    ss << r << " [" << p->m_x << "]\n";
  }
  return ss.str();
}


// Test an extensible container.
void test3 (TestRCUSvc& rcusvc)
{
  std::cout << "test3\n";
  DataObjID id ("cls", "key");
  CondCont<B> cc (rcusvc, id);

  const B* b = nullptr;
  assert (!cc.find (runlbn (10, 15), b));

  assert (cc.insert (EventIDRange (runlbn (10, 15), runlbn (10, 20)),
                     std::make_unique<B>(1)));
  assert (cc.insert (EventIDRange (runlbn (10, 30), runlbn (10, 35)),
                     std::make_unique<B>(2)));
  assert (dump_cc(cc) == "{[10,l:15] - [10,l:20]} [1]\n{[10,l:30] - [10,l:35]} [2]\n");

  assert (cc.extendLastRange (EventIDRange (runlbn (10, 30), runlbn (10, 37))).isSuccess());
  assert (dump_cc(cc) == "{[10,l:15] - [10,l:20]} [1]\n{[10,l:30] - [10,l:37]} [2]\n");

  assert (cc.extendLastRange (EventIDRange (runlbn (10, 30), runlbn (10, 33))).isSuccess());
  assert (dump_cc(cc) == "{[10,l:15] - [10,l:20]} [1]\n{[10,l:30] - [10,l:37]} [2]\n");

  assert (cc.extendLastRange (EventIDRange (runlbn (10, 31), runlbn (10, 33))).isFailure());
  assert (dump_cc(cc) == "{[10,l:15] - [10,l:20]} [1]\n{[10,l:30] - [10,l:37]} [2]\n");
}


//*******************************************************************************


std::shared_timed_mutex start_mutex;


class testThread_Base
{
public:
  testThread_Base (int slot);
  const EventContext& ctx() const { return m_ctx; }
  void setContext();

  
private:
  EventContext m_ctx;
};


testThread_Base::testThread_Base (int slot)
  : m_ctx (0, slot)
{
}


void testThread_Base::setContext()
{
  Gaudi::Hive::setCurrentContext (m_ctx);
}


class testThread_Writer
  : public testThread_Base
{
public:
  testThread_Writer (int slot, CondCont<B>& map);
  void operator()();
  EventIDRange makeRange (int i);

private:
  CondCont<B>& m_map;
};


testThread_Writer::testThread_Writer (int slot, CondCont<B>& map)
  : testThread_Base (slot),
    m_map (map)
{
}


void testThread_Writer::operator()()
{
  setContext();
  std::shared_lock<std::shared_timed_mutex> lock (start_mutex);

  for (int i=0; i < nwrites; i++) {
    if (i >= ninflight) {
      EventIDRange rr = makeRange(i-ninflight);
      m_map.erase (rr.start(), ctx()).ignore();
    }
    EventIDRange r = makeRange(i);
    assert (m_map.insert (r, std::make_unique<B> (i), ctx()).isSuccess());
    m_map.quiescent (ctx());
    if (((i+1)%128) == 0) {
      usleep (1000);
    }
  }
}


EventIDRange testThread_Writer::makeRange (int i)
{
  EventIDBase start (0, 0, EventIDBase::UNDEFNUM, 0, i*10);
  EventIDBase stop  (0, 0, EventIDBase::UNDEFNUM, 0, (i+1)*10);
  return EventIDRange (start, stop);
}


class testThread_Iterator
  : public testThread_Base
{
public:
  testThread_Iterator (int slot, CondCont<B>& map);
  void operator()();

private:
  CondCont<B>& m_map;
};


testThread_Iterator::testThread_Iterator (int slot, CondCont<B>& map)
  : testThread_Base (slot),
    m_map (map)
{
}


void testThread_Iterator::operator()()
{
  setContext();
  std::shared_lock<std::shared_timed_mutex> lock (start_mutex);

  bool full = false;
  while (true) {
    int sz = static_cast<int>(m_map.entries());
    if (full) {
      assert (std::abs (sz - ninflight) <= 1);
    }
    std::vector<EventIDRange> rvec = m_map.ranges();
    sz = rvec.size();
    if (full) {
      assert (std::abs (sz - ninflight) <= 1);
    }

    if (sz >= ninflight) {
      full = true;
    }

    for (const EventIDRange& r : rvec) {
      const B* obj;
      if (m_map.find (r.start(), obj)) {
        assert (static_cast<int>(r.start().lumi_block()) == static_cast<int>(obj->m_x*10));
      }
    }

    if (sz > 0 && (rvec.end()-1)->start().lumi_block() == (nwrites-1)*10) break;

    m_map.quiescent (ctx());
  }
}


class testThread_Reader
  : public testThread_Base
{
public:
  testThread_Reader (int slot, CondCont<B>& map);
  void operator()();

private:
  CondCont<B>& m_map;
  uint32_t m_seed;
};


testThread_Reader::testThread_Reader (int slot, CondCont<B>& map)
  : testThread_Base (slot),
    m_map (map),
    m_seed (slot * 123)
{
}


void testThread_Reader::operator()()
{
  setContext();
  std::shared_lock<std::shared_timed_mutex> lock (start_mutex);

  while (true) {
    std::vector<EventIDRange> rvec = m_map.ranges();
    if (rvec.empty()) continue;
    int ikey = Athena_test::randi_seed (m_seed,
                                        (rvec.end()-1)->stop().lumi_block()-1,
                                        rvec.begin()->start().lumi_block());
    EventIDBase key (0, 0, EventIDBase::UNDEFNUM, 0, ikey);
    const B* obj = nullptr;
    const EventIDRange* rr = nullptr;
    if (m_map.find (key, obj, &rr)) {
      assert (key >= rr->start() && key < rr->stop());
      assert (static_cast<int>(rr->start().lumi_block()) == static_cast<int>(obj->m_x*10));
    }

    if ((rvec.end()-1)->start().lumi_block() == (nwrites-1)*10) break;
    m_map.quiescent (ctx());
  }
}


void testThread_iter (TestRCUSvc& rcusvc)
{
  DataObjID id ("cls", "key");
  CondCont<B> condcont (rcusvc, id, nullptr, 20);

  const int nthread = 4;
  std::thread threads[nthread];
  start_mutex.lock();

  threads[0] = std::thread (testThread_Writer (0, condcont));
  threads[1] = std::thread (testThread_Iterator (1, condcont));
  threads[2] = std::thread (testThread_Reader (2, condcont));
  threads[3] = std::thread (testThread_Reader (3, condcont));

  // Try to get the threads starting as much at the same time as possible.
  start_mutex.unlock();
  for (int i=0; i < nthread; i++)
    threads[i].join();
}


void testThread (TestRCUSvc& rcusvc)
{
  std::cout << "testThread\n";

  for (int i=0; i < 10; i++) {
    testThread_iter (rcusvc);
  }
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  CondContBase::setCleanerSvcName ("ConditionsCleanerTest");
  Athena::getMessageSvcQuiet = true;

  // Verify that B is indeed offset in D, so we get to test a nontrivial conversion.
  {
    D d(0);
    B& bd = d;
    assert (reinterpret_cast<char*>(&d) != reinterpret_cast<char*>(&bd));
  }

  TestRCUSvc rcusvc;
  test1 (rcusvc);
  test2 (rcusvc);
  test3 (rcusvc);
  testThread (rcusvc);
  return 0;
}
