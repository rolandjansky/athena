/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4RunControl/LArG4HECOptions.h"

#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"


void LArG4HECOptions::saveMe()
{
  ServiceHandle<StoreGateSvc> detStore ("DetectorStore",
                                        "LArG4HECOptions::saveMe");
  if (detStore.retrieve().isSuccess()) {
    if (detStore->record(this,"LArG4HECOptions").isFailure())
      std::cout << "Can not record LArG4HECOptions" << std::endl;
  }
}

void LArG4HECOptions::printMe()
{
  std::cout << " ** HECBirksLaw = " << (m_HECBirksLaw ? "true" : "false") << "\n";
  std::cout << " ** HECBirksk  = " << m_HECBirksk << "\n *** *** \n";
  std::cout << " ** HECHVEnable  = " << m_HECHVEnable << "\n *** *** \n";
}
