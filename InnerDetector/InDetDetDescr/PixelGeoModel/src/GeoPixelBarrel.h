/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELBARREL_H
#define GEOPIXELBARREL_H

#include "GeoVPixelFactory.h"
class GeoPixelServices;

class ATLAS_NOT_THREAD_SAFE GeoPixelBarrel : public GeoVPixelFactory { // Thread unsafe GeoVPixelFactory class is used.
 public:
  GeoPixelBarrel(const GeoPixelServices * pixServices);
  virtual GeoVPhysVol* Build();
 private:
  const GeoPixelServices * m_pixServices;
};

#endif
