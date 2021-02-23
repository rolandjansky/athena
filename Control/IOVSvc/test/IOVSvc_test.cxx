/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file IOVSvc/test/IOVSvc_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2018
 * @brief Some tests for IOVSvc.  Not complete!
 */


#undef NDEBUG
#include "TestTools/initGaudi.h"
#include "StoreGate/StoreGateSvc.h"
#include "SGTools/DataProxy.h"
#include "AthenaKernel/DummyRCUSvc.h"
#include "AthenaKernel/IIOVSvc.h"
#include "AthenaKernel/IIOVDbSvc.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/CondCont.h"
#include "AthenaKernel/IAddressProvider.h"
#include "AthenaKernel/IOVTime.h"
#include "AthenaKernel/IOVRange.h"
#include "AthenaKernel/StorableConversions.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include <sstream>
#include <cassert>
#include <iostream>


class Payload
{
public:
  Payload (int x) : m_x (x) {}
  int m_x;
};

CLASS_DEF(Payload, 932847540, 0)
CLASS_DEF(CondCont<Payload>, 932847546, 0)


class TestAddress
  : public IOpaqueAddress
{
public:
  TestAddress (int);
  DataObject* dobj();

  virtual unsigned long addRef() override
  { std::abort(); }
  virtual unsigned long release() override
  { std::abort(); }
  virtual const CLID& clID() const override
  { std::abort(); }
  virtual long svcType() const override
  { std::abort(); }
  virtual IRegistry* registry() const override
  { std::abort(); }
  virtual void setRegistry( IRegistry* /*r*/ ) override
  { std::abort(); }
  virtual const std::string* par() const override
  { std::abort(); }
  virtual const unsigned long* ipar() const override
  { std::abort(); }


private:
  DataObject* m_dobj;
};


TestAddress::TestAddress (int x)
{
  Payload* payload = new Payload (x);
  m_dobj = SG::asStorable (payload);
}


DataObject* TestAddress::dobj()
{
  return m_dobj;
}


class TestLoader
  : public IConverter
{
public:
  virtual StatusCode createObj( IOpaqueAddress* pAddress, DataObject*& refpObject ) override;

  virtual unsigned long addRef() override { return 1;  }
  virtual unsigned long release() override { return 1; }
  virtual StatusCode queryInterface(const InterfaceID &/*ti*/, void** /*pp*/) override { std::abort(); }

  virtual StatusCode initialize() override
  { std::abort(); }
  virtual StatusCode finalize() override 
  { std::abort(); }
  virtual const CLID& objType() const override
  { std::abort(); }
  virtual long repSvcType() const override
  { std::abort(); }
  virtual StatusCode setDataProvider( IDataProviderSvc* /*pService*/ ) override
  { std::abort(); }
  virtual SmartIF<IDataProviderSvc>& dataProvider() const override
  { std::abort(); }
  virtual StatusCode setConversionSvc( IConversionSvc* /*pService*/ ) override
  { std::abort(); }
  virtual SmartIF<IConversionSvc>& conversionSvc() const override
  { std::abort(); }
  virtual StatusCode setAddressCreator( IAddressCreator* /*creator*/ ) override
  { std::abort(); }
  virtual SmartIF<IAddressCreator>& addressCreator() const override
  { std::abort(); }
  virtual StatusCode fillObjRefs( IOpaqueAddress* /*pAddress*/, DataObject* /*pObject*/ ) override
  { std::abort(); }
  virtual StatusCode updateObj( IOpaqueAddress* /*pAddress*/, DataObject* /*refpObject*/ ) override
  { std::abort(); }
  virtual StatusCode updateObjRefs( IOpaqueAddress* /*pAddress*/, DataObject* /*pObject*/ ) override
  { std::abort(); }
  virtual StatusCode createRep( DataObject* /*pObject*/, IOpaqueAddress*& /*refpAddress*/ ) override
  { std::abort(); }
  virtual StatusCode fillRepRefs( IOpaqueAddress* /*pAddress*/, DataObject* /*pObject*/ ) override
  { std::abort(); }
  virtual StatusCode updateRep( IOpaqueAddress* /*pAddress*/, DataObject* /*pObject*/ ) override
  { std::abort(); }
  virtual StatusCode updateRepRefs( IOpaqueAddress* /*pAddress*/, DataObject* /*pObject*/ ) override
  { std::abort(); }
};


StatusCode TestLoader::createObj( IOpaqueAddress* pAddress, DataObject*& refpObject )
{
  if (TestAddress* addr = dynamic_cast<TestAddress*> (pAddress)) {
    refpObject = addr->dobj();
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}


class TestDBSvc
  : public IAddressProvider, public IIOVDbSvc
{
public:
  TestDBSvc();

  virtual unsigned long addRef() override { std::abort(); }
  virtual unsigned long release() override { std::abort(); }
  virtual StatusCode queryInterface( const InterfaceID& /*ti*/, void** /*pp*/ ) override
  { std::abort(); }

  virtual StatusCode updateAddress(StoreID::type /*storeID*/,
				   SG::TransientAddress* /*pTAd*/,
                                   const EventContext& /*ctx*/) override
  { std::abort(); }

  virtual StatusCode getRange      (const CLID& /*clid*/, 
                                    const std::string& /*dbKey*/, 
                                    const IOVTime& /*time*/,
                                    IOVRange& /*range*/,
                                    std::string& /*tag*/,
                                    std::unique_ptr<IOpaqueAddress>& /*ioa*/) override;

  virtual StatusCode setRange      (const CLID& /*clid*/,
                                    const std::string& /*dbKey*/,
                                    const IOVRange& /*range*/,
                                    const std::string& /*tag*/ ) override
  { std::abort(); }

  virtual StatusCode signalBeginRun(const IOVTime& /*beginRunTime*/,
                                    const EventContext& /*ctx*/) override
  { std::abort(); }

  virtual void       signalEndProxyPreload() override
  { std::abort(); }

  virtual void       postConditionsLoad() override
  { std::abort(); }

  virtual StatusCode processTagInfo() override
  { std::abort(); }

  virtual std::vector<std::string> getKeyList() override
  { std::abort(); }

  virtual bool getKeyInfo(const std::string& /*key*/, IIOVDbSvc::KeyInfo& /*info*/) override
  { std::abort(); }

  virtual bool dropObject(const std::string& /*key*/,
                          const bool /*resetCache*/=false) override
  { std::abort(); }
};


TestDBSvc::TestDBSvc()
{
}


StatusCode TestDBSvc::getRange (const CLID& /*clid*/, 
                                const std::string& /*dbKey*/, 
                                const IOVTime& time,
                                IOVRange& range,
                                std::string& /*tag*/,
                                std::unique_ptr<IOpaqueAddress>& ioa)
{
  uint32_t lbn = time.event();

  if (lbn >= 10 && lbn < 20) {
    range = IOVRange (IOVTime (10, 10), IOVTime (10, 20));
    ioa = std::make_unique<TestAddress> (1);
    return StatusCode::SUCCESS;
  }

  if (lbn >= 30 && lbn < 40) {
    range = IOVRange (IOVTime (10, 30), IOVTime (10, lbn+1));
    ioa = std::make_unique<TestAddress> (2);
    return StatusCode::SUCCESS;
  }

  if (lbn >= 50) {
    range = IOVRange (IOVTime (10, 50),
                      IOVTime (10, IOVTime::MAXEVENT));
    ioa = std::make_unique<TestAddress> (3);
    return StatusCode::SUCCESS;
  }

  return StatusCode::FAILURE;
}


std::string dump_cc (const CondCont<Payload>& cc)
{
  std::ostringstream ss;
  for (const EventIDRange& r : cc.ranges()) {
    const Payload* p = nullptr;
    assert (cc.find (r.start(), p));
    ss << r << " [" << p->m_x << "]\n";
  }
  return ss.str();
}


// Testing extending containers.
void test1 (Athena_test::DummyRCUSvc& rcusvc, StoreGateSvc& detStore, IIOVSvc& iovsvc)
{
  std::cout << "test1\n";

  TestLoader testLoader;
  DataObjID id (ClassID_traits<Payload>::ID(), "key");
  SG::DataProxy* proxy = new SG::DataProxy
    (SG::TransientAddress  (id.clid(), id.key()), &testLoader);
  assert( detStore.addToStore (id.clid(), proxy).isSuccess() );

  TestDBSvc testDBSvc;
  proxy->setProvider (&testDBSvc, StoreID::EVENT_STORE);

  assert (iovsvc.regProxy(proxy, id.key()).isSuccess());
  
  CondCont<Payload> ccb (rcusvc, id, proxy);

  EventIDBase now;
  now.set_run_number (10);
  now.set_lumi_block (15);
  assert( iovsvc.createCondObj (&ccb, id, now).isSuccess() );
  assert (dump_cc(ccb) == "{[10,l:10] - [10,l:20]} [1]\n");

  now.set_lumi_block (17); 
  assert( iovsvc.createCondObj (&ccb, id, now).isSuccess() );
  assert (dump_cc(ccb) == "{[10,l:10] - [10,l:20]} [1]\n");

  now.set_lumi_block (25);
  assert( iovsvc.createCondObj (&ccb, id, now).isFailure() );
  assert (dump_cc(ccb) == "{[10,l:10] - [10,l:20]} [1]\n");

  now.set_lumi_block (32);
  assert( iovsvc.createCondObj (&ccb, id, now).isSuccess() );
  assert (dump_cc(ccb) == "{[10,l:10] - [10,l:20]} [1]\n"
          "{[10,l:30] - [10,l:33]} [2]\n");

  now.set_lumi_block (36);
  assert( iovsvc.createCondObj (&ccb, id, now).isSuccess() );
  assert (dump_cc(ccb) == "{[10,l:10] - [10,l:20]} [1]\n"
          "{[10,l:30] - [10,l:37]} [2]\n");

  now.set_lumi_block (34);
  assert( iovsvc.createCondObj (&ccb, id, now).isSuccess() );
  assert (dump_cc(ccb) == "{[10,l:10] - [10,l:20]} [1]\n"
          "{[10,l:30] - [10,l:37]} [2]\n");

  now.set_lumi_block (55);
  assert( iovsvc.createCondObj (&ccb, id, now).isSuccess() );
  assert (dump_cc(ccb) == "{[10,l:10] - [10,l:20]} [1]\n"
          "{[10,l:30] - [10,l:37]} [2]\n"
          "{[10,l:50] - [INVALID]} [3]\n");

  now.set_lumi_block (1000);
  assert( iovsvc.createCondObj (&ccb, id, now).isSuccess() );
  assert (dump_cc(ccb) == "{[10,l:10] - [10,l:20]} [1]\n"
          "{[10,l:30] - [10,l:37]} [2]\n"
          "{[10,l:50] - [INVALID]} [3]\n");

  //std::cout << dump_cc (ccb);
}


int main()
{
  std::cout << "IOVSvc_test\n";
  ISvcLocator* svcLoc;
  if (!Athena_test::initGaudi("IOVSvc_test.txt", svcLoc))
    return 1;

  ServiceHandle<IIOVSvc> iovsvc ("IOVSvc", "test");
  if (iovsvc.retrieve().isFailure()) return 1;

  ServiceHandle<StoreGateSvc> detStore ("DetectorStore", "test");
  if (detStore.retrieve().isFailure()) return 1;

  Athena_test::DummyRCUSvc rcusvc;
  test1 (rcusvc, *detStore.get(), *iovsvc.get());

  return 0;
}
