/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4RunControl/LArG4GlobalOptions.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "StoreGate/StoreGateSvc.h"


void LArG4GlobalOptions::saveMe()
{
  IService* pSvc;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode result = svcLocator->service("DetectorStore",pSvc);

  if(result.isSuccess())
  {
    StoreGateSvc* m_detStore = dynamic_cast<StoreGateSvc*>(pSvc);
    result=m_detStore->record(this,"LArG4GlobalOptions");
    if(!result.isSuccess())
      std::cout << "Can not record LArG4BarrelOptions" << std::endl;

  }
}

void LArG4GlobalOptions::printMe()
{
  std::cout << " *** *** This is the object of type LArG4GlobalOptions *** *** \n";
  std::cout << " ** OutOfTimeCut = " << m_OutOfTimeCut << "\n *** *** \n";
}
