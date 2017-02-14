/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4RunControl/LArGeoTBGeometricOptions.h"

#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"

void LArGeoTBGeometricOptions::saveMe()
{
  ServiceHandle<StoreGateSvc> detStore ("DetectorStore",
                                        "LArGeoTBGeometricOptions::saveMe");
  if (detStore.retrieve().isSuccess()) {
    if (detStore->record(this,"LArGeoTBGeometricOptions").isFailure())
      std::cout << "Can not record LArGeoTBGeometricOptions" << std::endl;
  }
}

void LArGeoTBGeometricOptions::printMe()
{
  std::cout << " *** *** This is the object of type LArGeoTBGeometricOptions *** *** \n";
  std::cout << " ** CryoEtaPosition = " << m_CryoEtaPosition << "\n *** *** \n";
  std::cout << " ** CryoPhiPosition = " << m_CryoPhiPosition << "\n *** *** \n";
}
