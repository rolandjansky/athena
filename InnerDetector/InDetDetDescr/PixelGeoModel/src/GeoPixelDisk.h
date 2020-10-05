/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELDISK_H
#define GEOPIXELDISK_H

#include "GeoVPixelFactory.h"
class GeoLogVol;

class GeoPixelDisk : public GeoVPixelFactory {
 public:
  GeoPixelDisk(InDetDD::PixelDetectorManager* ddmgr, PixelGeometryManager* mgr);
  virtual GeoVPhysVol* Build() override;
  double Thickness();
  double RMax();
  double RMin();
};

#endif
