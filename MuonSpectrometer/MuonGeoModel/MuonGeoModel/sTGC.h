/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef sTGC_H
#define sTGC_H

#include "MuonGeoModel/DetectorElement.h"

#include <vector>

class GeoFullPhysVol;

namespace MuonGM {

class Cutout;
class Component;
class sTGCComponent;

class sTGC: public DetectorElement {

public:
  double width;
  double length;
  double thickness;
  double longWidth;   // for trapezoidal layers
  double yCutout;     // for Hexagonal layer
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
