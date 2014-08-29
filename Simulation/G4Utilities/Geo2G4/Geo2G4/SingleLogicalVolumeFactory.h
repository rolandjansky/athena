/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SingleLogicalVolumeFactory_h
#define SingleLogicalVolumeFactory_h

class G4LogicalVolume;
class GeoLogVol;

#include <string>
#include <map>

class SingleLogicalVolumeFactory {
public:
	SingleLogicalVolumeFactory();
	G4LogicalVolume* Build(const GeoLogVol*) const;
};

#endif
