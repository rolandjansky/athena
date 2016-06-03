/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELLADDERSERVICES_H
#define GEOPIXELLADDERSERVICES_H

#include "PixelGeoModel/GeoVPixelFactory.h"
#include "CLHEP/Geometry/Point3D.h"
#include <sstream>

class GeoLogVol;
class GeoPhysVol;

class GeoPixelLadderServices : public GeoVPixelFactory {
 public:
  GeoPixelLadderServices(int ladderType);
  virtual GeoVPhysVol* Build();
  //double Thickness();
  const HepGeom::Point3D<double> & envelopeCornerA1() {return  m_envelopeCornerA1;}
  const HepGeom::Point3D<double> & envelopeCornerA2() {return  m_envelopeCornerA2;}
  const HepGeom::Point3D<double> & envelopeCornerC1() {return  m_envelopeCornerC1;}
  const HepGeom::Point3D<double> & envelopeCornerC2() {return  m_envelopeCornerC2;}
  double referenceX() {return m_xOffset;}
  double referenceY() {return m_yOffset;}

  GeoVPhysVol* BuildAlTube();
  GeoVPhysVol* BuildOmega();
  void BuildGlue(GeoPhysVol * parent);
  void BuildPigtailAndConnector(GeoPhysVol * parent);

 private:
  const GeoLogVol* m_ladderServicesLV;
  int m_ladderType;

  HepGeom::Point3D<double> m_envelopeCornerA1;
  HepGeom::Point3D<double> m_envelopeCornerA2;
  HepGeom::Point3D<double> m_envelopeCornerC1;
  HepGeom::Point3D<double> m_envelopeCornerC2;

  double m_xOffset;
  double m_yOffset;

};

#endif
