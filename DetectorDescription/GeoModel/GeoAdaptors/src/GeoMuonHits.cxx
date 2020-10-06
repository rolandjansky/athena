/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "GeoAdaptors/GeoMuonHits.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ServiceHandle.h"


const MuonGM::MuonDetectorManager* GeoMuonHitBase::init()
{
  ServiceHandle<StoreGateSvc> detStore ("DetectorStore", "GeoSiHit");
  const MuonGM::MuonDetectorManager* mgr = nullptr;
  if(detStore->retrieve(mgr,"Muon").isFailure()) {
    std::abort();
  }
  return mgr;
}


const MuonGM::MuonDetectorManager* GeoMuonHitBase::mgr() const
{
  const MuonGM::MuonDetectorManager* const mgr = init();
  return mgr;
}
