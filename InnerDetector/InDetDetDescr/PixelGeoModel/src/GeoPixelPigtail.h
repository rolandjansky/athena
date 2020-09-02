/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELPIGTAIL_H
#define GEOPIXELPIGTAIL_H

#include "GeoVPixelFactory.h"
class GeoLogVol;

class ATLAS_NOT_THREAD_SAFE GeoPixelPigtail : public GeoVPixelFactory { // Thread unsafe GeoVPixelFactory class is used.
 public:
  GeoPixelPigtail();
  virtual GeoVPhysVol* Build();

 private:
  
};

#endif
