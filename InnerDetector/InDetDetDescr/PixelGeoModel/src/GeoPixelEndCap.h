/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELENDCAP_H
#define GEOPIXELENDCAP_H

#include "GeoVPixelFactory.h"
class GeoPixelServices;

class ATLAS_NOT_THREAD_SAFE GeoPixelEndCap : public GeoVPixelFactory { // Thread unsafe GeoVPixelFactory class is used.
 public:
  GeoPixelEndCap(const GeoPixelServices * pixServices);
  virtual GeoVPhysVol* Build();
 private:
  const GeoPixelServices * m_pixServices;
};

#endif
