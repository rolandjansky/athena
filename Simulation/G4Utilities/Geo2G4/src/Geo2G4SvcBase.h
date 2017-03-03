/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEO2G4_Geo2G4SvcBase_H
#define GEO2G4_Geo2G4SvcBase_H

class VolumeBuilder;
#include <string>

class Geo2G4SvcBase
{
public:
  virtual void RegisterVolumeBuilder(VolumeBuilder* vb)=0;
  virtual void UnregisterVolumeBuilder(VolumeBuilder* vb)=0;
  virtual VolumeBuilder* GetVolumeBuilder(std::string s)=0;
  virtual VolumeBuilder* GetDefaultBuilder() =0;

  virtual void SetDefaultBuilder(VolumeBuilder*)=0;
  virtual void SetDefaultBuilder(std::string) =0;
  virtual void ListVolumeBuilders()=0;

  virtual bool UseTopTransforms()=0;
};
#endif
