/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file  AthenaServices/test/DelayedConditionsCleanerSvc_test.cxx
 * @author scott snyder
 * @date May 2018
 * @brief Regression test for DelayedConditionsCleanerSvc.
 */


#undef NDEBUG
#include "AthenaBaseComps/AthService.h"
#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/IConditionsCleanerSvc.h"
#include "AthenaKernel/IRCUSvc.h"
#include "AthenaKernel/CondCont.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "TestTools/initGaudi.h"
#include "TestTools/random.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/IHiveWhiteBoard.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/Service.h"
#include "tbb/global_control.h"
#include "tbb/concurrent_queue.h"
#include "tbb/task_group.h"
#include <list>
#include <algorithm>
#include <iostream>
#include <cassert>


class RCUTest
  : public Athena::IRCUSvc
{
public:
  virtual void add (Athena::IRCUObject*) override {  }
  virtual StatusCode remove (Athena::IRCUObject*) override { return StatusCode::SUCCESS; }
  virtual size_t getNumSlots() const override { return 1; }
  virtual unsigned long addRef()override { std::abort(); }
  virtual unsigned long release() override { std::abort(); }
  virtual StatusCode queryInterface( const InterfaceID&, void** ) override { std::abort(); }

};


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


class CondContTest
  : public CondContBase
{
public:
  static void delfcn (const void*) {}
  CondContTest (Athena::IRCUSvc& rcusvc, const DataObjID& id, int n,
                CondContBase::KeyType keyType)
    : CondContBase (rcusvc, KeyType::SINGLE, 123, id, nullptr, delfcn, 0),
      m_n (n)
  {
    // Do a dummy insert to set the key type.
    EventIDRange r;
    switch (keyType) {
    case CondContBase::KeyType::RUNLBN:
      r = EventIDRange (runlbn (10, 2), runlbn (10, 10));
      break;
    case CondContBase::KeyType::TIMESTAMP:
      r = EventIDRange (timestamp (100), timestamp (200));
      break;
    default:
      std::abort();
    }
    assert (typelessInsert (r, nullptr));
  }
  
  virtual const void* doCast (CLID /*clid*/, const void* /*ptr*/) const override
  { std::abort(); }
  
  virtual size_t entries() const override
  {
    return m_n;
  }

  virtual size_t trim (const std::vector<key_type>& keys) override
  {
    m_keys.push_back (keys);
    return 1;
  }
  

  size_t nkeys() const { return m_keys.size(); }
  std::vector<key_type> keys()
  {
    std::vector<key_type> v = m_keys.front();
    m_keys.pop_front();
    return v;
  }

  virtual
  void list (std::ostream& /*ost*/) const override
  { std::abort(); }

  virtual
  bool valid( const EventIDBase& /*t*/) const override
  { std::abort(); }

  virtual
  bool range (const EventIDBase& /*t*/, EventIDRange& /*r*/) const override
  { std::abort(); }

  virtual
  StatusCode erase (const EventIDBase& /*t*/,
                    const EventContext& /*ctx*/ = Gaudi::Hive::currentContext()) override
  { std::abort(); }
  
  virtual
  std::vector<EventIDRange> ranges() const override
  { std::abort(); }

  virtual
  StatusCode typelessInsert (const EventIDRange& r,
                             void* obj,
                             const EventContext& ctx = Gaudi::Hive::currentContext()) override
  {
    return insertBase (r,
                       CondContSet::payload_unique_ptr (obj, delfcn),
                       ctx);
  }


  virtual
  StatusCode extendLastRange (const EventIDRange& /*newRange*/,
                              const EventContext& /*ctx*/ = Gaudi::Hive::currentContext()) override
  { std::abort(); }


private:
  int m_n;
  std::list<std::vector<key_type> > m_keys;
};


EventContext makeCtx (EventContext::ContextEvt_t evt,
                      EventIDBase::event_number_t evnum = EventIDBase::UNDEFEVT,
                      EventContext::ContextID_t slot = EventContext::INVALID_CONTEXT_ID)
{
  if (evnum == EventIDBase::UNDEFEVT) {
    evnum = evt;
  }
  EventContext ctx (evt, slot);
  EventIDBase eid (0,      // run_number
                   evnum,   // event_number
                   0,      // time_stamp
                   evnum+2000, // ns_offset
                   evnum+1000); // lumi_block
  ctx.setEventID (eid);
  return ctx;
}


void test1 (Athena::IConditionsCleanerSvc& svc)
{
  typedef CondContBase::key_type key_type;

  RCUTest rcu;
  DataObjID id;

  std::cout << "test1\n";
  CondContTest cc1 (rcu, id, 10, CondContBase::KeyType::RUNLBN);
  CondContTest cc2 (rcu, id, 10, CondContBase::KeyType::TIMESTAMP);

  assert( svc.event (makeCtx(0), false).isSuccess() );
  assert( svc.event (makeCtx(1), false).isSuccess() );
  assert( svc.event (makeCtx(1), false).isSuccess() );

  assert( svc.condObjAdded (makeCtx(200), cc1).isSuccess() );
  assert( svc.condObjAdded (makeCtx(100), cc2).isSuccess() );  

  assert( svc.event (makeCtx(4), false).isSuccess() );
  assert( svc.event (makeCtx(3), false).isSuccess() );

  assert (cc1.nkeys() == 0);
  assert (cc2.nkeys() == 0);

  assert( svc.event (makeCtx(201), false).isSuccess() );
  // 1 1 4 3 201
  assert (cc1.nkeys() == 0);
  assert (cc2.nkeys() == 1);
  //for (key_type k : cc2.keysTimestamp()) std::cout << k << " ";
  //std::cout << "\n";
  assert (cc2.keys() == (std::vector<key_type> { 0, 2001, 2003, 2004, 2201 }));

  assert( svc.event (makeCtx(301), false).isSuccess() );
  // 1 4 3 201 301
  assert (cc1.nkeys() == 1);
  assert (cc2.nkeys() == 0);
  assert (cc1.keys() == (std::vector<key_type> { 0, 1001, 1003, 1004, 1201, 1301 }));

  CondContTest cc3 (rcu, id, 10, CondContBase::KeyType::RUNLBN);
  CondContTest cc4 (rcu, id, 10, CondContBase::KeyType::TIMESTAMP);

  assert( svc.condObjAdded (makeCtx(300), cc1).isSuccess() );
  assert( svc.condObjAdded (makeCtx(303), cc2).isSuccess() );
  assert( svc.condObjAdded (makeCtx(305), cc3).isSuccess() );
  assert( svc.condObjAdded (makeCtx(320), cc4).isSuccess() );

  assert( svc.event (makeCtx(401), false).isSuccess() );
  // 4 3 201 301 401
  assert (cc1.nkeys() == 1);
  assert (cc2.nkeys() == 1);
  assert (cc3.nkeys() == 1);
  assert (cc4.nkeys() == 0);
  assert (cc1.keys() == (std::vector<key_type> { 0, 1003, 1004, 1201, 1301, 1401 }));
  assert (cc2.keys() == (std::vector<key_type> { 0, 2003, 2004, 2201, 2301, 2401 }));
  assert (cc3.keys() == (std::vector<key_type> { 0, 1003, 1004, 1201, 1301, 1401 }));

  assert( svc.event (makeCtx(430), false).isSuccess() );
  // 3 201 301 401 430
  assert (cc1.nkeys() == 0);
  assert (cc2.nkeys() == 0);
  assert (cc3.nkeys() == 0);
  assert (cc4.nkeys() == 1);
  assert (cc4.keys() == (std::vector<key_type> { 0, 2003, 2201, 2301, 2401, 2430 }));

  assert (svc.reset().isSuccess());
}


//****************************************************************************
// multi-threaded test.
//


const size_t nthreads = 4;
const size_t nevt = 10000;


class DummyWhiteBoardSvc
  : public extends<AthService, IHiveWhiteBoard>
{
public:
  using extends::extends;

  virtual StatusCode selectStore( size_t ) override { std::abort(); }
  virtual StatusCode clearStore( size_t ) override { std::abort(); }
  virtual StatusCode setNumberOfStores( size_t ) override { std::abort(); }
  virtual bool exists( const DataObjID& ) override { std::abort(); }
  virtual size_t allocateStore( int ) override { std::abort(); }
  virtual StatusCode freeStore( size_t ) override { std::abort(); }
  virtual size_t getPartitionNumber( int ) const override { std::abort(); }
  virtual size_t freeSlots() override { std::abort(); }

  virtual size_t getNumberOfStores() const override
  { return nthreads; }
};
DECLARE_COMPONENT( DummyWhiteBoardSvc )


class DummyIncidentSvc
  : public extends<AthService, IIncidentSvc>
{
public:
  using extends::extends;


  virtual void removeListener( IIncidentListener*, const std::string& = "" ) override { std::abort(); }
  virtual void getListeners( std::vector<IIncidentListener*>&, const std::string& = "" ) const override { std::abort(); }
  virtual void fireIncident( const Incident& ) override { std::abort(); }
  virtual IIncidentSvc::IncidentPack getIncidents( const EventContext* ) override { std::abort(); }
  virtual void fireIncident( std::unique_ptr<Incident> ) override { std::abort(); }


  virtual void addListener( IIncidentListener*, const std::string& = "", long = 0,
                            bool = false, bool = false ) override
  {
  }
};
DECLARE_COMPONENT( DummyIncidentSvc )


typedef tbb::concurrent_bounded_queue<size_t> work_queue_t;


CLASS_DEF(CondCont<int>, 1234, 0)


class CondContWrapper
{
public:
  enum class KeyType
  {
    RUN_LBN,
    TIMESTAMP
  };

  CondContWrapper (Athena::IRCUSvc& rcu,
                   KeyType keyType,
                   size_t m_iovsize,
                   const std::string& name);
  void load (const EventContext& ctx);
  void check (const EventContext& ctx) const;
  void checkFin (bool threaded) const;

  std::unique_ptr<int> makeObj (const EventContext& ctx,
                                EventIDRange& r);
  EventIDBase::number_type getKey (const EventIDBase& eid) const;
  EventIDBase setKey (EventIDBase::number_type k, const EventIDBase& eid_in);


  //private:
  DataObjID m_id;
  KeyType m_keyType;
  size_t m_iovsize;
  CondCont<int> m_cc;

  typedef std::mutex mutex_t;
  typedef std::lock_guard<mutex_t> lock_t;
  std::mutex m_mutex;
};



CondContWrapper::CondContWrapper (Athena::IRCUSvc& rcu,
                                  KeyType keyType,
                                  size_t iovsize,
                                  const std::string& name)
  
  : m_id (1234, name),
    m_keyType (keyType),
    m_iovsize (iovsize),
    m_cc (rcu, m_id)
{
}


EventIDBase::number_type
CondContWrapper::getKey (const EventIDBase& eid) const
{
  if (m_keyType == KeyType::RUN_LBN) {
    return eid.lumi_block();
  }
  else {
    return eid.time_stamp_ns_offset();
  }
}


EventIDBase CondContWrapper::setKey (EventIDBase::number_type k,
                                     const EventIDBase& eid_in)
{
  EventIDBase eid;
  if (m_keyType == KeyType::RUN_LBN) {
    eid.set_run_number (eid_in.run_number());
    eid.set_lumi_block (k);
  }
  else {
    eid.set_time_stamp (eid_in.time_stamp());
    eid.set_time_stamp_ns_offset (k);
  }
  return eid;
}


std::unique_ptr<int>
CondContWrapper::makeObj (const EventContext& ctx,
                          EventIDRange& r)
{
  EventIDBase::number_type key = getKey (ctx.eventID());
  EventIDBase::number_type key_start = (key / m_iovsize) * m_iovsize;
  EventIDBase::number_type key_stop = key_start + m_iovsize;

  EventIDBase start = setKey (key_start, ctx.eventID());
  EventIDBase stop  = setKey (key_stop,  ctx.eventID());
  r = EventIDRange (start, stop);
  return std::make_unique<int> (key_start);
}


void CondContWrapper::load (const EventContext& ctx)
{
  const int* p;
  if (m_cc.find (ctx.eventID(), p)) return;

  lock_t lock (m_mutex);
  if (m_cc.find (ctx.eventID(), p)) return;

  EventIDRange r;
  std::unique_ptr<int> o = makeObj (ctx, r);
  assert( m_cc.insert (r, std::move (o), ctx).isSuccess() );
}


void CondContWrapper::check (const EventContext& ctx) const
{
  const int* p;
  const EventIDRange* r;

  assert (m_cc.find (ctx.eventID(), p, &r));
  assert (static_cast<int>(getKey (r->start())) == *p);
}


void CondContWrapper::checkFin (bool threaded) const
{
  assert ( std::abs (static_cast<int>(m_cc.nInserts() * m_iovsize) -
                     static_cast<int> (nevt))
           < static_cast<int> (m_iovsize) );

  // If the test is threaded, then we cannot make any guarantees on the
  // the maximum queue size.  A thread using the oldest item in the
  // conditions container could stall while other threads proceed.
  // The queue will grow until the stalled thread makes progress.
  // This was observed running the threaded test when there is other
  // load on the system.
  if (!threaded) {
    if (m_iovsize >= 100) {
      assert (m_cc.maxSize() < 10);
    }
    else {
      assert (m_cc.maxSize() < 40);
    }
  }
}


struct CCS
{
  CCS (Athena::IRCUSvc& rcu);
  CondContWrapper m_cc_lbn1;
  CondContWrapper m_cc_lbn2;
  CondContWrapper m_cc_ts1;
  CondContWrapper m_cc_ts2;

  void load (const EventContext& ctx);
  void check (const EventContext& ctx) const;
  void checkFin (bool threaded) const;
};


CCS::CCS (Athena::IRCUSvc& rcu)
  : m_cc_lbn1 (rcu, CondContWrapper::KeyType::RUN_LBN,   100, "lbn1"),
    m_cc_lbn2 (rcu, CondContWrapper::KeyType::RUN_LBN,    10, "lbn2"),
    m_cc_ts1  (rcu, CondContWrapper::KeyType::TIMESTAMP, 100, "ts1"),
    m_cc_ts2  (rcu, CondContWrapper::KeyType::TIMESTAMP,  10, "ts2")
{
}


void CCS::load (const EventContext& ctx)
{
  m_cc_lbn1.load (ctx);
  m_cc_lbn2.load (ctx);
  m_cc_ts1.load (ctx);
  m_cc_ts2.load (ctx);
}


void CCS::check (const EventContext& ctx) const
{
  m_cc_lbn1.check (ctx);
  m_cc_lbn2.check (ctx);
  m_cc_ts1.check (ctx);
  m_cc_ts2.check (ctx);
}


void CCS::checkFin (bool threaded) const
{
  m_cc_lbn1.checkFin (threaded);
  m_cc_lbn2.checkFin (threaded);
  m_cc_ts1.checkFin (threaded);
  m_cc_ts2.checkFin (threaded);
}


void eventTask (const EventContext& ctx,
                CCS& ccs,
                Athena::IRCUSvc& rcu,
                work_queue_t& wqueue)
{
  ccs.load (ctx);
  ccs.check (ctx);

  Incident inc ("test", IncidentType::EndEvent, ctx);
  dynamic_cast<IIncidentListener&>(rcu).handle (inc);
  wqueue.push (ctx.slot());
}


void shuffle (uint32_t& seed, std::vector<size_t>& v)
{
  size_t sz = v.size();
  size_t maxdist = std::min (sz-1, static_cast<size_t>(10));
  for (size_t i = 0; i < sz/2; i++) {
    size_t dist = Athena_test::randi_seed (seed, maxdist);
    size_t pos = Athena_test::randi_seed (seed, sz-1-dist);
    std::swap (v[pos], v[pos+dist]);
  }
}


class Tester
{
public:
  Tester (Athena::IConditionsCleanerSvc& cleaner,
          Athena::IRCUSvc& rcu,
          bool allowAsync);
  void nonthreaded_event_loop (CCS& ccs, const std::vector<size_t>& evnums);
  void event_loop (CCS& ccs, const std::vector<size_t>& evnums);


private:
  Athena::IConditionsCleanerSvc& m_cleaner;
  Athena::IRCUSvc& m_rcu;
  bool m_allowAsync;
  work_queue_t m_ready;
};



Tester::Tester (Athena::IConditionsCleanerSvc& cleaner,
                Athena::IRCUSvc& rcu,
                bool allowAsync)
  : m_cleaner (cleaner),
    m_rcu (rcu),
    m_allowAsync (allowAsync)
{
}



void Tester::nonthreaded_event_loop (CCS& ccs,
                                     const std::vector<size_t>& evnums)
{
  size_t nev = evnums.size();
  for (size_t ievt=0; ievt < nev; ievt++) {
    EventContext ctx = makeCtx (ievt, evnums[ievt], 0);

    assert (m_cleaner.event (ctx, false).isSuccess() );

    ccs.load (ctx);
    ccs.check (ctx);
  }

  ccs.checkFin (false);
}


void Tester::event_loop (CCS& ccs, const std::vector<size_t>& evnums)
{
  for (size_t i=0; i<nthreads; i++) {
    m_ready.push (i);
  }

  tbb::task_group tg;
  size_t nev = evnums.size();
  for (size_t ievt=0; ievt < nev; ievt++) {
    size_t slot;
    m_ready.pop (slot);

    EventContext ctx = makeCtx (ievt, evnums[ievt], slot);

    assert (m_cleaner.event (ctx, m_allowAsync).isSuccess() );

    tg.run ([&, ctx] { eventTask (ctx, ccs, m_rcu, m_ready); });
  }

  // Wait for all tasks to complete.
  for (size_t i=0; i<nthreads; i++) {
    size_t slot;
    m_ready.pop (slot);
  }
  tg.wait();

  ccs.checkFin (true);
}


void threaded_test (Athena::IConditionsCleanerSvc& cleaner,
                    Athena::IRCUSvc& rcu)
{
  assert (cleaner.reset().isSuccess());
  std::vector<size_t> evnums (nevt);
  for (size_t i = 0; i < nevt; i++) {
    evnums[i] = i;
  }
  uint32_t seed = 34563;
  shuffle (seed, evnums);

  std::cout << "threaded_test\n";
  tbb::global_control (tbb::global_control::max_allowed_parallelism, nthreads + 1);

  {
    Tester test (cleaner, rcu, false);
    CCS ccs_nt (rcu);
    test.nonthreaded_event_loop (ccs_nt, evnums);
    assert( cleaner.printStats().isSuccess() );
    CCS ccs_mt (rcu);
    test.event_loop (ccs_mt, evnums);
  }
}


int main()
{
  errorcheck::ReportMessage::hideErrorLocus();
  std::cout << "DelayedConditionsCleanerSvc_test\n";
  ISvcLocator* svcloc = 0;
  if (!Athena_test::initGaudi("AthenaServices/DelayedConditionsCleanerSvc_test.txt", svcloc)) {
    std::cerr << "This test can not be run" << std::endl;
    return 1;
  }  
  assert(svcloc);

  ServiceHandle<Athena::IConditionsCleanerSvc> svc
    ("Athena::DelayedConditionsCleanerSvc/TestConditionsCleanerSvc", "test");
  if (svc.retrieve().isFailure()) {
    std::cerr << "Can't retrieve service." << std::endl;
    return 1;
  }

  test1 (*svc);

  ServiceHandle<Athena::IConditionsCleanerSvc> facadeSvc
    ("Athena::ConditionsCleanerSvc", "test");
  if (facadeSvc.retrieve().isFailure()) {
    std::cerr << "Can't retrieve service." << std::endl;
    return 1;
  }

  ServiceHandle<Athena::IRCUSvc> rcuSvc
    ("Athena::RCUSvc", "test");
  if (rcuSvc.retrieve().isFailure()) {
    std::cerr << "Can't retrieve RCU service." << std::endl;
    return 1;
  }

  threaded_test (*facadeSvc, *rcuSvc);
  return 0;
}

