/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4RunControl/LArG4TBPosOptions.h"

#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"

void LArG4TBPosOptions::saveMe()
{
  ServiceHandle<StoreGateSvc> detStore ("DetectorStore",
                                        "LArG4TBPosOptions::saveMe");
  if (detStore.retrieve().isSuccess()) {
    if (detStore->record(this,"LArG4TBPosOptions").isFailure())
      std::cout << "Can not record LArG4TBPosOptions" << std::endl;
  }
}

void LArG4TBPosOptions::printMe()
{
  std::cout << " *** *** This is the object of type LArG4TBPosOptions *** *** \n";
  std::cout << " ** PositionNickname   = " << m_PositionNickname << "\n *** *** \n";
  std::cout << " ** PositionNicknumber = " << m_PositionNicknumber << "\n *** *** \n";
}
