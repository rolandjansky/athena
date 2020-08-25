/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELDISKSLHC_H
#define GEOPIXELDISKSLHC_H

#include "GeoVPixelFactory.h"

class ATLAS_NOT_THREAD_SAFE GeoPixelDiskSLHC : public GeoVPixelFactory { // Thread unsafe GeoVPixelFactory class is used.
 public:
  GeoPixelDiskSLHC();
  virtual GeoVPhysVol* Build();
};

#endif
