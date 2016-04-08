/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Micromegas_H
#define Micromegas_H

#include "MuonGeoModel/DetectorElement.h"
#include "MuonGeoModel/MicromegasComponent.h"

class GeoFullPhysVol;

namespace MuonGM {

class Cutout;

class Micromegas: public DetectorElement {

public:
  double width;
  double length;
  double thickness;
  double longWidth;   // for trapezoidal layers
  int index;
	
  Micromegas(Component* s);
  GeoFullPhysVol* build(int minimalgeo);
  GeoFullPhysVol* build(int minimalgeo, int cutoutson, std::vector<Cutout*>);
  void print();

private:
  MicromegasComponent* m_component;
};

} // namespace MuonGM


#endif
