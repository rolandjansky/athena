/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef sTGC_H
#define sTGC_H

#include "MuonGeoModel/DetectorElement.h"
#include "MuonGeoModel/sTGCComponent.h"

class GeoFullPhysVol;

namespace MuonGM {

class Cutout;

class sTGC: public DetectorElement {

public:
  double width;
  double length;
  double thickness;
  double longWidth;   // for trapezoidal layers
  double yCutout;     // for Hexagonal layer
  double yCutoutCathode;     // for Hexagonal layer
  int index;
	
  sTGC(Component* s);
  GeoFullPhysVol* build(int minimalgeo);
  GeoFullPhysVol* build(int minimalgeo, int cutoutson, std::vector<Cutout*>);
  void print();

private:
  sTGCComponent* m_component;
};

} // namespace MuonGM


#endif
