/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SingleLVCopyBuilder_H
#define SingleLVCopyBuilder_H

#include "Geo2G4/VolumeBuilder.h"
#include "AthenaKernel/MsgStreamMember.h"
#include <string>

class SingleLVCopyBuilder: public VolumeBuilder
{
 public:
  SingleLVCopyBuilder(std::string n):VolumeBuilder(n),m_msg(n){}

  virtual LogicalVolume* Build(PVConstLink pv, OpticalVolumesMap* optical_volumes = 0) const;
  /// Log a message using the Athena controlled logging system
  MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }
  /// Check whether the logging system is active at the provided verbosity level
  bool msgLvl( MSG::Level lvl ) const { return m_msg.get().level() <= lvl; }
 private:
  /// Private message stream member
  mutable Athena::MsgStreamMember m_msg;

};

#endif
