/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Mdt_H
#define Mdt_H

#include "MuonGeoModel/DetectorElement.h"
#include "MuonGeoModel/MdtComponent.h"


class GeoFullPhysVol;

namespace MuonGM {

class MultiLayer;
class Cutout;

class Mdt: public DetectorElement {

public:
   double width;
   double length;
   double thickness;
   double longWidth;	// for trapezoidal layers
   int index;
   double tubelenStepSize;
   double tubePitch;
	
   Mdt(Component* s1, std::string s2);
   ~Mdt();
   MultiLayer* layer;
   GeoFullPhysVol* build();
   GeoFullPhysVol* build(std::vector<Cutout*>);
   void print();
   void setGeoVersion(int ver);
   int getGeoVersion() {return m_geo_version;}

private:
   MdtComponent* m_component;
   int m_geo_version;
   Mdt & operator=(const Mdt &right);
   Mdt(const Mdt&);

};

} // namespace MuonGM


#endif
