/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4RunControl/LArG4FCALOptions.h"

#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"


void LArG4FCALOptions::saveMe()
{
  ServiceHandle<StoreGateSvc> detStore ("DetectorStore",
                                        "LArG4FCALOptions::saveMe");
  if (detStore.retrieve().isSuccess()) {
    if (detStore->record(this,"LArG4FCALOptions").isFailure())
      std::cout << "Can not record LArG4FCALOptions" << std::endl;
  }
}

void LArG4FCALOptions::printMe()
{
  std::cout << " ** FCALBirksLaw = " << (m_FCALBirksLaw ? "true" : "false") << "\n";
  std::cout << " ** FCALBirksk  = " << m_FCALBirksk << "\n *** *** \n";
  std::cout << " ** FCALHVEnable  = " << m_FCALHVEnable << "\n *** *** \n";
}
