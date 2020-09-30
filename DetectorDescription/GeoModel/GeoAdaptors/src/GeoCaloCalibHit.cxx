/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "GeoAdaptors/GeoCaloCalibHit.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ServiceHandle.h"


const CaloDetDescrManager* GeoCaloCalibHit::initDDMgr()
{
  ServiceHandle<StoreGateSvc> detStore ("DetectorStore", "GeoLArHit");
  const CaloDetDescrManager* mgr = nullptr;
  if (detStore->retrieve(mgr).isFailure()) {
    std::abort();
  }
  return mgr;
}


const CaloDetDescrManager* GeoCaloCalibHit::ddmgr() const
{
  const CaloDetDescrManager* const ddmgr = initDDMgr();
  return ddmgr;
}

