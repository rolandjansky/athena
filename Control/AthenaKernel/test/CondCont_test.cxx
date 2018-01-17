/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
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
#include "TestTools/random.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <cassert>
#include <iostream>


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
void fillit (CondCont<T>& cc, std::vector<T*> & ptrs)
{
  assert (cc.entries() == 0);
  assert (cc.ranges().empty());
  cc.setProxy (nullptr);
  assert (cc.proxy() == nullptr);
  std::ostringstream ss1;
  cc.list (ss1);
  assert (ss1.str() == "id: 'key'  proxy: 0\n\
clock: [0]\n\
RE: [0]\n");

  assert (cc.clid() == ClassID_traits<CondCont<T> >::ID());

  ptrs.push_back (new T(1));
  ptrs.push_back (new T(2));
  ptrs.push_back (new T(3));

  assert( cc.typelessInsert (r1, ptrs[0]) );
  assert( cc.typelessInsert (r2, ptrs[1]) );
  assert( cc.insert (r3, std::unique_ptr<T> (ptrs[2])) );
  assert (cc.entries() == 3);
  std::ostringstream ss2;
  cc.list (ss2);
  std::ostringstream exp2;
  exp2 << "id: 'key'  proxy: 0\n"
       << "clock: [1]\n"
       << "{[4294967295,t:123] - [4294967295,t:456]} " << ptrs[2] << "\n"
       << "RE: [2]\n"
       << "{[10,l:15] - [10,l:20]} " << ptrs[0] << "\n"
       << "{[20,l:17] - [2147483647,l:2147483647]} " << ptrs[1] << "\n";

  //  std::cout << "ss2: " << ss2.str() << "\nexp2: " << exp2.str() << "\n";

  assert (ss2.str() == exp2.str());

  auto t4 = std::make_unique<T> (4);
  assert( ! cc.insert (EventIDRange (runlbn (40, 2), timestamp (543)), std::move(t4)) );
}


template <class T>
void checkit (const CondCont<T>& cc, const std::vector<T*>& ptrs)
{
  std::vector<EventIDRange> rvec = cc.ranges();
  assert (rvec.size() == 3);
  assert (rvec[0] == r1);
  assert (rvec[1] == r2);
  assert (rvec[2] == r3);

  assert (cc.valid (runlbn (10, 17)));
  assert (cc.valid (runlbn (100, 200)));
  assert (cc.valid (timestamp (234)));
  assert (!cc.valid (runlbn (15, 17)));
  assert (!cc.valid (timestamp (999)));

  const T* t = nullptr;
  assert (cc.find (runlbn (10, 17), t));
  assert (t == ptrs[0]);

  t = nullptr;
  assert (cc.find (runlbn (100, 200), t));
  assert (t == ptrs[1]);

  EventIDRange r;
  const EventIDRange* rp = nullptr;
  t = nullptr;
  assert (cc.find (timestamp (234), t, &rp));
  assert (t == ptrs[2]);
  assert (*rp == r3);

  assert (!cc.find (runlbn (15, 17), t));
  assert (!cc.find (timestamp (999), t));

  assert (cc.range (runlbn (100, 200), r));
  assert (r == r2);
  assert (cc.range (timestamp (234), r));
  assert (r == r3);
  assert (!cc.range (runlbn (15, 17), r));
  assert (!cc.range (timestamp (999), r));
}


void test1 (TestRCUSvc& rcusvc)
{
  std::cout << "test1\n";
  SG::DataProxy proxy;
  DataObjID id ("key");
  CondCont<B> cc (rcusvc, id, &proxy);
  assert (cc.proxy() == &proxy);
  assert (cc.id() == id);

  std::vector<B*> bptrs;
  fillit (cc, bptrs);
  checkit (cc, bptrs);
}


void test2 (TestRCUSvc& rcusvc)
{
  std::cout << "test2\n";
  SG::DataProxy proxy;
  DataObjID id ("key");
  CondCont<D> cc (rcusvc, id, &proxy);
  assert (cc.proxy() == &proxy);
  assert (cc.id() == id);

  std::vector<D*> dptrs;
  fillit (cc, dptrs);
  checkit (cc, dptrs);

  CondCont<B> &bcc = cc;
  std::vector<B*> bptrs (dptrs.begin(), dptrs.end());
  checkit (bcc, bptrs);

  auto b4 = std::make_unique<B> (4);
  assert( ! bcc.insert (EventIDRange (runlbn (40, 2), timestamp (543)), std::move(b4)) );
}


std::shared_timed_mutex start_mutex;


class test3_Base
{
public:
  test3_Base (int slot);
  const EventContext& ctx() const { return m_ctx; }
  void setContext();

  
private:
  EventContext m_ctx;
};


test3_Base::test3_Base (int slot)
  : m_ctx (0, slot)
{
}


void test3_Base::setContext()
{
  Gaudi::Hive::setCurrentContext (m_ctx);
}


class test3_Writer
  : public test3_Base
{
public:
  test3_Writer (int slot, CondCont<B>& map);
  void operator()();
  EventIDRange makeRange (int i);

private:
  CondCont<B>& m_map;
};


test3_Writer::test3_Writer (int slot, CondCont<B>& map)
  : test3_Base (slot),
    m_map (map)
{
}


void test3_Writer::operator()()
{
  setContext();
  std::shared_lock<std::shared_timed_mutex> lock (start_mutex);

  for (int i=0; i < nwrites; i++) {
    if (i >= ninflight) {
      EventIDRange rr = makeRange(i-ninflight);
      m_map.erase (rr.start(), ctx());
    }
    EventIDRange r = makeRange(i);
    m_map.insert (r, std::make_unique<B> (i), ctx());
    m_map.quiescent (ctx());
    if (((i+1)%128) == 0) {
      usleep (1000);
    }
  }
}


EventIDRange test3_Writer::makeRange (int i)
{
  EventIDBase start (0, 0, EventIDBase::UNDEFNUM, 0, i*10);
  EventIDBase stop  (0, 0, EventIDBase::UNDEFNUM, 0, (i+1)*10);
  return EventIDRange (start, stop);
}


class test3_Iterator
  : public test3_Base
{
public:
  test3_Iterator (int slot, CondCont<B>& map);
  void operator()();

private:
  CondCont<B>& m_map;
};


test3_Iterator::test3_Iterator (int slot, CondCont<B>& map)
  : test3_Base (slot),
    m_map (map)
{
}


void test3_Iterator::operator()()
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


class test3_Reader
  : public test3_Base
{
public:
  test3_Reader (int slot, CondCont<B>& map);
  void operator()();

private:
  CondCont<B>& m_map;
  uint32_t m_seed;
};


test3_Reader::test3_Reader (int slot, CondCont<B>& map)
  : test3_Base (slot),
    m_map (map),
    m_seed (slot * 123)
{
}


void test3_Reader::operator()()
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


void test3_iter (TestRCUSvc& rcusvc)
{
  DataObjID id ("key");
  CondCont<B> condcont (rcusvc, id, nullptr, 20);

  const int nthread = 4;
  std::thread threads[nthread];
  start_mutex.lock();

  threads[0] = std::thread (test3_Writer (0, condcont));
  threads[1] = std::thread (test3_Iterator (1, condcont));
  threads[2] = std::thread (test3_Reader (2, condcont));
  threads[3] = std::thread (test3_Reader (3, condcont));

  // Try to get the threads starting as much at the same time as possible.
  start_mutex.unlock();
  for (int i=0; i < nthread; i++)
    threads[i].join();
}


void test3 (TestRCUSvc& rcusvc)
{
  std::cout << "test3\n";

  for (int i=0; i < 10; i++) {
    test3_iter (rcusvc);
  }
}


int main()
{
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
  return 0;
}
