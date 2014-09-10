/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELOLDFRAME_H
#define GEOPIXELOLDFRAME_H

#include "PixelGeoModel/GeoVPixelFactory.h"
class GeoLogVol;
class GeoFullPhysVol;
class GeoVPhysVol;
class PixelLegacyManager;

class GeoPixelOldFrame : public GeoVPixelFactory {
 public:
  GeoPixelOldFrame();
  virtual GeoVPhysVol* Build();

  void BuildInBarrel(GeoFullPhysVol * parent);
  void BuildOutBarrel(GeoFullPhysVol * parent);

private:  
  GeoVPhysVol* BuildBox();
  GeoVPhysVol* BuildTrap();

  PixelLegacyManager * m_legacyManager;

};

#endif
