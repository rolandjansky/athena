/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEO2G4_Geo2G4LVFactory_h
#define GEO2G4_Geo2G4LVFactory_h

#include "GeoModelKernel/GeoVPhysVol.h"

class G4LogicalVolume;
class GeoLogVol;

class Geo2G4LVFactory 
{
 public:
  Geo2G4LVFactory();
  G4LogicalVolume* Build(const PVConstLink&,
			 bool&) const;
};

#endif
