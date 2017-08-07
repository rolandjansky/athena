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

class GeoPixelStaveSupportInclRef : public PixelGeoBuilder {

public:  
  GeoPixelStaveSupportInclRef(const PixelGeoBuilderBasics* basics, int iLayer, const GeoDetModulePixel& barrelModule,
			      double barrelTilt=0., double endcapTilt=0.,
			      double planarGap=0., double endcapRmin=0., double endcapRmax=0., double zEndOfNBarrelModulePos=-1.);
  virtual GeoVPhysVol* Build();
  void preBuild();
			 
  virtual GeoVPhysVol* getPhysVol() const {return m_physVol;}
  virtual const HepGeom::Transform3D & transform() const {return m_transform;}
  virtual double thicknessP() const {return m_thicknessP;}
  virtual double thicknessP_barrel() const {return m_thicknessP_barrel;}
  virtual double thicknessP_endcap() const {return m_thicknessP_endcap;}
  virtual double thicknessN() const {return m_thicknessN;}
  virtual double thicknessN_endcap() const {return m_thicknessN_endcap;}
  virtual double width() const {return m_width; }
  virtual double length() const {return m_length; }
  virtual double thickness() const { return m_thicknessP+m_thicknessN; }
  std::string getSvcRoutingPos() const { return m_svcRouting; }

private:

  const GeoShape* addShape(const GeoShape * lastShape, const GeoShape * nextShape, const HepGeom::Transform3D & trans);
  GeoPhysVol* m_physVol;
  int m_layer;
  const GeoDetModulePixel& m_barrelModule;
  double m_barrelTilt;
  double m_endcapTilt;
  double m_planarGap;
  double m_endcapMinRadialPos;
  double m_endcapMaxRadialPos;
  double m_barrelZMax;
  std::string m_svcRouting; 

  HepGeom::Transform3D m_transform;
  double m_thicknessP;
  double m_thicknessP_endcap;
  double m_thicknessP_barrel;
  double m_thicknessN;
  double m_thicknessN_endcap;
  double m_width;
  double m_length;

};

#endif

