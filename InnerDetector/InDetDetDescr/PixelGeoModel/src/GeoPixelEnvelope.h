/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELENVELOPE_H
#define GEOPIXELENVELOPE_H

#include "GeoVPixelFactory.h"

class ATLAS_NOT_THREAD_SAFE GeoPixelEnvelope : public GeoVPixelFactory { // Thread unsafe GeoVPixelFactory class is used.
 public:
  virtual GeoVPhysVol* Build();
};

#endif
