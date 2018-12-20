/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include <iostream>
#include "TestTools/expect.h"
#include "TestTools/initGaudi.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"
#include "../src/BareDataBucket.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"


int main() {
  using namespace std;
  ISvcLocator* pSvcLoc;
  if (!Athena_test::initGaudi("test.txt",  pSvcLoc)) {
    cerr << "ERROR This test can not be run" << endl;
    return 0;
  }
  assert(pSvcLoc);
  MsgStream log(Athena::getMessageSvc(), "void_record_test");
  
  StoreGateSvc* pStore = 0;
  if( pSvcLoc->service("StoreGateSvc", pStore, true).isSuccess() ) {
    log << MSG::INFO << "SG pointer: " << pStore << endmsg;
  } else {
    log << MSG::ERROR << "can't obtain SG ptr" << endmsg;
    return -1;
  }

  xAOD::TrigCompositeContainer* testContainer = new xAOD::TrigCompositeContainer();
  xAOD::TrigCompositeAuxContainer* aux = new xAOD::TrigCompositeAuxContainer();
  testContainer->setStore( aux );
  
  testContainer->push_back( new xAOD::TrigComposite() );
  testContainer->push_back( new xAOD::TrigComposite() );
  testContainer->at(0)->setDetail( "idetail", 7 );
  testContainer->at(1)->setDetail( "fdetail", 2.5f );
  void * rawContainerPtr = static_cast<void*>( testContainer );
  void * rawStorePtr = static_cast<void*>( aux );

  SG::BaseInfo<xAOD::TrigCompositeContainer>::baseinfo(); // this is problematic because client code does not know about templates, will have to see in athena if the problem persists

  RootType containerRT = RootType::ByName( "xAOD::TrigCompositeContainer_v1" );  
  log << MSG::INFO << containerRT.Name() << endmsg;
  BareDataBucket containerDataBucket( rawContainerPtr, ClassID_traits<xAOD::TrigCompositeContainer>::ID(), containerRT ); 
  
  RootType storeRT = RootType::ByName( "xAOD::TrigCompositeAuxContainer_v1" );
  log << MSG::INFO << storeRT.Name() << endmsg;
  BareDataBucket storeDataBucket( rawStorePtr, ClassID_traits<xAOD::TrigCompositeAuxContainer>::ID(), storeRT ); 

  pStore->recordObject( SG::DataObjectSharedPtr<BareDataBucket>( &containerDataBucket ), "test", false, false );
  pStore->recordObject( SG::DataObjectSharedPtr<BareDataBucket>( &storeDataBucket ), "testAux.", false, false );

  log << MSG::INFO << "recordObject done" << endmsg;


  VALUE( pStore->accessData( ClassID_traits<xAOD::TrigCompositeContainer>::ID(),  "test" ) ) NOT_EXPECTED ( nullptr );
  VALUE( pStore->accessData( ClassID_traits<xAOD::TrigCompositeAuxContainer>::ID(),  "testAux." ) ) NOT_EXPECTED ( nullptr );

  log << MSG::INFO << "objects in store, trying to read them back via rertieve" << endmsg;

  const xAOD::TrigCompositeContainer* containerBack = 0;
  pStore->retrieve( containerBack, "test" );
  
  VALUE ( containerBack ) NOT_EXPECTED ( nullptr );
  VALUE( containerBack->size() ) EXPECTED ( 2 );
  VALUE( containerBack->at(0)->getDetail<int32_t>( "idetail" ) ) EXPECTED ( 7 );
  VALUE( containerBack->at(1)->getDetail<float>( "fdetail" ) ) EXPECTED ( 2.5f );

  log << MSG::INFO << "Container read back is identical" << endmsg;
  return 0;
}
