/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoAdaptors/GeoLArHit.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ServiceHandle.h"


const CaloDetDescrManager* GeoLArHit::init()
{
  ServiceHandle<StoreGateSvc> detStore ("DetectorStore", "GeoLArHit");
  const CaloDetDescrManager* mgr = nullptr;
  if (detStore->retrieve(mgr).isFailure()) {
    std::abort();
  }
  return mgr;
}


const CaloDetDescrManager* GeoLArHit::mgr() const
{
  static const CaloDetDescrManager* const mgr = init();
  return mgr;
}

