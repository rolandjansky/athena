/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSERVMATGEOMODEL_PIXELSERVMATFACTORYDC2_H
#define INDETSERVMATGEOMODEL_PIXELSERVMATFACTORYDC2_H

#include "AthenaBaseComps/AthMessaging.h"
#include "GaudiKernel/ServiceHandle.h"

class StoreGateSvc;
class GeoPhysVol;
class IRDBAccessSvc;

class PixelServMatFactoryDC2 : public AthMessaging  {

 public:
  
  // Constructor:
  PixelServMatFactoryDC2(StoreGateSvc *pDetStore,
			 ServiceHandle<IRDBAccessSvc> pRDBAccess);
  
  // Destructor:
  ~PixelServMatFactoryDC2();
  
  // Creation of geometry:
  void create(GeoPhysVol *mother);

 private:  
  
  // Illegal operations:
  const PixelServMatFactoryDC2 & operator=(const PixelServMatFactoryDC2 &right);
  PixelServMatFactoryDC2(const PixelServMatFactoryDC2 &right);

  // private data
  StoreGateSvc                   *m_detStore;
  ServiceHandle<IRDBAccessSvc>    m_rdbAccess;
};

#endif //  INDETSERVMATGEOMODEL_PIXELSERVMATFACTORYDC2_H
