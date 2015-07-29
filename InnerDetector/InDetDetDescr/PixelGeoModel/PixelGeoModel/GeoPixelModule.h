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
  double ThicknessN_noSvc();
  double ThicknessP();
  double Width();
  double Length();
  double ModuleServiceThickness() const { return m_moduleSvcThickness; }
  double ModuleServiceWidth() const { return m_moduleSvcWidth; }
  Identifier getID();

 private:

  const GeoShape*  addShape(const GeoShape * lastShape, const GeoShape * nextShape, const HepGeom::Transform3D & trans);

  const GeoLogVol* theModule;
  Identifier _id;
  GeoPixelSiCrystal& m_theSensor;
  bool m_isModule3D;

  double m_moduleSvcThickness;
  double m_moduleSvcWidth;
  int nbModuleSvc;

};

#endif
