/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDC_DETFACTORY_H
#define ZDC_DETFACTORY_H

#include "GeoModelKernel/GeoVDetectorFactory.h"
#include "ZDC_DetManager.h"
#include "AthenaBaseComps/AthMessaging.h"

class StoreGateSvc;

class ZDC_DetFactory : public GeoVDetectorFactory, public AthMessaging
{
 
 public:
  
  ZDC_DetFactory(StoreGateSvc*);
  ~ZDC_DetFactory();
  
  virtual void create(GeoPhysVol* world);
  virtual const ZDC_DetManager* getDetectorManager() const;

 private:
  static int Make_Pixel_ID(int Pid = -1);

  ZDC_DetManager* m_detectorManager;
  StoreGateSvc*   m_detectorStore;
};

#endif

