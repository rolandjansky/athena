/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSERVMATGEOMODEL_PIXELSERVMATFACTORYFS_H
#define INDETSERVMATGEOMODEL_PIXELSERVMATFACTORYFS_H

#include "AthenaBaseComps/AthMessaging.h"
#include "GaudiKernel/ServiceHandle.h"

class StoreGateSvc;
class GeoPhysVol;
class IRDBAccessSvc;
class InDetMaterialManager;

// Pixel service material factory for Frozen Showers (FS)
class PixelServMatFactoryFS : public AthMessaging  {

 public:
  
  // Constructor:
  PixelServMatFactoryFS(StoreGateSvc *pDetStore,
			ServiceHandle<IRDBAccessSvc> pRDBAccess);
  
  // Destructor:
  ~PixelServMatFactoryFS();
  
  // Creation of geometry:
  void create(GeoPhysVol *motherP, GeoPhysVol *motherM);

 private:  
  
  // Illegal operations:
  const PixelServMatFactoryFS & operator=(const PixelServMatFactoryFS &right);
  PixelServMatFactoryFS(const PixelServMatFactoryFS &right);

  // private data
  StoreGateSvc                    *m_detStore;
  ServiceHandle<IRDBAccessSvc>     m_rdbAccess;
  std::unique_ptr<InDetMaterialManager> m_materialManager;
};

#endif 
