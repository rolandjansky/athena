/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4RunControl/LArG4HECOptions.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "StoreGate/StoreGateSvc.h"


void LArG4HECOptions::saveMe()
{
  IService* pSvc;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode result = svcLocator->service("DetectorStore",pSvc);

  if(result.isSuccess())
  {
    StoreGateSvc* detStore = dynamic_cast<StoreGateSvc*>(pSvc);
    result=detStore->record(this,"LArG4HECOptions");

   if(!result.isSuccess())
       std::cout << "Can not record LArG4HECOptions" << std::endl;

  }
}

void LArG4HECOptions::printMe()
{
  std::cout << " ** HECBirksLaw = " << (m_HECBirksLaw ? "true" : "false") << "\n";
  std::cout << " ** HECBirksk  = " << m_HECBirksk << "\n *** *** \n";
  std::cout << " ** HECHVEnable  = " << m_HECHVEnable << "\n *** *** \n";
}
