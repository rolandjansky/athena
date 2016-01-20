/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELLOISTAVESUPPORT_H
#define GEOPIXELLOISTAVESUPPORT_H

#include "PixelGeoModel/PixelGeoBuilder.h"
#include "PixelGeoModelModule/GeoDetModulePixel.h"

#include "CLHEP/Geometry/Transform3D.h"
#include "GeoModelKernel/GeoPhysVol.h"

#include "CLHEP/Geometry/Transform3D.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h"
#include "GeoModelKernel/GeoTransform.h"

class GeoPixelStaveSupportExtRef : public PixelGeoBuilder {

public:  
  GeoPixelStaveSupportExtRef(const PixelGeoBuilderBasics* basics, int iLayer, const GeoDetModulePixel& barrelModule);
  virtual GeoVPhysVol* Build();
  void preBuild();
			 
  virtual GeoVPhysVol* getPhysVol() const {return m_physVol;}
  virtual const HepGeom::Transform3D & transform() const {return m_transform;}
  virtual double thicknessP() const {return m_thicknessP;}
  virtual double thicknessN() const {return m_thicknessN;}
  virtual double width() const {return m_width; }
  virtual double length() const {return m_length; }

private:
  GeoPhysVol* m_physVol;
  int m_layer;
  const GeoDetModulePixel& m_barrelModule;

  HepGeom::Transform3D m_transform;
  double m_thicknessP;
  double m_thicknessN;
  double m_width;
  double m_length;

};

#endif

