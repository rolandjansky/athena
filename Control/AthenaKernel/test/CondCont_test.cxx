/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
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

CLASS_DEF(CondContMixed<B>, 932847548, 0)


bool succ (StatusCode sc)
{
  return sc.getCode() == static_cast<StatusCode::code_t>(StatusCode::SUCCESS);
}


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


EventIDBase mixed (int run, int lbn, float t)
{
  return EventIDBase (run,  // run
                      EventIDBase::UNDEFEVT,  // event
                      int(t),
                      int((t - int(t)) * 1e9), // timestamp ns
                      lbn);
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

  assert(succ( cc_rl.typelessInsert (r1, ptrs[0]) ));
  assert(succ( cc_rl.typelessInsert (r2, ptrs[1]) ));
  assert(succ( cc_ts.insert (r3, std::unique_ptr<T> (ptrs[2])) ));
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
       << "{[0,0,t:123] - [t:456]} " << ptrs[2] << "\n";
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
  assert (cc_rl.find (runlbn (10, 17)) == t);

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
  assert (cc_rl.find (runlbn (15, 17)) == nullptr);

  assert (cc_rl.range (runlbn (100, 200), r));
  assert (r == r2);
  assert (cc_ts.range (timestamp (234), r));
  assert (r == r3);
  assert (!cc_rl.range (runlbn (15, 17), r));
  assert (!cc_ts.range (timestamp (999), r));
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
  assert (CondContBase::Category::isDuplicate (sc));
  assert (!CondContBase::Category::isOverlap (sc));

  sc = cc_rl.insert (EventIDRange (runlbn (10, 35), runlbn (10, 45)),
                     std::make_unique<B> (41));
  assert (sc.isSuccess()); 
  assert (!CondContBase::Category::isDuplicate (sc));
  assert (CondContBase::Category::isOverlap (sc));

  assert (cc_ts.insert (EventIDRange (timestamp (100), timestamp (200)),
                        std::make_unique<B> (50)).isSuccess());

  sc = cc_ts.insert (EventIDRange (timestamp (100), timestamp (300)),
                     std::make_unique<B> (60));
  assert (sc.isSuccess());
  assert (CondContBase::Category::isExtended (sc));
  assert (dump_cc(cc_ts) == "{[0,0,t:100] - [t:300]} [50]\n");

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

  assert (cc.extendLastRange (EventIDRange (runlbn (10, 25), runlbn (10, 33))).isFailure());
  assert (dump_cc(cc) == "{[10,l:15] - [10,l:20]} [1]\n{[10,l:30] - [10,l:37]} [2]\n");

  assert (cc.extendLastRange (EventIDRange (runlbn (10, 31), runlbn (10, 33))).isSuccess());
  assert (dump_cc(cc) == "{[10,l:15] - [10,l:20]} [1]\n{[10,l:30] - [10,l:37]} [2]\n");

  assert (cc.extendLastRange (EventIDRange (runlbn (10, 32), runlbn (10,40))).isSuccess());
  assert (dump_cc(cc) == "{[10,l:15] - [10,l:20]} [1]\n{[10,l:30] - [10,l:40]} [2]\n");
}


class CondContMixedTest
  : public CondContMixed<B>
{
public:
  CondContMixedTest (Athena::IRCUSvc& rcusvc, const DataObjID& id)
    : CondContMixed<B> (rcusvc, ClassID_traits<CondContMixed<B> >::ID(),
                        id, nullptr, 16)
  {
  }
};


// Testing overlaps keys.
void test4 (TestRCUSvc& rcusvc)
{
  std::cout << "test4\n";

  SG::DataProxy proxy;
  DataObjID id ("cls", "key");
  EventContext ctx_rl;
  EventContext ctx_ts;

  ctx_rl.setEventID (EventIDBase (runlbn (1, 25)));
  ctx_ts.setEventID (EventIDBase (timestamp (25)));

  CondCont<B> cc_rl (rcusvc, id, &proxy);
  CondCont<B> cc_ts (rcusvc, id, &proxy);

  //=============

  assert (cc_rl.insert (EventIDRange (runlbn (1, 10), runlbn (1, 20)),
                        std::make_unique<B>(1), ctx_rl).isSuccess());
  assert (dump_cc(cc_rl) == "{[1,l:10] - [1,l:20]} [1]\n");
  assert (cc_ts.insert (EventIDRange (timestamp (10), timestamp (20)),
                        std::make_unique<B>(1), ctx_ts).isSuccess());
  assert (dump_cc(cc_ts) == "{[0,0,t:10] - [t:20]} [1]\n");

  //=============

  assert (cc_rl.insert (EventIDRange (runlbn (1, 50), runlbn (1, 60)),
                        std::make_unique<B>(2), ctx_rl).isSuccess());
  assert (dump_cc(cc_rl) == "{[1,l:10] - [1,l:20]} [1]\n{[1,l:50] - [1,l:60]} [2]\n");
  assert (cc_ts.insert (EventIDRange (timestamp (50), timestamp (60)),
                        std::make_unique<B>(2),  ctx_ts).isSuccess());
  assert (dump_cc(cc_ts) == "{[0,0,t:10] - [t:20]} [1]\n{[0,0,t:50] - [t:60]} [2]\n");

  //=============

  StatusCode sc;

  sc = cc_rl.insert (EventIDRange (runlbn (1, 12), runlbn (1, 18)),
                     std::make_unique<B>(3), ctx_rl);
  assert (sc.isSuccess()); 
  assert (CondContBase::Category::isDuplicate (sc));
  assert (!CondContBase::Category::isOverlap (sc));
  assert (dump_cc(cc_rl) == "{[1,l:10] - [1,l:20]} [1]\n{[1,l:50] - [1,l:60]} [2]\n");

  sc = cc_ts.insert (EventIDRange (timestamp (12), timestamp (18)),
                     std::make_unique<B>(3), ctx_rl);
  assert (sc.isSuccess()); 
  assert (CondContBase::Category::isDuplicate (sc));
  assert (!CondContBase::Category::isOverlap (sc));
  assert (dump_cc(cc_ts) == "{[0,0,t:10] - [t:20]} [1]\n{[0,0,t:50] - [t:60]} [2]\n");

  //=============

  sc = cc_rl.insert (EventIDRange (runlbn (1, 8), runlbn (1, 30)),
                     std::make_unique<B>(4), ctx_rl);
  assert (sc.isSuccess()); 
  assert (!CondContBase::Category::isDuplicate (sc));
  assert (CondContBase::Category::isOverlap (sc));
  assert (dump_cc(cc_rl) == "{[1,l:10] - [1,l:20]} [1]\n{[1,l:20] - [1,l:30]} [4]\n{[1,l:50] - [1,l:60]} [2]\n");

  sc = cc_ts.insert (EventIDRange (timestamp (8), timestamp (30)),
                     std::make_unique<B>(4), ctx_rl);
  assert (sc.isSuccess()); 
  assert (!CondContBase::Category::isDuplicate (sc));
  assert (CondContBase::Category::isOverlap (sc));
  assert (dump_cc(cc_ts) == "{[0,0,t:10] - [t:20]} [1]\n{[0,0,t:20,l:0] - [t:30]} [4]\n{[0,0,t:50] - [t:60]} [2]\n");

  //=============

  sc = cc_rl.insert (EventIDRange (runlbn (1, 6), runlbn (1, 40)),
                     std::make_unique<B>(5), ctx_rl);
  assert (sc.isSuccess()); 
  assert (!CondContBase::Category::isDuplicate (sc));
  assert (CondContBase::Category::isOverlap (sc));
  assert (dump_cc(cc_rl) == "{[1,l:10] - [1,l:20]} [1]\n{[1,l:20] - [1,l:30]} [4]\n{[1,l:30] - [1,l:40]} [5]\n{[1,l:50] - [1,l:60]} [2]\n");

  sc = cc_ts.insert (EventIDRange (timestamp (6), timestamp (40)),
                     std::make_unique<B>(5), ctx_rl);
  assert (sc.isSuccess()); 
  assert (!CondContBase::Category::isDuplicate (sc));
  assert (CondContBase::Category::isOverlap (sc));
  assert (dump_cc(cc_ts) == "{[0,0,t:10] - [t:20]} [1]\n{[0,0,t:20,l:0] - [t:30]} [4]\n{[0,0,t:30,l:0] - [t:40]} [5]\n{[0,0,t:50] - [t:60]} [2]\n");

  //=============

  sc = cc_rl.insert (EventIDRange (runlbn (1, 6), runlbn (1, 15)),
                     std::make_unique<B>(6), ctx_rl);
  assert (sc.isSuccess()); 
  assert (!CondContBase::Category::isDuplicate (sc));
  assert (CondContBase::Category::isOverlap (sc));
  assert (dump_cc(cc_rl) == "{[1,l:6] - [1,l:10]} [6]\n{[1,l:10] - [1,l:20]} [1]\n{[1,l:20] - [1,l:30]} [4]\n{[1,l:30] - [1,l:40]} [5]\n{[1,l:50] - [1,l:60]} [2]\n");

  sc = cc_ts.insert (EventIDRange (timestamp (6), timestamp (15)),
                     std::make_unique<B>(6), ctx_rl);
  assert (sc.isSuccess()); 
  assert (!CondContBase::Category::isDuplicate (sc));
  assert (CondContBase::Category::isOverlap (sc));
  assert (dump_cc(cc_ts) == "{[0,0,t:6] - [t:10]} [6]\n{[0,0,t:10] - [t:20]} [1]\n{[0,0,t:20,l:0] - [t:30]} [4]\n{[0,0,t:30,l:0] - [t:40]} [5]\n{[0,0,t:50] - [t:60]} [2]\n");

  //=============

  sc = cc_rl.insert (EventIDRange (runlbn (1, 35), runlbn (1, 45)),
                     std::make_unique<B>(9), ctx_rl);
  assert (sc.isSuccess()); 
  assert (!CondContBase::Category::isDuplicate (sc));
  assert (CondContBase::Category::isOverlap (sc));
  assert (dump_cc(cc_rl) == "{[1,l:6] - [1,l:10]} [6]\n{[1,l:10] - [1,l:20]} [1]\n{[1,l:20] - [1,l:30]} [4]\n{[1,l:30] - [1,l:40]} [5]\n{[1,l:40] - [1,l:45]} [9]\n{[1,l:50] - [1,l:60]} [2]\n");

  sc = cc_ts.insert (EventIDRange (timestamp (35), timestamp (45)),
                     std::make_unique<B>(9), ctx_rl);
  assert (sc.isSuccess()); 
  assert (!CondContBase::Category::isDuplicate (sc));
  assert (CondContBase::Category::isOverlap (sc));
  assert (dump_cc(cc_ts) == "{[0,0,t:6] - [t:10]} [6]\n{[0,0,t:10] - [t:20]} [1]\n{[0,0,t:20,l:0] - [t:30]} [4]\n{[0,0,t:30,l:0] - [t:40]} [5]\n{[0,0,t:40,l:0] - [t:45]} [9]\n{[0,0,t:50] - [t:60]} [2]\n");
}


// Testing mixed keys.
void test5 (TestRCUSvc& rcusvc)
{
  std::cout << "test5\n";
  DataObjID id ("cls", "key");

  std::vector<B*> bptrs;
  for (int i=0; i < 6; i++) {
    bptrs.push_back (new B(i+1));
  }

  // Insert

  CondContMixedTest cc (rcusvc, id);
  assert (cc.keyType() == CondContBase::KeyType::MIXED);

  assert (succ (cc.insert (EventIDRange (mixed(1, 10, 1),
                                         mixed(1, 20, 2)),
                           std::unique_ptr<B>(bptrs[0]))) );
  assert (succ (cc.insert (EventIDRange (mixed(1, 10, 2),
                                         mixed(1, 20, 4.5)),
                           std::unique_ptr<B>(bptrs[1]))) );

  assert (succ (cc.insert (EventIDRange (mixed(1, 30, 25),
                                         mixed(1, 40, 30)),
                           std::unique_ptr<B>(bptrs[2]))) );

  assert (succ (cc.insert (EventIDRange (mixed(2, 10, 100),
                                         mixed(2, 20, 103.5)),
                           std::unique_ptr<B>(bptrs[3]))) );
  assert (succ (cc.insert (EventIDRange (mixed(2, 10, 103.5),
                                         mixed(2, 20, 110)),
                           std::unique_ptr<B>(bptrs[4]))) );
  assert (succ (cc.typelessInsert (EventIDRange (mixed(2, 10, 120),
                                                 mixed(2, 20, 130)),
                                   bptrs[5])) );

  assert (cc.insert (EventIDRange (mixed(2, 10, 150),
                                   mixed(2, 15, 150)),
                     std::make_unique<B>(7)).isFailure());

  StatusCode sc = cc.insert (EventIDRange (mixed(2, 10, 120),
                                           mixed(2, 20, 130)),
                             std::make_unique<B>(9));
  assert (sc.isSuccess());
  assert (CondContBase::Category::isDuplicate (sc));


  // List
  std::ostringstream ss1;
  cc.list (ss1);
  std::ostringstream exp1;
  exp1 << "id:  ( 'UNKNOWN_CLASS:cls' , 'key' )   proxy: 0 [3] run+lbn entries\n";
  exp1 << "{[1,t:1,l:10] - [1,t:2,l:20]} " << bptrs[0] << "\n";
  exp1 << "{[1,t:2,l:10] - [1,t:4.500000000,l:20]} " << bptrs[1] << "\n";
  exp1 << "{[1,t:25,l:30] - [1,t:30,l:40]} " << bptrs[2] << "\n";
  exp1 << "{[2,t:100,l:10] - [2,t:103.500000000,l:20]} " << bptrs[3] << "\n";
  exp1 << "{[2,t:103.500000000,l:10] - [2,t:110,l:20]} " << bptrs[4] << "\n";
  exp1 << "{[2,t:120,l:10] - [2,t:130,l:20]} " << bptrs[5] << "\n";
  //std::cout << "ss1: " << ss1.str() << "\nexp1: " << exp1.str() << "\n";
  assert (ss1.str() == exp1.str());


  // Ranges
  std::vector<EventIDRange> rvec = cc.ranges();
  assert (rvec.size() == 6);
  assert (rvec[0] == EventIDRange (mixed(1, 10,   1),   mixed(1, 20,   2)));
  assert (rvec[1] == EventIDRange (mixed(1, 10,   2),   mixed(1, 20,   4.5)));
  assert (rvec[2] == EventIDRange (mixed(1, 30,  25),   mixed(1, 40,  30)));
  assert (rvec[3] == EventIDRange (mixed(2, 10, 100),   mixed(2, 20, 103.5)));
  assert (rvec[4] == EventIDRange (mixed(2, 10, 103.5), mixed(2, 20, 110)));
  assert (rvec[5] == EventIDRange (mixed(2, 10, 120),   mixed(2, 20, 130)));


  // Find
  const EventIDRange* range = nullptr;
  const B* obj = nullptr;
  assert (!cc.find (runlbn(1, 10), obj, &range));
  assert (!cc.find (timestamp(110), obj, &range));
  assert (cc.find (runlbn(1, 10)) == nullptr);

  assert (cc.find (mixed(1, 12, 3), obj, &range));
  assert (obj->m_x == 2);
  assert (*range == EventIDRange (mixed(1, 10,   2),   mixed(1, 20,   4.5)));
  assert (cc.find (mixed(1, 12, 3)) == obj);

  assert (cc.find (mixed(1, 35, 25), obj, &range));
  assert (obj->m_x == 3);
  assert (*range == EventIDRange (mixed(1, 30,  25),   mixed(1, 40,  30)));
                             
  assert (cc.find (mixed(2, 12, 103.7), obj, &range));
  assert (obj->m_x == 5);
  assert (*range == EventIDRange (mixed(2, 10, 103.5), mixed(2, 20, 110)));

  assert (!cc.find (mixed(1, 12, 10), obj, &range));
  assert (!cc.find (mixed(1,  8,  3), obj, &range));
  assert (!cc.find (mixed(1, 35, 20), obj, &range));
  assert (!cc.find (mixed(2, 15, 115), obj, &range));


  // Valid
  assert (!cc.valid (runlbn(1, 10)));
  assert (!cc.valid (timestamp(110)));

  assert (cc.valid (mixed(1, 12, 3)));
  assert (cc.valid (mixed(1, 35, 25)));
  assert (cc.valid (mixed(2, 12, 103.7)));

  assert (!cc.valid (mixed(1, 12, 10)));
  assert (!cc.valid (mixed(1,  8,  3)));
  assert (!cc.valid (mixed(1, 35, 20)));
  assert (!cc.valid (mixed(2, 15, 115)));


  // Range
  EventIDRange r2;
  assert (cc.range (mixed(1, 35, 25), r2));
  assert (r2 == EventIDRange (mixed(1, 30,  25),   mixed(1, 40,  30)));
  assert (!cc.range (mixed(1, 35, 20), r2));


  // Insert w/overlap
  bptrs.push_back (new B (11));
  sc = cc.insert (EventIDRange (mixed (2, 10, 125),
                                mixed (2, 20, 127)),
                  std::unique_ptr<B> (bptrs.back()));
  assert (CondContBase::Category::isDuplicate (sc));


  bptrs.push_back (new B (11));
  sc = cc.insert (EventIDRange (mixed (2, 10, 125),
                                mixed (2, 20, 135)),
                  std::unique_ptr<B> (bptrs.back()));
  assert (CondContBase::Category::isOverlap (sc));


  // Erase/extendLastRange
  assert (cc.erase (mixed(2, 10, 100)).isFailure());
  assert (cc.extendLastRange (EventIDRange (mixed(2, 10, 125),
                                            mixed(2, 20, 200))).isFailure());

  // Extending input
  // Multiple TS ranges in last RL range.
  assert ( cc.insert (EventIDRange (mixed (2, 10, 120),
                                    mixed (2, 30, 130)),
                      std::make_unique<B> (12)).isFailure() );
  // Insert new last RL range with one TS range.
  bptrs.push_back (new B(13));
  assert ( cc.insert (EventIDRange (mixed (20, 10, 120),
                                    mixed (20, 30, 130)),
                      std::unique_ptr<B> (bptrs.back())).isSuccess() );
  // TS range doesn't match.
  assert ( cc.insert (EventIDRange (mixed (20, 10, 120),
                                    mixed (20, 40, 150)),
                      std::make_unique<B> (14)).isFailure() );
  // RL range isn't last.
  assert ( cc.insert (EventIDRange (mixed (1, 30, 25),
                                    mixed (1, 50, 30)),
                      std::make_unique<B> (15)).isFailure() );
  // Should work.
  sc = cc.insert (EventIDRange (mixed (20, 10, 120),
                                mixed (20, 40, 130)),
                  std::make_unique<B> (16));
  assert (sc.isSuccess());
  assert (CondContBase::Category::isExtended (sc));

  std::ostringstream ss2;
  cc.list (ss2);
  std::ostringstream exp2;
  exp2 << "id:  ( 'UNKNOWN_CLASS:cls' , 'key' )   proxy: 0 [4] run+lbn entries\n";
  exp2 << "{[1,t:1,l:10] - [1,t:2,l:20]} " << bptrs[0] << "\n";
  exp2 << "{[1,t:2,l:10] - [1,t:4.500000000,l:20]} " << bptrs[1] << "\n";
  exp2 << "{[1,t:25,l:30] - [1,t:30,l:40]} " << bptrs[2] << "\n";
  exp2 << "{[2,t:100,l:10] - [2,t:103.500000000,l:20]} " << bptrs[3] << "\n";
  exp2 << "{[2,t:103.500000000,l:10] - [2,t:110,l:20]} " << bptrs[4] << "\n";
  exp2 << "{[2,t:120,l:10] - [2,t:130,l:20]} " << bptrs[5] << "\n";
  exp2 << "{[2,t:130,l:10] - [2,t:135,l:20]} " << bptrs[6] << "\n";
  exp2 << "{[20,t:120,l:10] - [20,t:130,l:40]} " << bptrs[8] << "\n";
  //                                                  xxx 
  //std::cout << "ss2: " << ss2.str() << "\nexp2: " << exp2.str() << "\n";
  assert (ss2.str() == exp2.str());
}


//******************************************************************************


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


//******************************************************************************


class testThread_MixedWriter
  : public testThread_Base
{
public:
  testThread_MixedWriter (int slot, CondContMixedTest& map);
  void operator()();
  EventIDRange makeRange (int i);

private:
  CondContMixedTest& m_map;
};


testThread_MixedWriter::testThread_MixedWriter (int slot, CondContMixedTest& map)
  : testThread_Base (slot),
    m_map (map)
{
}


void testThread_MixedWriter::operator()()
{
  setContext();
  std::shared_lock<std::shared_timed_mutex> lock (start_mutex);

  for (int i=0; i < nwrites; i++) {
    if (i >= ninflight/2) {
      std::vector<CondContBase::key_type> keys;
      keys.reserve (ninflight/2);
      for (int j = i/2-ninflight/2; j<i/2; j++) {
        keys.push_back (j);
      }
      m_map.trim (keys);
    }
    EventIDRange r = makeRange(i);
    int payload = r.start().lumi_block() + r.start().time_stamp();
    assert (m_map.insert (r, std::make_unique<B> (payload), ctx()).isSuccess());
    m_map.quiescent (ctx());
    if (((i+1)%128) == 0) {
      usleep (1000);
    }
  }
}


EventIDRange testThread_MixedWriter::makeRange (int i)
{
  //                  111111111
  //        0123456789012345678  // i
  //
  //        0011223344556677889  // lb
  //        0112233445566778899  // ts

  unsigned int lbn = i / 2;
  unsigned int ts = (i+1)/2 * 1000;
  EventIDBase start (0, 0, ts,   0, lbn);
  EventIDBase stop  (0, 0, ts+1, 0, lbn+1);
  return EventIDRange (start, stop);
}


class testThread_MixedIterator
  : public testThread_Base
{
public:
  testThread_MixedIterator (int slot, CondContMixedTest& map);
  void operator()();

private:
  CondContMixedTest& m_map;
};


testThread_MixedIterator::testThread_MixedIterator (int slot, CondContMixedTest& map)
  : testThread_Base (slot),
    m_map (map)
{
}


void testThread_MixedIterator::operator()()
{
  setContext();
  std::shared_lock<std::shared_timed_mutex> lock (start_mutex);

  bool full = false;
  while (true) {
    int sz = static_cast<int>(m_map.entries());
    if (full) {
      assert (std::abs (sz - ninflight/2) <= 1);
    }
    std::vector<EventIDRange> rvec = m_map.ranges();
    sz = rvec.size();
    if (full) {
      assert (std::abs (sz - ninflight) <= 2);
    }

    if (sz >= ninflight) {
      full = true;
    }

    for (const EventIDRange& r : rvec) {
      const B* obj;
      if (m_map.find (r.start(), obj)) {
        assert (obj->m_x == static_cast<int>(r.start().lumi_block() + r.start().time_stamp()));
      }
    }

    if (sz > 0 && (rvec.end()-1)->start().lumi_block() == (nwrites-1)/2) break;

    m_map.quiescent (ctx());
  }
}


class testThread_MixedReader
  : public testThread_Base
{
public:
  testThread_MixedReader (int slot, CondContMixedTest& map);
  void operator()();

private:
  CondContMixedTest& m_map;
  uint32_t m_seed;
};


testThread_MixedReader::testThread_MixedReader (int slot, CondContMixedTest& map)
  : testThread_Base (slot),
    m_map (map),
    m_seed (slot * 123)
{
}


void testThread_MixedReader::operator()()
{
  setContext();
  std::shared_lock<std::shared_timed_mutex> lock (start_mutex);

  while (true) {
    std::vector<EventIDRange> rvec = m_map.ranges();
    if (rvec.empty()) continue;
    const EventIDBase& stop = (rvec.end()-1)->stop();
    const EventIDBase& start = rvec.begin()->start();
    unsigned int lb = Athena_test::randi_seed (m_seed,
                                               stop.lumi_block()-1,
                                               start.lumi_block());
    unsigned int ts = Athena_test::randi_seed (m_seed,
                                               stop.time_stamp()-1,
                                               start.time_stamp());
    EventIDBase key (0, 0, ts, 0, lb);
    const B* obj = nullptr;
    const EventIDRange* rr = nullptr;
    if (m_map.find (key, obj, &rr)) {
      assert (lb >= rr->start().lumi_block() && lb < rr->stop().lumi_block());
      assert (ts >= rr->start().time_stamp() && ts < rr->stop().time_stamp());
      assert (obj->m_x == static_cast<int> (rr->start().lumi_block() + rr->start().time_stamp()));
    }

    if ((rvec.end()-1)->start().lumi_block() == (nwrites-1)/2) break;
    m_map.quiescent (ctx());
  }
}


void testThreadMixed_iter (TestRCUSvc& rcusvc)
{
  DataObjID id ("cls", "key");
  CondContMixedTest condcont (rcusvc, id);

  const int nthread = 4;
  std::thread threads[nthread];
  start_mutex.lock();

  threads[0] = std::thread (testThread_MixedWriter (0, condcont));
  threads[1] = std::thread (testThread_MixedIterator (1, condcont));
  threads[2] = std::thread (testThread_MixedReader (2, condcont));
  threads[3] = std::thread (testThread_MixedReader (3, condcont));

  // Try to get the threads starting as much at the same time as possible.
  start_mutex.unlock();
  for (int i=0; i < nthread; i++)
    threads[i].join();
}


void testThreadMixed (TestRCUSvc& rcusvc)
{
  std::cout << "testThreadMixed\n";

  for (int i=0; i < 10; i++) {
    testThreadMixed_iter (rcusvc);
  }
}


//******************************************************************************


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
  test4 (rcusvc);
  test5 (rcusvc);
  testThread (rcusvc);
  testThreadMixed (rcusvc);
  return 0;
}
