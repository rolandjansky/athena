/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AtlasInterfaces_Geo2G4SvcBase_H
#define G4AtlasInterfaces_Geo2G4SvcBase_H

#include <string>

class VolumeBuilder;

/// @todo NEEDS DOCUMENTATION
class Geo2G4SvcBase
{
public:
  virtual VolumeBuilder* GetVolumeBuilder(std::string s) const = 0;
  virtual VolumeBuilder* GetDefaultBuilder() const = 0;

  virtual void SetDefaultBuilder(std::string) = 0;
  virtual void ListVolumeBuilders() const = 0;

  virtual bool UseTopTransforms() const = 0;
};
#endif // G4AtlasInterfaces_Geo2G4SvcBase_H
