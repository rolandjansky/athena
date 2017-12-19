/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "iostream"
#include "vector"

#include "TestTools/initGaudi.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"
#include "TrigNavigation/Holder.h"
#include "TrigNavigation/Holder.icc"

#include "TrigNavigation/TriggerElement.h"


#include "TestTypes.h"
#include "TestUtils.h"

using namespace std;
using namespace HLTNavDetails;
using namespace HLT;
using namespace TrigNavTest;
// Hi-lock: (("REPORT_AND_STOP" (0 (quote hi-yellow) t)))

//const int OK=1;
//const int FAILED=0;

StoreGateSvc* pStore(0);
HLT::Navigation* hns;


template <class T> 
std::string view_or_own (T* c) {
  return ((c->ownPolicy() == SG::VIEW_ELEMENTS) ? "VIEW" : "OWN");
}

TriggerElement * makeTE(HLT::Navigation* hns) {
  TriggerElement* initial =  hns->getInitialNode();
  TriggerElement* roi =  hns->addRoINode(initial);
  TriggerElement* te =  hns->addNode(roi, 23);
  return te;
}


TestBContainer* makeContainer(SG::OwnershipPolicy policy, int initval) {
  TestBContainer* c = new TestBContainer();
  c->clear(policy);
  c->push_back(new TestB(initval));
  c->push_back(new TestB(initval));
  c->push_back(new TestB(initval+1));
  return c;
  
}

bool noMixTest() {
  BEGIN_TEST("noMixTest");
  pStore->clearStore();
  hns->reset();
  hns->prepare();
  
  TestBContainer *b1 = makeContainer(SG::OWN_ELEMENTS, 2);
  TestBContainer *b2 = makeContainer(SG::VIEW_ELEMENTS, 77);
  TriggerElement* te =  makeTE(hns);
  REPORT_AND_CONTINUE("Objects prepared");
  std::string key_back; 

  bool stat = hns->attachFeature(te, b1, Navigation::ObjectCreatedByNew, key_back, "BContainer1");  
  if (  stat == false ) {
    REPORT_AND_STOP("attachFeature failed");
  }

  REPORT_AND_CONTINUE("After attachFeture the container is " << view_or_own(b1));

  stat = hns->attachFeature(te, b2, Navigation::ObjectCreatedByNew, key_back, "BContainer1");  
  if (  stat == true ) {
    REPORT_AND_STOP("attachFeature worked while it should not");
  }
  REPORT_AND_CONTINUE("The attachFeature was unsuccesfull (as expected)");

  END_TEST;
}

bool firsInsertDecidesPolicy() {
  BEGIN_TEST("testing if the insert operation on an empty container makes it a view container");
  pStore->clearStore();
  hns->reset();
  hns->prepare();
  
  TestDContainer *b1 = new TestDContainer();
  b1->clear(SG::VIEW_ELEMENTS, SG::NEVER_TRACK_INDICES);
  b1->push_back(new TestD(9));
  b1->push_back(new TestD(10));

  TriggerElement* te =  makeTE(hns);
  REPORT_AND_CONTINUE("Objects prepared");
  std::string key_back; 

  const TestDContainer* ewise(0);
  if ( pStore->retrieve(ewise, "HLT_TestDContainer_DContainer1").isFailure() ) {
    REPORT_AND_CONTINUE(pStore->dump());
    REPORT_AND_STOP("could not get the event-wise container from SG");
  }

  REPORT_AND_CONTINUE("Event wise container has ownership policy " << view_or_own(ewise));  
  if ( view_or_own(ewise) != "OWN" ) {
    REPORT_AND_STOP("At start the event wise container is not OWNing");
  }

  // tested code
  bool stat = hns->attachFeature(te, b1, Navigation::ObjectCreatedByNew, key_back, "DContainer1");  
  if (  stat == false ) {
    REPORT_AND_STOP("attachFeature failed");
  }

  ewise = 0;
  if ( pStore->retrieve(ewise, "HLT_TestDContainer_DContainer1").isFailure() ) {
    REPORT_AND_STOP("could not get the event-wise container from SG");
  }

  REPORT_AND_CONTINUE("After attachFeture the container is " << view_or_own(ewise));
  if ( view_or_own(ewise) != "VIEW" ) {
    REPORT_AND_STOP("At end the event wise container is not VIEW");
  }

  END_TEST;
}

bool viewContainerAttachTest() {
  BEGIN_TEST("testing if the xAOD view container works");
  TestBContainer *b1 = makeContainer(SG::OWN_ELEMENTS, 2);
  TestBContainerView* view1 = new TestBContainerView();
  TestBContainerView* view2 = new TestBContainerView();
  REPORT_AND_CONTINUE("Objects prepared");
  view1->push_back(b1->at(0));
  view1->push_back(b1->at(2)); // i.e. the el of index 1 is missing from the view

  view2->push_back(b1->at(0));
  view2->push_back(b1->at(1));

  REPORT_AND_CONTINUE("View filled");
  TriggerElement* te =  makeTE(hns);
  REPORT_AND_CONTINUE("TE in place - attaching");
  std::string key_back;
  bool stat = hns->attachFeature(te, view1, Navigation::ObjectCreatedByNew, key_back, "BView");  
  if (  stat == false ) {
    REPORT_AND_STOP("attachFeature failed (for the first view)");
  }
  REPORT_AND_CONTINUE("First View attach worked");
  // another attach ( this is append effectively ) 
  stat = hns->attachFeature(te, view2, Navigation::ObjectCreatedByNew, key_back, "BView");  
  if (  stat == false ) {
    REPORT_AND_STOP("attachFeature failed (for the second view)");
  }
  REPORT_AND_CONTINUE("Second View attach worked");


  END_TEST;
}




int main() {


   ISvcLocator* pSvcLoc;
   if (!Athena_test::initGaudi("test.txt",  pSvcLoc)) {
     cerr << "ERROR This test can not be run" << endl;
     return 0;
   }
   assert(pSvcLoc);
   MsgStream log(Athena::getMessageSvc(), "Ownership_test");
   msglog = &log;

  if( pSvcLoc->service("StoreGateSvc", pStore, true).isSuccess() ) {
    *msglog << MSG::DEBUG << "SG pointer: " << pStore << endmsg;
  } else {
    ABORT( "ERROR no SG available" );
  }

  IToolSvc* toolSvc;

  if( pSvcLoc->service("ToolSvc", toolSvc, true).isSuccess()  ) {
    log << MSG::DEBUG << "ToolSvc pointer: " << toolSvc << endmsg;
  } else 
    ABORT ( "no ToolSvc available" );



  IAlgTool* algTool;
  if ( toolSvc->retrieveTool("HLT::Navigation/Navigation", algTool).isSuccess() ) {
    log << MSG::DEBUG << "OK navigation tool retrieved" << endmsg;
    hns = dynamic_cast< HLT::Navigation*>(algTool);
    if ( hns ) {
      log << MSG::DEBUG << "OK navigation casted" << endmsg;    
    } else 
      ABORT( "navigation cast failed" );    

  } else  ABORT("navigation tool NOT retrieved" );

  if ( !firsInsertDecidesPolicy() ) 
    ABORT("");
  
  

  if ( !noMixTest() ) 
    ABORT("");
  
  

  if ( !viewContainerAttachTest() ) {
    ABORT("");
  }


  REPORT_AND_CONTINUE( "END all went fine" );
  return 0;

  *msglog << MSG::INFO << "SUCCESS " << endmsg;
  *msglog << MSG::DEBUG << "deleting messages below are related to scope exit " << endmsg;
  
  return 0;
}
