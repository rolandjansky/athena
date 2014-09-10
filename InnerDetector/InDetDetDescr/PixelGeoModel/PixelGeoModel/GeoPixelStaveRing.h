/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELSTAVERING_H
#define GEOPIXELSTAVERING_H

#include "PixelGeoModel/GeoPixelStaveSupport.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "GeoModelKernel/GeoTransform.h"

class GeoPixelStaveRing :  public GeoVPixelFactory {

public:  
  GeoPixelStaveRing();
  virtual GeoVPhysVol* Build();

  GeoVPhysVol* SetParametersAndBuild(std::string,std::string);
  double GetPositionAlongZAxis() const { return m_zPosition; }

  double GetInnerRadius() const { return m_innerRadius; }
  double GetOuterRadius() const { return m_outerRadius; }

private:
  GeoVPhysVol* m_physVol;
  double m_zPosition;
  double m_innerRadius, m_outerRadius;
  std::string m_ringPosition;
  std::string m_ringName;

};

#endif

