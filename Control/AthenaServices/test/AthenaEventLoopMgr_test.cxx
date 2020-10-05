/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AthenaServices/test/AthenaEventLoopMgr_test.cxx
 * @author scott snyder
 * @date Jun 2016
 * @brief Unit test for AthenaEventLoopMgr
 *
 * Still sadly incomplete.
 */


#undef NDEBUG
#include "TestTools/initGaudi.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthService.h"
#include "AthenaKernel/errorcheck.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "AthenaKernel/ClassID_traits.h"
#include "AthenaKernel/ICollectionSize.h"
#include "AthenaKernel/IEventSeek.h"
#include "AthenaKernel/IEvtSelectorSeek.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IEventProcessor.h"
#include "GaudiKernel/Service.h"
// //#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/ConversionSvc.h"
#include <iostream>
#include <cassert>


//********************************************************************


class TestConversionSvc
  : public ConversionSvc
{
public:
  TestConversionSvc(const std::string &name, ISvcLocator* svc)
    : ConversionSvc(name, svc, storageType()) {}

  static long int storageType() { return 96; }
  virtual long int repSvcType() const override { return storageType(); }
};


//********************************************************************


class TestAddress
  : public IOpaqueAddress
{
public:
  TestAddress(DataObject* obj);
  virtual unsigned long        addRef     () override { return 10; }
  virtual unsigned long        release    () override { return 10; }
  virtual const CLID&          clID       () const override;
  virtual long                 svcType    () const override;
  virtual IRegistry*           registry   () const override
  { std::cout << "registry\n"; std::abort(); }
  virtual void                 setRegistry(IRegistry* /*r*/) override
  { std::cout << "setRegistry\n"; std::abort(); }
  virtual const std::string*   par        () const override;
  virtual const unsigned long* ipar       () const override
  { std::cout << "ipar\n"; std::abort(); }

  DataObject* m_object;
  CLID m_clid;
  std::string m_par[2];
};


TestAddress::TestAddress(DataObject* obj)
  : m_object(obj),
    m_clid (ClassID_traits<EventInfo>::ID())
{
  m_par[1] = "EventInfo";
}


const CLID& TestAddress::clID () const
{
  return m_clid;
}


const std::string*   TestAddress::par() const
{
  return m_par;
}


long TestAddress::svcType () const
{
  return TestConversionSvc::storageType();
}


//********************************************************************


class TestConverter
  : public Converter
{
public:
  TestConverter(ISvcLocator* svcloc) : 
    Converter(storageType(), classID(), svcloc) {}

  virtual StatusCode createObj(IOpaqueAddress* addr, DataObject *& pO) override;
  virtual long int repSvcType() const override;
  static const CLID& classID() { return ClassID_traits<EventInfo>::ID(); }
  static long int storageType()
  { return TestConversionSvc::storageType(); }
};


StatusCode TestConverter::createObj(IOpaqueAddress* addr, DataObject *& pO)
{
  TestAddress* taddr = dynamic_cast<TestAddress*> (addr);
  if (!taddr) std::abort();
  pO = taddr->m_object;
  return StatusCode::SUCCESS;
}

long int TestConverter::repSvcType() const
{
  return storageType();
}



//********************************************************************


class TestContext
  : public IEvtSelector::Context
{
public:
  TestContext();
  virtual ~TestContext() override;
  virtual void* identifier() const override
  { std::cout << "identifier\n"; std::abort(); }

  EventInfo m_ei;
  SG::DataBucket<EventInfo> m_object;
  std::unique_ptr<TestAddress> m_addr;
  unsigned int m_event_num;
};


TestContext::TestContext()
  : m_object(&m_ei),
    m_addr (std::make_unique<TestAddress> (&m_object)),
    m_event_num (0)
{
  m_object.addRef();
}


TestContext::~TestContext()
{
  m_object.relinquish();
}


class TestEvtSelector
  : public extends<AthService, IEvtSelector>
{
public:
  TestEvtSelector (const std::string& nam, ISvcLocator* svcLoc);
  virtual ~TestEvtSelector() override;
  virtual StatusCode createContext(Context*& c) const override;
  virtual StatusCode next(Context& c) const override;
  virtual StatusCode next(Context& /*c*/,int /*jump*/) const override
  { std::cout << "next2\n"; std::abort(); }
  virtual StatusCode previous(Context& /*c*/) const override
  { std::cout << "previous1\n"; std::abort(); }
  virtual StatusCode previous(Context& /*c*/,int /*jump*/) const override
  { std::cout << "previous2\n"; std::abort(); }
  virtual StatusCode last(Context& /*refContext*/) const override
  { std::cout << "last\n"; std::abort(); }
  virtual StatusCode rewind(Context& /*c*/) const override
  { std::cout << "rewind\n"; std::abort(); }
  virtual StatusCode createAddress(const Context& /*c*/,IOpaqueAddress*& /*iop*/) const override;
  virtual StatusCode releaseContext(Context*& c) const override;
  virtual StatusCode resetCriteria(const std::string& /*cr*/,Context& /*c*/)const override
  { std::cout << "resetCriteria\n"; std::abort(); }
};


TestEvtSelector::TestEvtSelector (const std::string& nam, ISvcLocator* svcLoc)
  : base_class (nam, svcLoc)
{
}


TestEvtSelector::~TestEvtSelector()
{
}


StatusCode TestEvtSelector::createContext(Context*& c) const
{
  c = new TestContext;
  return StatusCode::SUCCESS;
}


StatusCode TestEvtSelector::releaseContext(Context*& c) const
{
  delete c;
  return StatusCode::SUCCESS;
}


StatusCode TestEvtSelector::next(Context& c) const
{
  TestContext& ctx = dynamic_cast<TestContext&>(c);
  EventID::number_type run = ctx.m_ei.event_ID()->run_number();
  *ctx.m_ei.event_ID() = EventID (run, ++ctx.m_event_num);
  
  std::cout << "TestEvtSelector::next(Context&)\n";
  return StatusCode::SUCCESS;
}


StatusCode
TestEvtSelector::createAddress(const Context& c,IOpaqueAddress*& iop) const
{
  const TestContext& ctx = dynamic_cast<const TestContext&>(c);
  iop = ctx.m_addr.get();
  return StatusCode::SUCCESS;
}


//********************************************************************


class TestEvtSelector2
  : public extends<TestEvtSelector, IEvtSelectorSeek>
{
public:
  TestEvtSelector2 (const std::string& nam, ISvcLocator* svcLoc)
    : base_class (nam, svcLoc) {}
  virtual int size (Context&) const override { return 123; }
  virtual StatusCode seek (Context& c, int evtnum) const override
  {
    TestContext& ctx = dynamic_cast<TestContext&>(c);
    ctx.m_event_num = evtnum; return StatusCode::SUCCESS;
  }
  virtual int curEvent (const Context& c) const override
  {
    const TestContext& ctx = dynamic_cast<const TestContext&>(c);
    return ctx.m_event_num;
  }
};


//********************************************************************


class TestAlgorithm
  : public AthAlgorithm
{
public:
  TestAlgorithm(const std::string& name,
                ISvcLocator* pSvcLocator,
                const std::string& version = PACKAGE_VERSION)
    : AthAlgorithm(name, pSvcLocator, version) {}

  virtual StatusCode initialize() override
  { std::cout << "TestAlgorithm::initialize\n"; return StatusCode::SUCCESS; }
  virtual StatusCode finalize() override
  { std::cout << "TestAlgorithm::finalize\n"; return StatusCode::SUCCESS; }
  virtual StatusCode execute() override;
};


StatusCode TestAlgorithm::execute()
{
  const EventContext& ctx = getContext();
  std::cout << "TestAlgorithm::execute "
            << ctx.eventID().event_number() << " "
            << ctx.evt() << "\n";
  return StatusCode::SUCCESS;
}


//********************************************************************


DECLARE_COMPONENT( TestEvtSelector )
DECLARE_COMPONENT( TestEvtSelector2 )
DECLARE_COMPONENT( TestConversionSvc )
DECLARE_COMPONENT( TestAlgorithm )
DECLARE_CONVERTER( TestConverter )
class swallowsemicolon;

void testit (IService* mgr)
{
  IEventProcessor* iep = dynamic_cast<IEventProcessor*> (mgr);
  if (!iep) std::abort();
  assert (iep->nextEvent(1));
  assert (iep->nextEvent(3));

  ICollectionSize* isize = dynamic_cast<ICollectionSize*> (mgr);
  if (!isize) std::abort();
  int sz = isize->size();
  std::cout << "size: " <<  sz << "\n";

  IEventSeek* iseek = dynamic_cast<IEventSeek*> (mgr);
  if (!iseek) std::abort();
  StatusCode sc = iseek->seek (1);
  std::cout << "seek: " << sc.isSuccess() << "\n";

  assert (iep->nextEvent(5));
}


void test1 (ISvcLocator* svcloc)
{
  std::cout << "test1\n";

  IService *mgr = nullptr;
  if (svcloc->service ("AthenaEventLoopMgr/AthenaEventLoopMgr1", mgr).isFailure()) std::abort();

  testit (mgr);
}


void test2 (ISvcLocator* svcloc)
{
  std::cout << "test2\n";

  IService *mgr = nullptr;
    if (svcloc->service ("AthenaEventLoopMgr/AthenaEventLoopMgr2", mgr).isFailure()) std::abort();

  testit (mgr);
}


int main()
{
  errorcheck::ReportMessage::hideErrorLocus();
  ISvcLocator* svcloc = nullptr;
  if (!Athena_test::initGaudi("AthenaServices/AthenaEventLoopMgr_test.txt", svcloc)) {
    std::cerr << "This test can not be run" << std::endl;
    return 0;
  }  
  assert(svcloc);

  test1 (svcloc);
  test2 (svcloc);
  
  return 0;
}
