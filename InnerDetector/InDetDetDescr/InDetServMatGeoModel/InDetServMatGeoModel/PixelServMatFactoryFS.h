/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSERVMATGEOMODEL_PIXELSERVMATFACTORYFS_H
#define INDETSERVMATGEOMODEL_PIXELSERVMATFACTORYFS_H

#include "AthenaKernel/MsgStreamMember.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/ServiceHandle.h"

class StoreGateSvc;
class GeoPhysVol;
class IRDBAccessSvc;
class InDetMaterialManager;

// Pixel service material factory for Frozen Showers (FS)
class PixelServMatFactoryFS   {

 public:
  
  // Constructor:
  PixelServMatFactoryFS(StoreGateSvc *pDetStore,
			ServiceHandle<IRDBAccessSvc> pRDBAccess);
  
  // Destructor:
  ~PixelServMatFactoryFS();
  
  // Creation of geometry:
  void create(GeoPhysVol *motherP, GeoPhysVol *motherM);

  MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }

 private:  
  
  // Illegal operations:
  const PixelServMatFactoryFS & operator=(const PixelServMatFactoryFS &right);
  PixelServMatFactoryFS(const PixelServMatFactoryFS &right);

  // private data
  StoreGateSvc                    *m_detStore;
  ServiceHandle<IRDBAccessSvc>     m_rdbAccess;
  std::unique_ptr<InDetMaterialManager> m_materialManager;
  mutable Athena::MsgStreamMember  m_msg ATLAS_THREAD_SAFE;

};

#endif 
