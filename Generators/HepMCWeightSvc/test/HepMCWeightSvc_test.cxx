/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AthenaKernel/test/ILockableTool_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2022
 * @brief Unit test for HepMCWeightSvc.
 */


#undef NDEBUG
#include "GenInterfaces/IHepMCWeightSvc.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEventInfo/EventAuxInfo.h"
#include "IOVDbDataModel/IOVMetaDataContainer.h"
#include "IOVDbMetaDataTools/IIOVDbMetaDataTool.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "SGTools/TestStore.h"
#include "TestTools/initGaudi.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <sstream>
#include <vector>
#include <iostream>
#include <cassert>


void test1 (SGTest::TestStore& store)
{
  std::cout << "test1\n";
  ServiceHandle<IHepMCWeightSvc> svc ("test1", "test");
  if (svc.retrieve().isFailure()) std::abort();

  ToolHandle<IIOVDbMetaDataTool> mdtool ("IOVDbMetaDataTool");
  if (mdtool.retrieve().isFailure()) std::abort();

  // create a dummy EventContext
  EventContext ctx;
  ctx.setEventID (EventIDBase (12345, 1));
  ctx.setExtension( Atlas::ExtendedEventContext( &store ) );
  Gaudi::Hive::setCurrentContext( ctx );

  IHepMCWeightSvc::WeightMap wmap;
  wmap["one"] = 1;
  wmap["two"] = 2;
  wmap["three"] = 3;
  if (svc->setWeightNames (wmap, ctx).isFailure()) std::abort();

  assert (svc->weightNames (ctx) == wmap);
  assert (svc->weightNameVec (ctx) == (std::vector<std::string> {"three", "two", "one"}));

  {
    const IOVMetaDataContainer* cont = mdtool->findMetaDataContainer ("/Generation/Parameters");
    std::ostringstream ss;
    cont->dump (ss);
    std::cout << ss.str() << "\n";
  }

  IHepMCWeightSvc::WeightMap wmap2;
  wmap2["foo"] = 10;
  if (svc->setWeightNames (wmap2, ctx).isFailure()) std::abort();
  assert (svc->weightNames (ctx) == wmap);

  {
    const IOVMetaDataContainer* cont = mdtool->findMetaDataContainer ("/Generation/Parameters");
    std::ostringstream ss;
    cont->dump (ss);
    std::cout << ss.str() << "\n";
  }
}


void test2 (SGTest::TestStore& store)
{
  std::cout << "test2\n";

  ServiceHandle<IHepMCWeightSvc> svc ("test2", "test");
  if (svc.retrieve().isFailure()) std::abort();

  ServiceHandle<StoreGateSvc> mdstore ("MetaDataStore", "test");
  if (mdstore.retrieve().isFailure()) std::abort();
  if (mdstore->clearStore().isFailure()) std::abort();

  ToolHandle<IIOVDbMetaDataTool> mdtool ("IOVDbMetaDataTool");
  if (mdtool.retrieve().isFailure()) std::abort();

  // create a dummy EventContext
  EventContext ctx;
  ctx.setEventID (EventIDBase (22334, 1));
  ctx.setExtension( Atlas::ExtendedEventContext( &store ) );
  Gaudi::Hive::setCurrentContext( ctx );

  if (mdtool->registerFolder("/Generation/Parameters","Metadata created during Event Generation").isFailure())
  {
    std::abort();
  }

  auto cont = std::make_unique<CondAttrListCollection> (true);
  {
    CondAttrListCollection::AttributeList  myAttributes;
    myAttributes.extend("HepMCWeightNames","string");
    myAttributes.extend("HepMCWeightSvcVersion","int");
    myAttributes["HepMCWeightSvcVersion"].data<int>() = 2;
    myAttributes["HepMCWeightNames"].data<std::string>() = "{ 'one' : 1 , 'three' : 3 , 'two' : 2 }";
    if (!cont->add (1234, myAttributes)) std::abort();
  }
  {
    CondAttrListCollection::AttributeList  myAttributes;
    myAttributes.extend("HepMCWeightNames","string");
    myAttributes.extend("HepMCWeightSvcVersion","int");
    myAttributes["HepMCWeightSvcVersion"].data<int>() = 2;
    myAttributes["HepMCWeightNames"].data<std::string>() = "{ 'foo' : 10 }";
    if (!cont->add (5678, myAttributes)) std::abort();
  }
  if (mdtool->addPayload ("/Generation/Parameters", cont.release()).isFailure()) std::abort();

  {
    auto ev = std::make_unique<xAOD::EventInfo>();
    auto aux = std::make_unique<xAOD::EventAuxInfo>();
    ev->setStore (aux.get());
    ev->setMCChannelNumber (1234);
    store.record (ev.release(), "EventInfo");
    store.record (aux.release(), "EventInfoAux.");
  }

  IHepMCWeightSvc::WeightMap wmap_exp1 = { {"one", 1},
                                           {"two", 2},
                                           {"three", 3} };
  assert (svc->weightNames (ctx) == wmap_exp1);
  assert (svc->weightNameVec (ctx) == (std::vector<std::string> {"three", "two", "one"}));

  {
    auto ev = std::make_unique<xAOD::EventInfo>();
    auto aux = std::make_unique<xAOD::EventAuxInfo>();
    ev->setStore (aux.get());
    ev->setMCChannelNumber (5678);
    store.record (ev.release(), "EventInfo");
    store.record (aux.release(), "EventInfoAux.");
  }

  IHepMCWeightSvc::WeightMap wmap_exp2 = { {"foo", 10} };
  assert (svc->weightNames (ctx) == wmap_exp2);
  assert (svc->weightNameVec (ctx) == (std::vector<std::string> {"foo"}));
}


int main()
{
  std::cout << "HepMCWeightSvc/HepMCWeightSvc_test\n";

  ISvcLocator* svcLoc = nullptr;
  if (!Athena_test::initGaudi ("HepMCWeightSvc/HepMCWeightSvc_test.txt", svcLoc)) {
    std::cerr << "This test can not be run" << std::endl;
    return 0;
  }

  std::unique_ptr<SGTest::TestStore> store = SGTest::getTestStore();
  test1 (*store);
  test2 (*store);
  return 0;
}
