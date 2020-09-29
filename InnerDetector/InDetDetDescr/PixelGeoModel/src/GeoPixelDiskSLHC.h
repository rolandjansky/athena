/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELDISKSLHC_H
#define GEOPIXELDISKSLHC_H

#include "GeoVPixelFactory.h"

class GeoPixelDiskSLHC : public GeoVPixelFactory {
 public:
  GeoPixelDiskSLHC(InDetDD::PixelDetectorManager* ddmgr,
                   PixelGeometryManager* mgr);
  virtual GeoVPhysVol* Build() override;
};

#endif
