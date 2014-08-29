/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ExtParameterisedVolumeBuilder_H
#define ExtParameterisedVolumeBuilder_H

#include "Geo2G4/VolumeBuilder.h"
#include <string>

class Geo2G4AssemblyVolume;
class GeoMaterial;

class ExtParameterisedVolumeBuilder: public VolumeBuilder
{
public:
  ExtParameterisedVolumeBuilder(std::string n);

  LogicalVolume* Build(PVConstLink pv, OpticalVolumesMap* optical_volumes = 0) const;

  Geo2G4AssemblyVolume* BuildAssembly(PVConstLink pv) const;

 private:
  // Prints Info when some PhysVol contains both types (PV and ST) of daughters 
  void PrintSTInfo(std::string volume) const;

  mutable bool               _getMatEther;
  mutable const GeoMaterial* _matEther;

  void getMatEther() const;
};

#endif
