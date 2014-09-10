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
  int NCylinders(){return _rmin.size();}
  void SetCylinder(int _n) {_nframe = _n;}
  double ZPos() {return _zpos[_nframe];}
 private:
  std::vector<double> _rmin,_rmax,_halflength,_zpos;
  std::vector<int> _typeNum;
  int _nframe;

};

#endif
