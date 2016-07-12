/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4RunControl/LArG4FCALOptions.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "StoreGate/StoreGateSvc.h"


void LArG4FCALOptions::saveMe()
{
  IService* pSvc;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode result = svcLocator->service("DetectorStore",pSvc);

  if(result.isSuccess())
  {
    StoreGateSvc* detStore = dynamic_cast<StoreGateSvc*>(pSvc);
    result=detStore->record(this,"LArG4FCALOptions");

   if(!result.isSuccess())
       std::cout << "Can not record LArG4FCALOptions" << std::endl;

  }
}

void LArG4FCALOptions::printMe()
{
  std::cout << " ** FCALBirksLaw = " << (m_FCALBirksLaw ? "true" : "false") << "\n";
  std::cout << " ** FCALBirksk  = " << m_FCALBirksk << "\n *** *** \n";
  std::cout << " ** FCALHVEnable  = " << m_FCALHVEnable << "\n *** *** \n";
}
