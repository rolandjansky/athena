/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELDISKSLHC_H
#define GEOPIXELDISKSLHC_H

#include "GeoVPixelFactory.h"

class GeoPixelDiskSLHC : public GeoVPixelFactory {
 public:
  GeoPixelDiskSLHC();
  virtual GeoVPhysVol* Build();
};

#endif
