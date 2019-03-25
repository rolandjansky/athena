/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEO2G4_ExtParameterisedVolumeBuilder_H
#define GEO2G4_ExtParameterisedVolumeBuilder_H

#include "VolumeBuilder.h"
#include "Geo2G4AssemblyFactory.h"

#include "AthenaKernel/MsgStreamMember.h"
#include <string>

class Geo2G4AssemblyVolume;
class GeoMaterial;

class ExtParameterisedVolumeBuilder: public VolumeBuilder
{
public:
  ExtParameterisedVolumeBuilder(std::string n, Geo2G4AssemblyFactory* G4AssemblyFactory);
  ///
  G4LogicalVolume* Build(PVConstLink pv, OpticalVolumesMap* optical_volumes = 0);
  ///
  Geo2G4AssemblyVolume* BuildAssembly(PVConstLink pv);
  /// Log a message using the Athena controlled logging system
  MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }
  /// Check whether the logging system is active at the provided verbosity level
  bool msgLvl( MSG::Level lvl ) const { return m_msg.get().level() <= lvl; }
 private:
  /// Prints info when some PhysVol contains both types (PV and ST) of daughters
  void PrintSTInfo(std::string volume) const;
  ///
  void getMatEther() const;

  mutable bool               m_getMatEther;
  mutable const GeoMaterial* m_matEther;
  mutable const GeoMaterial* m_matHypUr;
  /// Private message stream member
  mutable Athena::MsgStreamMember m_msg;
  
  Geo2G4AssemblyFactory* m_G4AssemblyFactory;
};

#endif
