/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Sup_H
#define Sup_H

#include "MuonGeoModel/DetectorElement.h"
#include "MuonGeoModel/SupComponent.h"

class GeoVPhysVol;
class GeoShape;

namespace MuonGM {

class Cutout;

class Sup: public DetectorElement {
public:
   double width;
   double length;
   double thickness;
   double alThickness;

   // these are the coordinates of the AMDB origine for these components
   // with respect to a box of size width*length*thickness which contains
   // completely the component 

   Sup(Component* s);
   GeoVPhysVol* build();
   GeoVPhysVol* build(int cutoutson, std::vector<const GeoShape*>);
   void print();

private:
   SupComponent* m_component;
};

} // namespace MuonGM

#endif
