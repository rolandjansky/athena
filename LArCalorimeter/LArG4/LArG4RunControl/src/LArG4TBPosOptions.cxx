/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4RunControl/LArG4TBPosOptions.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "StoreGate/StoreGateSvc.h"

void LArG4TBPosOptions::saveMe()
{
  IService* pSvc;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode result = svcLocator->service("DetectorStore",pSvc);

  if(result.isSuccess())
  {
    StoreGateSvc* detStore = dynamic_cast<StoreGateSvc*>(pSvc);
    if (!detStore){
      std::cout << "LArG4TBPosOptions::saveMe ERROR Could not dynamic cast det store" << std::endl;
      return;
    }
    result=detStore->record(this,"LArG4TBPosOptions");
    if(!result.isSuccess())
      std::cout << "Can not record LArG4TBPosOptions" << std::endl;

  }
}

void LArG4TBPosOptions::printMe()
{
  std::cout << " *** *** This is the object of type LArG4TBPosOptions *** *** \n";
  std::cout << " ** PositionNickname   = " << m_PositionNickname << "\n *** *** \n";
  std::cout << " ** PositionNicknumber = " << m_PositionNicknumber << "\n *** *** \n";
}
