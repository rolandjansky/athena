/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4RunControl/LArGeoTBH1GeoOptions.h"

#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"

void LArGeoTBH1GeoOptions::saveMe()
{
  ServiceHandle<StoreGateSvc> detStore ("DetectorStore",
                                        "LArGeoTBH1GeoOptions::saveMe");
  if (detStore.retrieve().isSuccess()) {
    if (detStore->record(this,"LArGeoTBH1GeoOptions").isFailure())
      std::cout << "Can not record LArGeoTBH1GeoOptions" << std::endl;
  }
}

void LArGeoTBH1GeoOptions::printMe()
{
  std::cout << " *** *** This is the object of type LArGeoTBH1GeoOptions *** *** \n";
  std::cout << " ** H1 Cryostat X Position = " << m_CryoXPosition << "\n *** *** \n";
  std::cout << " ** H1 Table Y Position = " << m_TableYPosition << "\n *** *** \n";
}
