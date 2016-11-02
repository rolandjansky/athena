/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>

//#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
//#include "StoreGate/StoreGate.h"
//#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/getMessageSvc.h"


#include "TestTools/initGaudi.h"

//#include "TrigNavigation/TriggerElement.h"
//#include "TrigNavigation/Navigation.h"
//#include "TrigNavigation/RoICacheHelper.h"

#include "TrigTimeAlgs/ITrigTimerSvc.h"
#include "TrigTimeAlgs/TrigTimer.h"
int main() {
  using namespace std;

  ISvcLocator* pSvcLoc;
  if (!Athena_test::initGaudi("test.txt",  pSvcLoc)) {
    cerr << "ERROR This test can not be run, can not get ServiceLocator" << endl;
    return -1;
  }
  assert(pSvcLoc);
  
  //  getThe timers service
  //  MsgStream log(pSvcLoc->messageService(), "TimerSvc_test");

  ITrigTimerSvc *pTimerSvc(0);
  if( pSvcLoc->service("TrigTimerSvc", pTimerSvc, true).isSuccess() ) {
    cerr << "TrigTimerSvc ptr:: " << pTimerSvc << endl;
  } else {
    cerr << "ERROR no TrigTimerSvc available" << endl;
    return -1;
  }
  
  TrigTimer* timerA = pTimerSvc->addItem("timerA");
  TrigTimer* timerB = pTimerSvc->addItem("timerB");
  timerA->start();
  timerB->start();
  timerB->stop();
  timerA->stop();

  // basic consitency check
  if ( timerB->elapsed() > timerA->elapsed() ) {
    cerr << "ERROR timers behave inconsitenctly " << endl; 
    return -1;
  }

  // performance test
  int N = 1e6;
  timerB->start();
  for (int n=0; n<N; ++n) {
    timerA->start();
    timerA->stop();
  }
  timerB->stop();

  pTimerSvc->print();
  
  cerr << "SUCCESS test passed OK" << endl;
  cout << timerB->elapsed()/N*1000 <<  " ms per 1000 stop/start calls" << endl;
  return 0;
}
