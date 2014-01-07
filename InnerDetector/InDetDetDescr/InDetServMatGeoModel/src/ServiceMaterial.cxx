/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetServMatGeoModel/ServiceMaterial.h"

#include <ostream>

void ServiceMaterial::dump( std::ostream& os) const
{
  os << std::endl << "Dump of ServiceMaterial " << name() 
     << " containing " << components().size() << " components" << std::endl;
  double totalWeight = 0;
  for (unsigned int i=0; i<components().size(); i++) {
    ServiceMaterial::Entry comp = components().at(i);
    os << "Comp. " << i << " name " << comp.name 
       << " number " << comp.number
       << " weight " << comp.weight
       << " is linear? " << comp.linear
       << std::endl;
    totalWeight += comp.weight * comp.number;
  }
  os << "Total weight: " << totalWeight << std::endl;
}
