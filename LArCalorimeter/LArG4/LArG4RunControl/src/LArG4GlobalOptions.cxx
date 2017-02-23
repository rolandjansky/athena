/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4RunControl/LArG4GlobalOptions.h"

#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"


void LArG4GlobalOptions::saveMe()
{
  ServiceHandle<StoreGateSvc> detStore ("DetectorStore",
                                        "LArG4GlobalOptions::saveMe");
  if (detStore.retrieve().isSuccess()) {
    if (detStore->record(this,"LArG4GlobalOptions").isFailure())
      std::cout << "Can not record LArG4GlobalOptions" << std::endl;
  }
}

void LArG4GlobalOptions::printMe()
{
  std::cout << " *** *** This is the object of type LArG4GlobalOptions *** *** \n";
  std::cout << " ** OutOfTimeCut = " << m_OutOfTimeCut << "\n *** *** \n";
}
