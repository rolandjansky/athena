/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELDISK_H
#define GEOPIXELDISK_H

#include "GeoVPixelFactory.h"
class GeoLogVol;

class ATLAS_NOT_THREAD_SAFE GeoPixelDisk : public GeoVPixelFactory { // Thread unsafe GeoVPixelFactory class is used.
 public:
  GeoPixelDisk();
  virtual GeoVPhysVol* Build();
  double Thickness();
  double RMax();
  double RMin();
};

#endif
