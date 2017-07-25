/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SimHelpers/AthenaHitsCollectionHelper.h"
#include <iostream>

// Simple init
AthenaHitsCollectionHelper::AthenaHitsCollectionHelper()
   : m_storeGate(0) 
{}

// Real initialization - done by being lazy
void AthenaHitsCollectionHelper::initialize(){
  ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap
      
  IMessageSvc*	msgSvc;
  StatusCode status = svcLocator->service("MessageSvc", msgSvc);
  if (status.isFailure()) 
  {
    std::cout<<"AthenaHitsCollectionHelper: could not access MessageSvc!  Will crash in a moment..."<<std::endl;
  }
			
  MsgStream log(msgSvc, "AthenaHitsCollectionHelper");
  log << MSG::INFO <<" new collection helper being created "<<std::endl;
  status = svcLocator->service("StoreGateSvc", m_storeGate);
  if (status.isFailure()) 
  {
    log << MSG::WARNING <<"AthenaHitsCollectionHelper: could not accessStoreGateSvc!"<<std::endl;
  }
}

// Simple destructor
AthenaHitsCollectionHelper::~AthenaHitsCollectionHelper()
{}
