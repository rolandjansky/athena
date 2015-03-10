/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ParameterisedVolumeBuilder_H
#define ParameterisedVolumeBuilder_H

#include "Geo2G4/VolumeBuilder.h"

class ParameterisedVolumeBuilder: public VolumeBuilder 
{
 public:
  ParameterisedVolumeBuilder(std::string n):VolumeBuilder(n){}
  
  LogicalVolume* Build(PVConstLink pv, OpticalVolumesMap* optical_volumes = 0) const;
};

#endif
