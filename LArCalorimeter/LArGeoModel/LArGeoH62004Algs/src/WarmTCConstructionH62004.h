/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// WarmTCConstructionH62004

#ifndef LARGEOH62004ALGS_WARMTCCONSTRUCTIONH62004_H
#define LARGEOH62004ALGS_WARMTCCONSTRUCTIONH62004_H

#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoPhysVol.h"

#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "StoreGate/StoreGateSvc.h"

namespace LArGeo {
   
  class WarmTCConstructionH62004 {
        public:
           WarmTCConstructionH62004();
           virtual ~WarmTCConstructionH62004();
	   GeoVFullPhysVol* GetEnvelope();

        private:
	   GeoFullPhysVol*  m_WarmTCPhys;
	   StoreGateSvc * m_detectorStore = nullptr;
  };
}

#endif // LARGEOH62004ALGS_WARMTCCONSTRUCTIONH62004_H
