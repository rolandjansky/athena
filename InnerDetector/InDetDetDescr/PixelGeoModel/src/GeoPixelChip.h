/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELCHIP_H
#define GEOPIXELCHIP_H

#include "GeoVPixelFactory.h"
class GeoLogVol;

class ATLAS_NOT_THREAD_SAFE GeoPixelChip : public GeoVPixelFactory { // Thread unsafe GeoVPixelFactory class is used.
 public:
  GeoPixelChip(bool isModule3D): m_isModule3D(isModule3D) {};
  virtual GeoVPhysVol* Build();

 private:
  bool m_isModule3D;
};

#endif
