/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSERVMATGEOMODEL_SUPPORTRAILFACTORYFS_H
#define INDETSERVMATGEOMODEL_SUPPORTRAILFACTORYFS_H

#include "AthenaKernel/MsgStreamMember.h"
#include "GaudiKernel/ServiceHandle.h"

class StoreGateSvc;
class GeoPhysVol;
class IRDBAccessSvc;

class SupportRailFactoryFS {

 public:
  
  // Constructor:
  SupportRailFactoryFS(StoreGateSvc *pDetStore,
		       ServiceHandle<IRDBAccessSvc> pRDBAccess);
  
  // Destructor:
  ~SupportRailFactoryFS();
  
  // Creation of geometry:
  void create(GeoPhysVol *motherP,GeoPhysVol *motherM);

  MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }

 private:  
  
  // Illegal operations:
  const SupportRailFactoryFS & operator=(const SupportRailFactoryFS &right);
  SupportRailFactoryFS(const SupportRailFactoryFS &right);

  // private data
  StoreGateSvc                    *m_detStore;
  ServiceHandle<IRDBAccessSvc>     m_rdbAccess;
  mutable Athena::MsgStreamMember  m_msg;

};

#endif 
