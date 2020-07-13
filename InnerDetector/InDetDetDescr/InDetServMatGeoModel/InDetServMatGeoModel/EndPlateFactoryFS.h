/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSERVMATGEOMODEL_ENDPLATEFACTORYFS_H
#define INDETSERVMATGEOMODEL_ENDPLATEFACTORYFS_H

#include "AthenaKernel/MsgStreamMember.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/ServiceHandle.h"

class StoreGateSvc;
class GeoPhysVol;
class IRDBAccessSvc;


// End plate factory for Frozen Showers (FS)
class EndPlateFactoryFS {

 public:
  
  // Constructor:
  EndPlateFactoryFS(StoreGateSvc *pDetStore,
		    ServiceHandle<IRDBAccessSvc> pRDBAccess);
  
  // Destructor:
  ~EndPlateFactoryFS();
  
  // Creation of geometry:
  void create(GeoPhysVol *motherP, GeoPhysVol *motherM);

  MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }

 private:  
  
  // Illegal operations:
  const EndPlateFactoryFS & operator=(const EndPlateFactoryFS &right);
  EndPlateFactoryFS(const EndPlateFactoryFS &right);

  // private data
  StoreGateSvc                   *m_detStore;
  ServiceHandle<IRDBAccessSvc>    m_rdbAccess;
  mutable Athena::MsgStreamMember m_msg ATLAS_THREAD_SAFE;
};

#endif 
