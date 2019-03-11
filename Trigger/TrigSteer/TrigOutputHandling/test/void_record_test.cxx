/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
  using TrigCompositeUtils::DecisionID;

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

  std::vector<DecisionID> decisions = {2, 4, 6};

  xAOD::TrigCompositeContainer* testContainer = new xAOD::TrigCompositeContainer();
  xAOD::TrigCompositeAuxContainer* aux = new xAOD::TrigCompositeAuxContainer();
  testContainer->setStore( aux );
  
  // Note: idetail and fdetail are dynamic decorations.
  // The decoration must be set on all objects within the container

  testContainer->push_back( new xAOD::TrigComposite() );
  testContainer->back()->setDetail( "idetail", 7 );
  testContainer->back()->setDetail( "fdetail", 2.5f );
  testContainer->back()->setDetail( "decisions", decisions );

  testContainer->push_back( new xAOD::TrigComposite() );
  testContainer->back()->setDetail( "idetail", 8 );
  testContainer->back()->setDetail( "fdetail", 3.5f );
  testContainer->back()->setDetail( "decisions", decisions );
  testContainer->back()->setObjectLink( "link", ElementLink<xAOD::TrigCompositeContainer>( 123, 456, testContainer->at(0) ) );
  // Note the SG part of this link is junk data, but the third parameter is real. Meaning it will pass a isValid sanity check

  void * rawContainerPtr = static_cast<void*>( testContainer );
  void * rawStorePtr = static_cast<void*>( aux );

  SG::BaseInfo<xAOD::TrigCompositeContainer>::baseinfo(); // this is problematic because client code does not know about templates, will have to see in athena if the problem persists

  RootType containerRT = RootType::ByName( "xAOD::TrigCompositeContainer_v1" );  
  log << MSG::INFO << containerRT.Name() << endmsg;
  BareDataBucket containerDataBucket( rawContainerPtr, ClassID_traits<xAOD::TrigCompositeContainer>::ID(), containerRT ); 
  
  RootType storeRT = RootType::ByName( "xAOD::TrigCompositeAuxContainer_v2" );
  log << MSG::INFO << storeRT.Name() << endmsg;
  BareDataBucket storeDataBucket( rawStorePtr, ClassID_traits<xAOD::TrigCompositeAuxContainer>::ID(), storeRT ); 
  log << MSG::INFO << "recordObject done" << endmsg;

  pStore->recordObject( SG::DataObjectSharedPtr<BareDataBucket>( &containerDataBucket ), "test", false, false );
  pStore->recordObject( SG::DataObjectSharedPtr<BareDataBucket>( &storeDataBucket ), "testAux.", false, false );

  log << MSG::INFO << "recordObject done" << endmsg;

  VALUE( pStore->accessData( ClassID_traits<xAOD::TrigCompositeContainer>::ID(),  "test" ) ) NOT_EXPECTED ( nullptr );
  VALUE( pStore->accessData( ClassID_traits<xAOD::TrigCompositeAuxContainer>::ID(),  "testAux." ) ) NOT_EXPECTED ( nullptr );

  log << MSG::INFO << "objects in store, trying to read them back via retrieve" << endmsg;

  const xAOD::TrigCompositeContainer* containerBack = 0;
  pStore->retrieve( containerBack, "test" );
  
  log << MSG::INFO << "Check retrieve" << endmsg;
  VALUE ( containerBack ) NOT_EXPECTED ( nullptr );
  log << MSG::INFO << "Check size" << endmsg;
  VALUE( containerBack->size() ) EXPECTED ( 2 );
  log << MSG::INFO << "Check [0] int32_t" << endmsg;
  VALUE( containerBack->at(0)->getDetail<int32_t>( "idetail" ) ) EXPECTED ( 7 );
  log << MSG::INFO << "Check [0] float" << endmsg;
  VALUE( containerBack->at(0)->getDetail<float>( "fdetail" ) ) EXPECTED ( 2.5f );
  log << MSG::INFO << "Check [0] std::vector<DecisionID>" << endmsg;
  VALUE( containerBack->at(0)->getDetail<std::vector<DecisionID>>( "decisions" )[0] ) EXPECTED ( decisions[0] );
  VALUE( containerBack->at(0)->getDetail<std::vector<DecisionID>>( "decisions" )[1] ) EXPECTED ( decisions[1] );
  VALUE( containerBack->at(0)->getDetail<std::vector<DecisionID>>( "decisions" )[2] ) EXPECTED ( decisions[2] );
  log << MSG::INFO << "Check [1] int32_t" << endmsg;
  VALUE( containerBack->at(1)->getDetail<int32_t>( "idetail" ) ) EXPECTED ( 8 );
  log << MSG::INFO << "Check [1] float" << endmsg;
  VALUE( containerBack->at(1)->getDetail<float>( "fdetail" ) ) EXPECTED ( 3.5f );
  log << MSG::INFO << "Check [1] link" << endmsg;
  ElementLink<xAOD::TrigCompositeContainer> link = containerBack->at(1)->objectLink<xAOD::TrigCompositeContainer>("link");
  VALUE( link.key() ) EXPECTED ( 123 );
  VALUE( link.index() ) EXPECTED ( 456 );

  log << MSG::INFO << "Container read back is identical" << endmsg;
  return 0;
}
