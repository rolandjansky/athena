/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELLADDERSERVICES_H
#define GEOPIXELLADDERSERVICES_H

#include "GeoVPixelFactory.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelKernel/GeoDefinitions.h"

class GeoLogVol;
class GeoPhysVol;

class GeoPixelLadderServices : public GeoVPixelFactory {
 public:
  GeoPixelLadderServices(InDetDD::PixelDetectorManager* ddmgr,
                         PixelGeometryManager* mgr ,int ladderType);
  virtual ~GeoPixelLadderServices();
  virtual GeoVPhysVol* Build() override;
  const GeoTrf::Vector3D & envelopeCornerA1() {return  m_envelopeCornerA1;}
  const GeoTrf::Vector3D & envelopeCornerA2() {return  m_envelopeCornerA2;}
  const GeoTrf::Vector3D & envelopeCornerC1() {return  m_envelopeCornerC1;}
  const GeoTrf::Vector3D & envelopeCornerC2() {return  m_envelopeCornerC2;}
  double referenceX() {return m_xOffset;}
  double referenceY() {return m_yOffset;}

  GeoVPhysVol* BuildAlTube();
  GeoVPhysVol* BuildOmega();
  void BuildGlue(GeoPhysVol * parent);
  void BuildPigtailAndConnector(GeoPhysVol * parent);

 private:
  const GeoLogVol* m_ladderServicesLV;
  int m_ladderType;

  GeoTrf::Vector3D m_envelopeCornerA1;
  GeoTrf::Vector3D m_envelopeCornerA2;
  GeoTrf::Vector3D m_envelopeCornerC1;
  GeoTrf::Vector3D m_envelopeCornerC2;

  double m_xOffset;
  double m_yOffset;

};

#endif
