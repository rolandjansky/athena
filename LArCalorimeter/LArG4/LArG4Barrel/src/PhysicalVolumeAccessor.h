/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PhysicalVolumeAccessor_H
#define PhysicalVolumeAccessor_H

// Helper class for getting all physical volumes (by name) in a LV
// and organise them by copy #

#include "G4VPhysicalVolume.hh"

#include <map>
#include <string>

typedef std::map<int,G4VPhysicalVolume *, std::less<int> > PVMap;

class PhysicalVolumeAccessor {
public:
  PhysicalVolumeAccessor(std::string);
  PhysicalVolumeAccessor(std::string,const std::string);
  const G4VPhysicalVolume* GetPhysicalVolume(int) const;
  void SetPhysicalVolumeList(std::string);
private:
  G4LogicalVolume* m_theLogicalVolume;
  PVMap m_thePhysicalVolumes;
  G4LogicalVolume *GetLV(std::string);
};


#endif
