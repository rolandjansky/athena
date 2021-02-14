/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMSpacer_H
#define MMSpacer_H

#include "MuonGeoModel/DetectorElement.h"

#include <vector>

class GeoPhysVol;

namespace MuonGM {

class Cutout;
class Component;
class MMSpacerComponent;

class MMSpacer: public DetectorElement {

public:
  double width;
  double length;
  double thickness;
  double longWidth;   // for trapezoidal layers
  int index;
	
  MMSpacer(Component* s);
  GeoPhysVol* build(int minimalgeo);
  GeoPhysVol* build(int minimalgeo, int cutoutson, std::vector<Cutout*>);
  void print();

private:
  MMSpacerComponent* m_component;
};

} // namespace MuonGM


#endif
