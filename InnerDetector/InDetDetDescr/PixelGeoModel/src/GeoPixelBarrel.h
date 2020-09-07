/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELBARREL_H
#define GEOPIXELBARREL_H

#include "GeoVPixelFactory.h"
class GeoPixelServices;

class GeoPixelBarrel : public GeoVPixelFactory {
 public:
  GeoPixelBarrel(InDetDD::PixelDetectorManager* ddmgr,
                 PixelGeometryManager* mgr,
                 GeoPixelServices * pixServices);
  virtual GeoVPhysVol* Build() override;
 private:
  GeoPixelServices * m_pixServices;
};

#endif
