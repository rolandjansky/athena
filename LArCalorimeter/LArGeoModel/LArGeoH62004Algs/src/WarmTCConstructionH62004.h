/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// WarmTCConstructionH62004

#ifndef __WarmTCConstructionH62004_H__
#define __WarmTCConstructionH62004_H__

#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoPhysVol.h"

#include "StoreGate/DataHandle.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"

namespace LArGeo {
   
  class WarmTCConstructionH62004 {
        public:
           WarmTCConstructionH62004();
           virtual ~WarmTCConstructionH62004();
	   GeoVFullPhysVol* GetEnvelope();

        private:
	   GeoFullPhysVol*  m_WarmTCPhys;
	   StoreGateSvc * m_detectorStore;
	   DataHandle<StoredMaterialManager> m_materialManager;
  };
}

#endif // __WarmTCConstructionH62004_H__
