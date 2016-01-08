/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelServicesTool/ServiceDynMaterial.h"

#include <ostream>

void ServiceDynMaterial::dump( std::ostream& os) const
{
  os << std::endl << "Dump of ServiceDynMaterial " << name() 
     << " containing " << components().size() << " components" << std::endl;
  double totalWeight = 0;
  for (int i=0; i<(int)components().size(); i++) {
    ServiceDynMaterial::Entry comp = components().at(i);
    os << "Comp. " << i << " name " << comp.name 
       << " number " << comp.number
       << " weight " << comp.weight
       << " is linear? " << comp.linear
       << std::endl;
    totalWeight += comp.weight * comp.number;
  }
  os << "Total weight: " << totalWeight << std::endl;
}
