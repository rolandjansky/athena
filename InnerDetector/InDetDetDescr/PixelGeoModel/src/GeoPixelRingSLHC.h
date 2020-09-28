/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELRINGSLHC_H
#define GEOPIXELRINGSLHC_H

#include "GeoVPixelFactory.h"
class GeoPixelSiCrystal;
class GeoLogVol;

class GeoPixelRingSLHC : public GeoVPixelFactory {
 public:
  GeoPixelRingSLHC(InDetDD::PixelDetectorManager* ddmgr,
                   PixelGeometryManager* mgr, GeoPixelSiCrystal& sensor);
  virtual GeoVPhysVol* Build() override;

 private:  
  GeoPixelSiCrystal& m_sensor;
};

#endif
