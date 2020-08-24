/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELECCABLE_H
#define GEOPIXELECCABLE_H

#include "GeoVPixelFactory.h"
class GeoLogVol;

class ATLAS_NOT_THREAD_SAFE GeoPixelECCable : public GeoVPixelFactory { // Thread unsafe GeoVPixelFactory class is used.
 public:
  GeoPixelECCable();
  virtual GeoVPhysVol* Build();
};

#endif
