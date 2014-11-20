/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Geo2G4AssemblyFactory.h"
#include "Geo2G4AssemblyVolume.h"

#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoVPhysVol.h"

#include <iostream>
#include <map>

typedef std::map<const GeoVPhysVol*, Geo2G4AssemblyVolume*, std::less<const GeoVPhysVol*> > assemblyMap;

Geo2G4AssemblyFactory::Geo2G4AssemblyFactory()
{
}

Geo2G4AssemblyVolume* Geo2G4AssemblyFactory::Build(const PVConstLink thePhys,
                                                   bool& descend) const
{
  static assemblyMap _map;
  assemblyMap::const_iterator it;
  Geo2G4AssemblyVolume* _assembly;

  const GeoLogVol* theLog = thePhys->getLogVol();

  // Check if the assembly was used in GeoModel as a leaf node
  if(thePhys->getNChildVols() == 0)
    throw std::runtime_error(std::string("In GeoModel description the assembly ") + theLog->getName()
                             + std::string(" has no children!"));

  descend = true;

  // Search for the assembly in the map
  it = _map.find(&(*thePhys));
  if(it == _map.end())
    {
      _assembly = new Geo2G4AssemblyVolume();
      _map[&(*thePhys)] = _assembly;
    }
  else
    {
      _assembly = it->second;
      descend = false;
    }

  return _assembly;
}
