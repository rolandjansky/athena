/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <sys/time.h>
#include <iostream>
#include <iterator>
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/getMessageSvc.h"

#include "TestTools/initGaudi.h"


#include "TrigNavigation/TypeMaps.h"
#include "TrigNavigation/TriggerElement.h"
#include "TrigNavigation/Navigation.h"
#include "TrigNavigation/RoICacheHelper.h"
#include "TrigNavigation/TrigFeatureLink.h"



// tests hack
#include "TestTypes.h"
#include "TestUtils.h"


int main () {

  //  using namespace Athena_test;
  using namespace std;
  using namespace HLT;


  ISvcLocator* pSvcLoc;
  if (!Athena_test::initGaudi("test.txt",  pSvcLoc)) {
    cerr << "ERROR This test can not be run" << endl;
    return 0;
  }
  assert(pSvcLoc);


  MsgStream log(Athena::getMessageSvc(), "Registration_test"); 
  msglog = &log;



  StoreGateSvc* pStore(0);


  if( pSvcLoc->service("StoreGateSvc", pStore, true).isSuccess() ) {
    log << MSG::DEBUG << "SG pointer: " << pStore << endreq;
  } else 
    ABORT ( "ERROR no SG available" );
  
  IToolSvc* toolSvc;

  if( pSvcLoc->service("ToolSvc", toolSvc, true).isSuccess()  ) {
    log << MSG::DEBUG << "ToolSvc pointer: " << toolSvc << endreq;
  } else 
    ABORT( "ERROR no ToolSvc available" );


  HLT::Navigation* hns;
  IAlgTool* algTool;
  if ( toolSvc->retrieveTool("HLT::Navigation/Navigation", algTool).isSuccess() ) {
    log << MSG::DEBUG << "OK navigation tool retrieved" << endreq;
    hns = dynamic_cast< HLT::Navigation*>(algTool);
    if ( hns ) {
      log << MSG::DEBUG << "OK navigation casted" << endreq;    
    } else 
      ABORT( "ERROR navigation casted" );    
    
  } else 
    ABORT( "ERROR navigation tool NOT retrieved" );


  hns->reset( );
  hns->prepare();


  /*
  TestC* c = new TestC();
  std::string key;
  hns->attachFeature(hns->getInitialNode(), c, HLT::Navigation::ObjectCreatedByNew, key);

  TrigFeatureLink fl = hns->object2FeatureLink(hns->getInitialNode(), key, c );
  log << MSG::DEBUG << "fl " << fl.clid() << endreq;
  const TestC* c_back = hns->featureLink2Object<TestC>(fl);
  if ( c_back != c ) {
    log << MSG::ERROR << "failing the TrigFeatureLink get/restore"  << endreq;
  } else {
    log << MSG::DEBUG << "TrigFeatureLink worked ok" << endreq;
  }
  */

  log << MSG::DEBUG << pStore->dump() << endreq;
  REPORT_AND_CONTINUE ( "Test finished" );
  return 0;
}
