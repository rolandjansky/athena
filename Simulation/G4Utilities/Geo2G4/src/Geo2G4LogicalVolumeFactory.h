/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Geo2G4LogicalVolumeFactory_h
#define Geo2G4LogicalVolumeFactory_h

class G4LogicalVolume;
class GeoLogVol;

class Geo2G4LogicalVolumeFactory {
public:
  Geo2G4LogicalVolumeFactory();
  G4LogicalVolume* Build(const GeoLogVol*) const;
};

#endif
