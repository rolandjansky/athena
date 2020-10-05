/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELCHIP_H
#define GEOPIXELCHIP_H

#include "GeoVPixelFactory.h"
class GeoLogVol;

class GeoPixelChip : public GeoVPixelFactory {
 public:
  GeoPixelChip(InDetDD::PixelDetectorManager* ddmgr,
               PixelGeometryManager* mgr,
               bool isModule3D)
    : GeoVPixelFactory (ddmgr, mgr),
      m_isModule3D(isModule3D)
  {};
  virtual GeoVPhysVol* Build() override;

 private:
  bool m_isModule3D;
};

#endif
