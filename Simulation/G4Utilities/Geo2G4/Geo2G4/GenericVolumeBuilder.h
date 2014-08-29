/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GenericVolumeBuilder_H
#define GenericVolumeBuilder_H

#include "Geo2G4/VolumeBuilder.h"

class GenericVolumeBuilder: public VolumeBuilder 
{
 public:
  GenericVolumeBuilder(std::string n):VolumeBuilder(n)
  {}

  LogicalVolume* Build(PVConstLink pv, OpticalVolumesMap* optical_volumes = 0) const;
};

#endif
