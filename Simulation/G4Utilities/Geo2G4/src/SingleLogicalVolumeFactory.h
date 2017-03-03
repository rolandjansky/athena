/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEO2G4_SingleLogicalVolumeFactory_h
#define GEO2G4_SingleLogicalVolumeFactory_h

#include "AthenaKernel/MsgStreamMember.h"

class G4LogicalVolume;
class GeoLogVol;

#include <string>
#include <map>

class SingleLogicalVolumeFactory {
public:
  SingleLogicalVolumeFactory();
  G4LogicalVolume* Build(const GeoLogVol*) const;
  /// Log a message using the Athena controlled logging system
  MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }
  /// Check whether the logging system is active at the provided verbosity level
  bool msgLvl( MSG::Level lvl ) const { return m_msg.get().level() <= lvl; }
 private:
  /// Private message stream member
  mutable Athena::MsgStreamMember m_msg;
};

#endif
