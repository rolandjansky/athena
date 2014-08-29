/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SingleLVCopyBuilder_H
#define SingleLVCopyBuilder_H

#include "Geo2G4/VolumeBuilder.h"
#include <string>

class SingleLVCopyBuilder: public VolumeBuilder 
{
 public:
  SingleLVCopyBuilder(std::string n):VolumeBuilder(n){}

  virtual LogicalVolume* Build(PVConstLink pv, OpticalVolumesMap* optical_volumes = 0) const;
};

#endif
