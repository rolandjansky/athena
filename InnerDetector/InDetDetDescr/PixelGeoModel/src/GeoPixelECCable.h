/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELECCABLE_H
#define GEOPIXELECCABLE_H

#include "GeoVPixelFactory.h"
class GeoLogVol;

class GeoPixelECCable : public GeoVPixelFactory {
 public:
  GeoPixelECCable(InDetDD::PixelDetectorManager* ddmgr,
                  PixelGeometryManager* mgr);
  virtual GeoVPhysVol* Build() override;
};

#endif
