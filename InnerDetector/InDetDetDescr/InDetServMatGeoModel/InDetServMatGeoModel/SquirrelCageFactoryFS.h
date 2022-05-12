/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSERVMATGEOMODEL_SQUIRRELCAGEFACTORYFS_H
#define INDETSERVMATGEOMODEL_SQUIRRELCAGEFACTORYFS_H

#include "AthenaBaseComps/AthMessaging.h"
#include "GaudiKernel/ServiceHandle.h"

class StoreGateSvc;
class GeoPhysVol;
class IRDBAccessSvc;

class SquirrelCageFactoryFS : public AthMessaging {

 public:
  
  // Constructor:
  SquirrelCageFactoryFS(StoreGateSvc *pDetStore,
			ServiceHandle<IRDBAccessSvc> pRDBAccess);
  
  // Destructor:
  ~SquirrelCageFactoryFS();
  
  // Creation of geometry:
  void create(GeoPhysVol *motherP, GeoPhysVol *motherM);

 private:  
  
  // Illegal operations:
  const SquirrelCageFactoryFS & operator=(const SquirrelCageFactoryFS &right);
  SquirrelCageFactoryFS(const SquirrelCageFactoryFS &right);

  // private data
  StoreGateSvc                   *m_detStore;
  ServiceHandle<IRDBAccessSvc>    m_rdbAccess;
};

#endif 
