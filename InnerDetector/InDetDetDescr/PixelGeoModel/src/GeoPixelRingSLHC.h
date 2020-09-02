/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELRINGSLHC_H
#define GEOPIXELRINGSLHC_H

#include "GeoVPixelFactory.h"
class GeoPixelSiCrystal;
class GeoLogVol;

class ATLAS_NOT_THREAD_SAFE GeoPixelRingSLHC : public GeoVPixelFactory { // Thread unsafe GeoVPixelFactory class is used.
 public:
  GeoPixelRingSLHC(GeoPixelSiCrystal& sensor);
  virtual GeoVPhysVol* Build();

 private:  
  GeoPixelSiCrystal& m_sensor;
};

#endif
