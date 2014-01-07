/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSERVMATGEOMODEL_TRT_SERVMATFACTORYFS_H
#define INDETSERVMATGEOMODEL_TRT_SERVMATFACTORYFS_H

#include "AthenaKernel/MsgStreamMember.h"
#include "GaudiKernel/ServiceHandle.h"

class StoreGateSvc;
class GeoPhysVol;
class InDetMaterialManager;
class IRDBAccessSvc;

#include <string>

// TRT service material factory fro Frozen Showers
class TRT_ServMatFactoryFS   {

 public:
  
  // Constructor:
  TRT_ServMatFactoryFS(StoreGateSvc  *pDetStore,
		       ServiceHandle<IRDBAccessSvc> pRDBAccess);   
  // Destructor:
  ~TRT_ServMatFactoryFS();
  
  // Creation of geometry:
  void create(GeoPhysVol *motherP, GeoPhysVol *motherM);

  MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }

 private:

  // Illegal operations:
  const TRT_ServMatFactoryFS & operator=(const TRT_ServMatFactoryFS &right);
  TRT_ServMatFactoryFS(const TRT_ServMatFactoryFS &right);

  // private data
  StoreGateSvc                    *m_detStore;
  ServiceHandle<IRDBAccessSvc>     m_rdbAccess;
  InDetMaterialManager            *m_materialManager;
  mutable Athena::MsgStreamMember  m_msg;
};

#endif 
