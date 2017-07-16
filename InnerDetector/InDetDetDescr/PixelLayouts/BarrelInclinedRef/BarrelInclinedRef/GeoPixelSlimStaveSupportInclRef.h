/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELSLIMSTAVESUPPORTINCLREF_H
#define GEOPIXELSLIMSTAVESUPPORTINCLREF_H

#include "PixelGeoModel/PixelGeoBuilder.h"
#include "PixelGeoModelModule/GeoDetModulePixel.h"

#include "CLHEP/Geometry/Transform3D.h"
#include "GeoModelKernel/GeoPhysVol.h"

#include "CLHEP/Geometry/Transform3D.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h"
#include "GeoModelKernel/GeoTransform.h"

class GeoPixelSlimStaveSupportInclRef : public PixelGeoBuilder {

public:  
  // Inner = longeron half pointing inwards,  i.e. at lower radius than layer
  // Outer = longeron half pointing outwards, i.e. at higher radius than layer
  enum halfStaveType{INNER, OUTER};
  GeoPixelSlimStaveSupportInclRef(const PixelGeoBuilderBasics* basics, int innerLayer, double gapPlanar, halfStaveType staveType, int staveShapeIndex);
  virtual GeoVPhysVol* Build();
  void preBuild();
			 
  virtual GeoVPhysVol* getPhysVol()  const {return m_physVol;}
 
  std::string getSvcRoutingPos()     const { return m_svcRouting; }
  virtual const HepGeom::Transform3D & transform() const {return m_transform;}

private:
  GeoPhysVol* m_physVol;
  int m_innerLayer;
  double m_planarGap;
  halfStaveType m_staveType;
  int m_staveShapeIndex;
  std::string m_svcRouting; 

  HepGeom::Transform3D m_transform;
};

#endif

