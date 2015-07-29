/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELPIGTAIL_H
#define GEOPIXELPIGTAIL_H

#include "PixelGeoModel/GeoVPixelFactory.h"
class GeoLogVol;

class GeoPixelPigtail : public GeoVPixelFactory {
 public:
  GeoPixelPigtail();
  virtual GeoVPhysVol* Build();
  double bendCenterX() {return m_posBendX;}
  double bendCenterY() {return m_posBendY;}

 private:
  const GeoLogVol* thePigtail;
  double m_rmin;
  double m_rmax;
  double m_phi;
  double m_dphi;
  double m_width;
  double m_posBendX;
  double m_posBendY;
  double m_posZ;
};

#endif
