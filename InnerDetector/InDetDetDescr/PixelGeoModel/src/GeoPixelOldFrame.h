/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELOLDFRAME_H
#define GEOPIXELOLDFRAME_H

#include "GeoVPixelFactory.h"
class GeoLogVol;
class GeoFullPhysVol;
class GeoVPhysVol;
class PixelLegacyManager;

class ATLAS_NOT_THREAD_SAFE GeoPixelOldFrame : public GeoVPixelFactory { // Thread unsafe GeoVPixelFactory class is used.
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
