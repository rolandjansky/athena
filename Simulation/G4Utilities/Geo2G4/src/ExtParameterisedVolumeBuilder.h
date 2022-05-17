/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEO2G4_ExtParameterisedVolumeBuilder_H
#define GEO2G4_ExtParameterisedVolumeBuilder_H

#include "VolumeBuilder.h"
#include "Geo2G4AssemblyFactory.h"

#include "AthenaBaseComps/AthMessaging.h"
#include <string>

class Geo2G4AssemblyVolume;
class GeoMaterial;

class ExtParameterisedVolumeBuilder: public VolumeBuilder, public AthMessaging
{
public:
  ExtParameterisedVolumeBuilder(const std::string& n, Geo2G4AssemblyFactory* G4AssemblyFactory);
  ///
  G4LogicalVolume* Build(PVConstLink pv, OpticalVolumesMap* optical_volumes = 0) const;
  ///
  Geo2G4AssemblyVolume* BuildAssembly(const PVConstLink& pv) const;

 private:
  /// Prints info when some PhysVol contains both types (PV and ST) of daughters
  void PrintSTInfo(const std::string& volume) const;
  ///
  void getMatEther() const;

  mutable bool               m_getMatEther;
  mutable const GeoMaterial* m_matEther;
  mutable const GeoMaterial* m_matHypUr;

  Geo2G4AssemblyFactory* m_G4AssemblyFactory;
};

#endif
