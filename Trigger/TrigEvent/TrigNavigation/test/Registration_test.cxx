/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <sys/time.h>
#include <iostream>
#include <iterator>
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/MsgStream.h"
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
    log << MSG::DEBUG << "SG pointer: " << pStore << endmsg;
  } else 
    ABORT ( "ERROR no SG available" );
  
  IToolSvc* toolSvc;

  if( pSvcLoc->service("ToolSvc", toolSvc, true).isSuccess()  ) {
    log << MSG::DEBUG << "ToolSvc pointer: " << toolSvc << endmsg;
  } else 
    ABORT( "ERROR no ToolSvc available" );


  HLT::Navigation* hns;
  IAlgTool* algTool;
  if ( toolSvc->retrieveTool("HLT::Navigation/Navigation", algTool).isSuccess() ) {
    log << MSG::DEBUG << "OK navigation tool retrieved" << endmsg;
    hns = dynamic_cast< HLT::Navigation*>(algTool);
    if ( hns ) {
      log << MSG::DEBUG << "OK navigation casted" << endmsg;    
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
  log << MSG::DEBUG << "fl " << fl.clid() << endmsg;
  const TestC* c_back = hns->featureLink2Object<TestC>(fl);
  if ( c_back != c ) {
    log << MSG::ERROR << "failing the TrigFeatureLink get/restore"  << endmsg;
  } else {
    log << MSG::DEBUG << "TrigFeatureLink worked ok" << endmsg;
  }
  */

  log << MSG::DEBUG << pStore->dump() << endmsg;
  REPORT_AND_CONTINUE ( "Test finished" );
  return 0;
}
