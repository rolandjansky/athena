/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELFLUID_H
#define GEOPIXELFLUID_H

#include "PixelGeoModel/GeoVPixelFactory.h"
#include <sstream>

class GeoLogVol;

class GeoPixelFluid : public GeoVPixelFactory {
public:
  GeoPixelFluid(int type);
  virtual GeoVPhysVol* Build();
  double posX() {return m_posX;}
  double posY() {return m_posY;}
  double posZ() {return m_posZ;}

private:
  int m_index;
  double m_posX;
  double m_posY;
  double m_posZ;
};

#endif

