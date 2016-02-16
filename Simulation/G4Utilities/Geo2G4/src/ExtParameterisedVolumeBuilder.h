/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ExtParameterisedVolumeBuilder_H
#define ExtParameterisedVolumeBuilder_H

#include "Geo2G4/VolumeBuilder.h"
#include "AthenaKernel/MsgStreamMember.h"
#include <string>

class Geo2G4AssemblyVolume;
class GeoMaterial;

class ExtParameterisedVolumeBuilder: public VolumeBuilder
{
public:
  ExtParameterisedVolumeBuilder(std::string n);
  ///
  LogicalVolume* Build(PVConstLink pv, OpticalVolumesMap* optical_volumes = 0) const;
  ///
  Geo2G4AssemblyVolume* BuildAssembly(PVConstLink pv) const;
  /// Log a message using the Athena controlled logging system
  MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }
  /// Check whether the logging system is active at the provided verbosity level
  bool msgLvl( MSG::Level lvl ) const { return m_msg.get().level() <= lvl; }
 private:
  /// Prints info when some PhysVol contains both types (PV and ST) of daughters
  void PrintSTInfo(std::string volume) const;
  ///
  void getMatEther() const;

  mutable bool               _getMatEther;
  mutable const GeoMaterial* _matEther;
  mutable const GeoMaterial* _matHypUr;
  /// Private message stream member
  mutable Athena::MsgStreamMember m_msg;
};

#endif
