/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELDISK_H
#define GEOPIXELDISK_H

#include "PixelGeoModel/GeoVPixelFactory.h"
class GeoLogVol;

class GeoPixelDisk : public GeoVPixelFactory {
 public:
  GeoPixelDisk();
  virtual GeoVPhysVol* Build();
  double Thickness();
  double RMax();
  double RMin();
};

#endif
