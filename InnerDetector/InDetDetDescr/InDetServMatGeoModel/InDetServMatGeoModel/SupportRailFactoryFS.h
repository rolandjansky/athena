/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSERVMATGEOMODEL_SUPPORTRAILFACTORYFS_H
#define INDETSERVMATGEOMODEL_SUPPORTRAILFACTORYFS_H

#include "AthenaBaseComps/AthMessaging.h"
#include "GaudiKernel/ServiceHandle.h"

class StoreGateSvc;
class GeoPhysVol;
class IRDBAccessSvc;

class SupportRailFactoryFS : public AthMessaging {

 public:
  
  // Constructor:
  SupportRailFactoryFS(StoreGateSvc *pDetStore,
		       ServiceHandle<IRDBAccessSvc> pRDBAccess);
  
  // Destructor:
  ~SupportRailFactoryFS();
  
  // Creation of geometry:
  void create(GeoPhysVol *motherP,GeoPhysVol *motherM);

 private:  
  
  // Illegal operations:
  const SupportRailFactoryFS & operator=(const SupportRailFactoryFS &right);
  SupportRailFactoryFS(const SupportRailFactoryFS &right);

  // private data
  StoreGateSvc                    *m_detStore;
  ServiceHandle<IRDBAccessSvc>     m_rdbAccess;
};

#endif 
