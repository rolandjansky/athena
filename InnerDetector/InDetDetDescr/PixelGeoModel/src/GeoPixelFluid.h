/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELFLUID_H
#define GEOPIXELFLUID_H

#include "GeoVPixelFactory.h"
#include <sstream>

class GeoLogVol;

class GeoPixelFluid : public GeoVPixelFactory {
public:
  GeoPixelFluid(InDetDD::PixelDetectorManager* ddmgr,
                PixelGeometryManager* mgr, int type);
  virtual GeoVPhysVol* Build() override;
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

