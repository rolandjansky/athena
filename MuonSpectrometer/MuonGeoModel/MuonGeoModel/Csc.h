/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Csc_H
#define Csc_H

#include "GeoModelKernel/GeoFullPhysVol.h"
#include "MuonGeoModel/DetectorElement.h"
#include "MuonGeoModel/CscComponent.h"

namespace MuonGM {

class CscMultiLayer;
class Cutout;

class Csc: public DetectorElement {

public:
  double width;
  double length;
  double thickness;
  double longWidth;    // for trapezoidal layers
  double excent;       // for csc layers
  double physicalLength;
  double maxwLength;
  double upWidth;

  int index;
	
  Csc(Component* s);
  ~Csc();
  CscMultiLayer* layer;
  GeoFullPhysVol* build(int minimalgeo);
  GeoFullPhysVol* build(int minimalgeo, int cutoutson,
                        std::vector<Cutout*>);
  void print();

private:
  CscComponent* m_component;
  Csc & operator=(const Csc &right);
  Csc(const Csc&);
};

} // namespace MuonGM

#endif
