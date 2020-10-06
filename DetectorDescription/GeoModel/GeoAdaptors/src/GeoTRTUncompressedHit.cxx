/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "GeoAdaptors/GeoTRTUncompressedHit.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ServiceHandle.h"


const InDetDD::TRT_DetectorManager* GeoTRTUncompressedHit::init()
{
  ServiceHandle<StoreGateSvc> detStore ("DetectorStore", "GeoSiHit");
  const InDetDD::TRT_DetectorManager* mgr = nullptr;
  if(detStore->retrieve(mgr,"TRT").isFailure()) {
    std::abort();
  }
  return mgr;
}


const InDetDD::TRT_DetectorManager* GeoTRTUncompressedHit::mgr() const
{
  const InDetDD::TRT_DetectorManager* const mgr = init();
  return mgr;
}
