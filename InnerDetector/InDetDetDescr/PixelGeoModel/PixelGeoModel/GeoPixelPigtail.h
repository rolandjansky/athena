/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELPIGTAIL_H
#define GEOPIXELPIGTAIL_H

#include "PixelGeoModel/GeoVPixelFactory.h"
class GeoLogVol;

class GeoPixelPigtail : public GeoVPixelFactory {
 public:
  GeoPixelPigtail();
  virtual GeoVPhysVol* Build();

 private:
  
};

#endif
