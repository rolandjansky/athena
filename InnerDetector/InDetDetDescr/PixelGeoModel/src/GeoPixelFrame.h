/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELFRAME_H
#define GEOPIXELFRAME_H

#include "GeoVPixelFactory.h"

class ATLAS_NOT_THREAD_SAFE GeoPixelFrame : public GeoVPixelFactory { // Thread unsafe GeoVPixelFactory class is used.

public:  
  GeoPixelFrame();
  void BuildAndPlace(GeoFullPhysVol * parent, int section);

private:
  virtual GeoVPhysVol* Build() {return 0;} // unused - but satisfy interface
  bool same(double v1, double v2);
};

#endif

