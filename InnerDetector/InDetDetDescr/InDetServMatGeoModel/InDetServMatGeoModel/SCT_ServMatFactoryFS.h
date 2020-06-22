/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSERVMATGEOMODEL_SCT_SERVMATFACTORYFS_H
#define INDETSERVMATGEOMODEL_SCT_SERVMATFACTORYFS_H

#include "AthenaKernel/MsgStreamMember.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/ServiceHandle.h"

class StoreGateSvc;
class GeoPhysVol;
class InDetMaterialManager;
class IRDBAccessSvc;

#include <memory>
#include <string>

// SCT service material factory for Frozen Showers
class SCT_ServMatFactoryFS   {

 public:
  
  // Constructor:
  SCT_ServMatFactoryFS(StoreGateSvc  *pDetStore,
		       ServiceHandle<IRDBAccessSvc>& pRDBAccess);  
  // Destructor:
  ~SCT_ServMatFactoryFS();
  
  // Creation of geometry:
  void create(GeoPhysVol *motherP, GeoPhysVol *motherM);

  MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }

 private:

 // Illegal operations:
  const SCT_ServMatFactoryFS & operator=(const SCT_ServMatFactoryFS &right);
  SCT_ServMatFactoryFS(const SCT_ServMatFactoryFS &right);

  // private data
  StoreGateSvc                    *m_detStore;
  ServiceHandle<IRDBAccessSvc>     m_rdbAccess;
  std::unique_ptr<InDetMaterialManager> m_materialManager;
  mutable Athena::MsgStreamMember  m_msg ATLAS_THREAD_SAFE;

};

#endif
