/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEO2G4_Geo2G4AssemblyFactory_h
#define GEO2G4_Geo2G4AssemblyFactory_h

#include "GeoModelKernel/GeoVPhysVol.h"
#include "Geo2G4AssemblyFactory.h"

#include <map>
#include <memory>

class Geo2G4AssemblyVolume;

class Geo2G4AssemblyFactory 
{
 public:
  Geo2G4AssemblyFactory();
  Geo2G4AssemblyVolume* Build(const PVConstLink thePhys,
			                  bool& descend);
 
 private:
  std::map<const GeoVPhysVol*, std::unique_ptr<Geo2G4AssemblyVolume>, std::less<const GeoVPhysVol*> > m_assemblyMap;
};

#endif
