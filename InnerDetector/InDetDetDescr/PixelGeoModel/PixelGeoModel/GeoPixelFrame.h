/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELFRAME_H
#define GEOPIXELFRAME_H

#include "PixelGeoModel/GeoVPixelFactory.h"

class GeoPixelFrame : public GeoVPixelFactory {

public:  
  GeoPixelFrame();
  void BuildAndPlace(GeoFullPhysVol * parent, int section);

private:
  virtual GeoVPhysVol* Build() {return 0;} // unused - but satisfy interface
  bool same(double v1, double v2);
};

#endif

