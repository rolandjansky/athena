/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELDISKSUPPORTS_H
#define GEOPIXELDISKSUPPORTS_H

#include "PixelGeoModel/GeoVPixelFactory.h"
class GeoLogVol;

class GeoPixelDiskSupports : public GeoVPixelFactory {

 public:
  GeoPixelDiskSupports();
  virtual GeoVPhysVol* Build();

  int NCylinders(){return m_rmin.size();}
  void SetCylinder(int n) {m_nframe = n;}
  double ZPos() {return m_zpos[m_nframe];}

 private:
  std::vector<double> m_rmin,m_rmax,m_halflength,m_zpos;
  std::vector<int> m_typeNum;
  int m_nframe;

};

#endif
