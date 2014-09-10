/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELENVELOPE_H
#define GEOPIXELENVELOPE_H

#include "PixelGeoModel/GeoVPixelFactory.h"

class GeoPixelEnvelope : public GeoVPixelFactory {
 public:
  virtual GeoVPhysVol* Build();
};

#endif
