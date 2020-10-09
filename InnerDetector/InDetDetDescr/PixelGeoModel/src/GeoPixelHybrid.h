/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELHYBRID_H
#define GEOPIXELHYBRID_H

#include "GeoVPixelFactory.h"
class GeoLogVol;

class GeoPixelHybrid : public GeoVPixelFactory {
 public:
    GeoPixelHybrid(InDetDD::PixelDetectorManager* ddmgr,
                   PixelGeometryManager* mgr,
                   bool isModule3D)
      : GeoVPixelFactory (ddmgr, mgr),
        m_isModule3D(isModule3D) {};
    virtual GeoVPhysVol* Build() override;

 private:
  bool m_isModule3D;
};

#endif
