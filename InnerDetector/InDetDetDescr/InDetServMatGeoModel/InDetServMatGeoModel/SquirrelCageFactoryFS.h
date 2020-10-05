/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSERVMATGEOMODEL_SQUIRRELCAGEFACTORYFS_H
#define INDETSERVMATGEOMODEL_SQUIRRELCAGEFACTORYFS_H

#include "AthenaKernel/MsgStreamMember.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/ServiceHandle.h"

class StoreGateSvc;
class GeoPhysVol;
class IRDBAccessSvc;

class SquirrelCageFactoryFS {

 public:
  
  // Constructor:
  SquirrelCageFactoryFS(StoreGateSvc *pDetStore,
			ServiceHandle<IRDBAccessSvc> pRDBAccess);
  
  // Destructor:
  ~SquirrelCageFactoryFS();
  
  // Creation of geometry:
  void create(GeoPhysVol *motherP, GeoPhysVol *motherM);

  MsgStream& msg (MSG::Level lvl) { return m_msg << lvl; }

 private:  
  
  // Illegal operations:
  const SquirrelCageFactoryFS & operator=(const SquirrelCageFactoryFS &right);
  SquirrelCageFactoryFS(const SquirrelCageFactoryFS &right);

  // private data
  StoreGateSvc                   *m_detStore;
  ServiceHandle<IRDBAccessSvc>    m_rdbAccess;
  Athena::MsgStreamMember m_msg;
};

#endif 
