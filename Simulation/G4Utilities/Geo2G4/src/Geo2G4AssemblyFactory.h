/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEO2G4_Geo2G4AssemblyFactory_h
#define GEO2G4_Geo2G4AssemblyFactory_h

#include "GeoModelKernel/GeoVPhysVol.h"

class Geo2G4AssemblyVolume;

class Geo2G4AssemblyFactory 
{
 public:
  Geo2G4AssemblyFactory();
  Geo2G4AssemblyVolume* Build(const PVConstLink,
			  bool&) const;
};

#endif
