
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include <iostream>

#include "TestTools/expect.h"
#include "TestTools/initGaudi.h"
#include "GaudiKernel/IToolSvc.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "../src/BareDataBucket.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigCalo/TrigEMClusterAuxContainer.h"
#include "xAODTrigCalo/versions/TrigEMClusterAuxContainer_v1.h"

#include "../src/TriggerEDMSerialiserTool.h"
#include "../src/TriggerEDMDeserialiserAlg.h"

#include "CxxUtils/checker_macros.h"
#include "CxxUtils/ubsan_suppress.h"
#include "TInterpreter.h"

std::vector<uint32_t> serialisedData ATLAS_THREAD_SAFE; //Only used in test program
StatusCode tester( TriggerEDMSerialiserTool* ser) {
    auto em = new xAOD::TrigEMClusterContainer();
    auto emAux = new xAOD::TrigEMClusterAuxContainer_v1();
    em->setStore( emAux );
    
    auto cluster = new xAOD::TrigEMCluster();
    float x = 1.0;
    em->push_back( cluster );
    cluster->setEnergy(0.0);
    cluster->setEt(0.0);
    cluster->setRawEnergy(0.0);
    cluster->setRawEt(x - 0.2);
    cluster->setE277(0);
    
    // got trivial object to serialise, need to create addresses
    TriggerEDMSerialiserTool::Address interfaceAddress{
      "xAOD::TrigEMClusterContainer", "xAOD::TrigEMClusterContainer_v1",
      1264979038/*clid*/, "HLT_one", {},
      TriggerEDMSerialiserTool::Address::Category::xAODInterface};

    TriggerEDMSerialiserTool::Address auxAddress{
      "xAOD::TrigEMClusterAuxContainer", "xAOD::TrigEMClusterAuxContainer_v1",
      1111649561/*clid*/, "HLT_oneAux.", {},
      TriggerEDMSerialiserTool::Address::Category::xAODAux};
     
    auto status = ser->serialiseContainer( (void*)em, interfaceAddress, serialisedData );
    VALUE( status ) EXPECTED( StatusCode::SUCCESS );

    const EventContext ctx = Gaudi::Hive::currentContext();  
    SGImplSvc* evtStore = static_cast<SGImplSvc*>(Atlas::getExtendedEventContext(ctx).proxy());
        
    status = ser->serialisexAODAuxContainer( (void*)emAux, auxAddress, serialisedData, evtStore );
    VALUE( status ) EXPECTED( StatusCode::SUCCESS );  

    return StatusCode::SUCCESS;
}

int main() {
  CxxUtils::ubsan_suppress ( []() { TInterpreter::Instance(); } );
  using namespace std;
  ISvcLocator* pSvcLoc;
  if( !Athena_test::initGaudi("test.txt",  pSvcLoc) ) {
    cerr << "ERROR This test can not be run" << endl;
    return -1;
  }
  assert(pSvcLoc);
  MsgStream log(Athena::getMessageSvc(), "serial_deserial_test");
  
  StoreGateSvc* pStore = nullptr;
  VALUE( pSvcLoc->service("StoreGateSvc", pStore, true).isSuccess() ) EXPECTED ( true );

  IToolSvc * toolSvc = nullptr;
  VALUE( pSvcLoc->service("ToolSvc", toolSvc, true) ) EXPECTED ( StatusCode::SUCCESS );
  
  IAlgTool* algTool;
  VALUE( toolSvc->retrieveTool("TriggerEDMSerialiserTool/schemaTest", algTool) ) EXPECTED( StatusCode::SUCCESS );
  TriggerEDMSerialiserTool* ser = dynamic_cast< TriggerEDMSerialiserTool*>(algTool);
  VALUE( ser == nullptr ) EXPECTED ( false );

  TriggerEDMDeserialiserAlg deser ("deserialiser", pSvcLoc);  deser.addRef();
  VALUE( deser.sysInitialize() ) EXPECTED ( StatusCode::SUCCESS );

  IProxyDict* xdict = &*deser.evtStore();
  xdict = deser.evtStore()->hiveProxyDict();
  EventContext ctx;
  ctx.setExtension( Atlas::ExtendedEventContext(xdict) );
  Gaudi::Hive::setCurrentContext (ctx);

  VALUE( tester( ser ) ) EXPECTED( StatusCode::SUCCESS );

  auto result = new HLT::HLTResultMT();
  result->addSerialisedData( 0, serialisedData );

  VALUE( pStore->record(result, "HLTResultMT") ) EXPECTED ( StatusCode::SUCCESS );

  VALUE( deser.execute( ctx ) ) EXPECTED ( StatusCode::SUCCESS );

}
