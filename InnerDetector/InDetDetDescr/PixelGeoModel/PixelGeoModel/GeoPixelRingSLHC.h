/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELRINGSLHC_H
#define GEOPIXELRINGSLHC_H

#include "PixelGeoModel/GeoVPixelFactory.h"
class GeoPixelSiCrystal;
class GeoLogVol;

class GeoPixelRingSLHC : public GeoVPixelFactory {
 public:
  GeoPixelRingSLHC(GeoPixelSiCrystal& sensor);
  virtual GeoVPhysVol* Build();

 private:  
  GeoPixelSiCrystal& m_sensor;
  GeoLogVol* m_ringLog;
};

#endif
