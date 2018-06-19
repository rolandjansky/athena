/*
 * Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file AthenaServices/test/ConditionsCleanerSvc_test.cxx
 * @author scott snyder
 * @date May 2018
 * @brief Unit test for ConditionsCleanerSvc
 */


#undef NDEBUG
#include "TestTools/initGaudi.h"
#include "AthenaBaseComps/AthService.h"
#include "AthenaKernel/IConditionsCleanerSvc.h"
#include "AthenaKernel/IRCUSvc.h"
#include "AthenaKernel/CondCont.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/ISvcLocator.h"
#include <iostream>
#include <cassert>
#include <cstdlib>


class RCUTest
  : public Athena::IRCUSvc
{
public:
  virtual void add (Athena::IRCUObject*) override { std::abort(); }
  virtual StatusCode remove (Athena::IRCUObject*) override { std::abort(); }
  virtual size_t getNumSlots() const override { std::abort(); }
  virtual unsigned long addRef()override { std::abort(); }
  virtual unsigned long release() override { std::abort(); }
  virtual StatusCode queryInterface( const InterfaceID&, void** ) override { std::abort(); }

};


class CondContTest
  : public CondContBase
{
public:
  CondContTest()
    : CondContBase (m_rcu, 123, m_id, nullptr)
  {}
  virtual const DataObjID& id() const override { std::abort(); }
  virtual SG::DataProxy* proxy() override { std::abort(); }
  virtual void setProxy(SG::DataProxy*) override { std::abort(); }
  virtual void list (std::ostream&) const override { std::abort(); }
  virtual size_t entries() const override { std::abort(); }
  virtual size_t entriesRunLBN() const override { std::abort(); }
  virtual size_t entriesTimestamp() const override { std::abort(); }
  virtual std::vector<EventIDRange> ranges() const override { std::abort(); }
  virtual StatusCode typelessInsert (const EventIDRange&,
                                     void*,
                                     const EventContext&) override { std::abort(); }
  virtual bool valid( const EventIDBase&) const override { std::abort(); }
  virtual bool range (const EventIDBase&, EventIDRange&) const override { std::abort(); }
  virtual void erase (const EventIDBase&,
                      const EventContext&) override { std::abort(); }
  virtual size_t trimRunLBN (const std::vector<key_type>&) override { std::abort(); }
  virtual size_t trimTimestamp (const std::vector<key_type>&) override { std::abort(); }
  virtual void quiescent (const EventContext&) override { std::abort(); }
  
  virtual const void* findByCLID (CLID,
                                  const EventIDBase&,
                                  EventIDRange const**) const override { std::abort(); }
  virtual size_t nInserts() const override { std::abort(); }
  virtual size_t maxSize() const override { std::abort(); }

private:
  RCUTest m_rcu;
  DataObjID m_id;
};



class ConditionsCleanerTest
  : public extends<AthService, Athena::IConditionsCleanerSvc>
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

  virtual StatusCode printStats() const override;
};


StatusCode ConditionsCleanerTest::event (const EventContext& ctx,
                                         bool allowAsync)
{
  std::cout << "ConditionsCleanerTest::event "
            << ctx.evt() << " " << allowAsync << "\n";
  return StatusCode::SUCCESS;
}


StatusCode ConditionsCleanerTest::condObjAdded (const EventContext& ctx,
                                                CondContBase& cc)
{
  std::cout << "ConditionsCleanerTest::condObjAdded "
            << ctx.evt() << " " << cc.clid() << "\n";
  return StatusCode::SUCCESS;
}


StatusCode ConditionsCleanerTest::printStats() const
{
  std::cout << "ConditionsCleanerTest::printStats\n";
  return StatusCode::SUCCESS;
}


DECLARE_COMPONENT( ConditionsCleanerTest )


void testit (IService* mgr)
{
  Athena::IConditionsCleanerSvc* ccs =
    dynamic_cast<Athena::IConditionsCleanerSvc*> (mgr);
  if (!ccs) std::abort();

  assert( ccs->event (EventContext(0,0), false).isSuccess() );

  CondContTest cc;

  assert( ccs->condObjAdded (EventContext(0,0), cc).isSuccess() );
  assert( ccs->printStats().isSuccess() );
}


void test1 (ISvcLocator* svcloc)
{
  std::cout << "test1\n";

  IService *mgr = nullptr;
  if (svcloc->service ("ConditionsCleanerSvc/ConditionsCleanerSvc1", mgr).isFailure()) std::abort();
  testit (mgr);
}


void test2 (ISvcLocator* svcloc)
{
  std::cout << "test2\n";

  IService *mgr = nullptr;
  if (svcloc->service ("ConditionsCleanerSvc/ConditionsCleanerSvc2", mgr).isFailure()) std::abort();
  testit (mgr);
}


int main()
{
  std::cout << "ConditionsCleanerSvc_test\n";
  ISvcLocator* svcloc = nullptr;
  if (!Athena_test::initGaudi("ConditionsCleanerSvc_test.txt", svcloc)) {
    std::cerr << "This test can not be run" << std::endl;
    return 0;
  }  
  assert(svcloc);

  test1 (svcloc);
  test2 (svcloc);
  
  return 0;
}
