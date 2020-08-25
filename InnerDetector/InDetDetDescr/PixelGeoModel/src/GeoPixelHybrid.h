/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELHYBRID_H
#define GEOPIXELHYBRID_H

#include "GeoVPixelFactory.h"
class GeoLogVol;

class ATLAS_NOT_THREAD_SAFE GeoPixelHybrid : public GeoVPixelFactory { // Thread unsafe GeoVPixelFactory class is used.
 public:
    GeoPixelHybrid(bool isModule3D): m_isModule3D(isModule3D) {};
    virtual GeoVPhysVol* Build();

 private:
  bool m_isModule3D;
};

#endif
