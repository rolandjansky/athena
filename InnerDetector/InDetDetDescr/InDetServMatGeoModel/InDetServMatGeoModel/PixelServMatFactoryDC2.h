/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSERVMATGEOMODEL_PIXELSERVMATFACTORYDC2_H
#define INDETSERVMATGEOMODEL_PIXELSERVMATFACTORYDC2_H

#include "AthenaKernel/MsgStreamMember.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/ServiceHandle.h"

class StoreGateSvc;
class GeoPhysVol;
class IRDBAccessSvc;

class PixelServMatFactoryDC2   {

 public:
  
  // Constructor:
  PixelServMatFactoryDC2(StoreGateSvc *pDetStore,
			 ServiceHandle<IRDBAccessSvc> pRDBAccess);
  
  // Destructor:
  ~PixelServMatFactoryDC2();
  
  // Creation of geometry:
  void create(GeoPhysVol *mother);

  MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }

 private:  
  
  // Illegal operations:
  const PixelServMatFactoryDC2 & operator=(const PixelServMatFactoryDC2 &right);
  PixelServMatFactoryDC2(const PixelServMatFactoryDC2 &right);

  // private data
  StoreGateSvc                   *m_detStore;
  ServiceHandle<IRDBAccessSvc>    m_rdbAccess;
  mutable Athena::MsgStreamMember m_msg ATLAS_THREAD_SAFE;
};

#endif //  INDETSERVMATGEOMODEL_PIXELSERVMATFACTORYDC2_H
