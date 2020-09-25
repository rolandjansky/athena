/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELPIGTAIL_H
#define GEOPIXELPIGTAIL_H

#include "GeoVPixelFactory.h"
class GeoLogVol;

class GeoPixelPigtail : public GeoVPixelFactory {
 public:
  GeoPixelPigtail(InDetDD::PixelDetectorManager* ddmgr,
                  PixelGeometryManager* mgr);
  virtual GeoVPhysVol* Build() override;

 private:
  
};

#endif
