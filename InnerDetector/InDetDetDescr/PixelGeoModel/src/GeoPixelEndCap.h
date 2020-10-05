/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELENDCAP_H
#define GEOPIXELENDCAP_H

#include "GeoVPixelFactory.h"
class GeoPixelServices;

class GeoPixelEndCap : public GeoVPixelFactory {
 public:
  GeoPixelEndCap(InDetDD::PixelDetectorManager* ddmgr,
                 PixelGeometryManager* mgr,
                 GeoPixelServices * pixServices);
  virtual GeoVPhysVol* Build() override;
 private:
  GeoPixelServices * m_pixServices;
};

#endif
