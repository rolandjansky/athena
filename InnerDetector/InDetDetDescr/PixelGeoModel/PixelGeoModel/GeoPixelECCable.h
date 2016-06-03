/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELECCABLE_H
#define GEOPIXELECCABLE_H

#include "PixelGeoModel/GeoVPixelFactory.h"
class GeoLogVol;

class GeoPixelECCable : public GeoVPixelFactory {
 public:
  GeoPixelECCable();
  virtual GeoVPhysVol* Build();
};

#endif
