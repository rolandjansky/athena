/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELMODULE_H
#define GEOPIXELMODULE_H
#include "Identifier/Identifier.h"
#include "GeoVPixelFactory.h"
class GeoLogVol;
class GeoPixelSiCrystal;

class GeoPixelModule : public GeoVPixelFactory {

 public:
  GeoPixelModule(InDetDD::PixelDetectorManager* ddmgr,
                 PixelGeometryManager* mgr,
                 GeoPixelSiCrystal &theSensor);
  GeoPixelModule (const GeoPixelModule&) = delete;
  GeoPixelModule& operator= (const GeoPixelModule&) = delete;
  virtual ~GeoPixelModule();
  virtual GeoVPhysVol* Build() override;
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

  const GeoShape*  addShape(const GeoShape * lastShape, const GeoShape * nextShape, const GeoTrf::Transform3D & trans);

  const GeoLogVol* m_theModule;
  Identifier m_id;
  GeoPixelSiCrystal& m_theSensor;
  bool m_isModule3D;

  double m_moduleSvcThickness;
  double m_moduleSvcWidth;
  int m_nbModuleSvc;

};

#endif
