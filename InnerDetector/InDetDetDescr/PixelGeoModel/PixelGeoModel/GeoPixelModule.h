/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELMODULE_H
#define GEOPIXELMODULE_H
#include "Identifier/Identifier.h"
#include "PixelGeoModel/GeoVPixelFactory.h"
class GeoLogVol;
class GeoPixelSiCrystal;

class GeoPixelModule : public GeoVPixelFactory {
 public:
  GeoPixelModule(GeoPixelSiCrystal &theSensor);
  virtual GeoVPhysVol* Build();
  double Thickness();
  double ThicknessN();
  double ThicknessP();
  double Width();
  double Length();
  Identifier getID();
 private:
  const GeoLogVol* theModule;
  Identifier _id;
  GeoPixelSiCrystal& m_theSensor;
  bool m_isModule3D;
};

#endif
