/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELBARREL_H
#define GEOPIXELBARREL_H

#include "PixelGeoModel/GeoVPixelFactory.h"
class GeoPixelServices;

class GeoPixelBarrel : public GeoVPixelFactory {
 public:
  GeoPixelBarrel(const GeoPixelServices * pixServices);
  virtual GeoVPhysVol* Build();
 private:
  const GeoPixelServices * m_pixServices;
};

#endif
