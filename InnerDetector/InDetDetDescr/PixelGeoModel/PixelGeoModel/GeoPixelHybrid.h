/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELHYBRID_H
#define GEOPIXELHYBRID_H

#include "PixelGeoModel/GeoVPixelFactory.h"
class GeoLogVol;

class GeoPixelHybrid : public GeoVPixelFactory {
 public:
    GeoPixelHybrid(bool isModule3D): m_isModule3D(isModule3D) {};
    virtual GeoVPhysVol* Build();

 private:
  bool m_isModule3D;
};

#endif
