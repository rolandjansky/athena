/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ComputeStaveServices_H
#define ComputeStaveServices_H

#include "InDetServMatGeoModel/StaveServices.h"
#include "AthenaKernel/MsgStreamMember.h"
#include "CxxUtils/checker_macros.h"

class ComputeStaveServices {
public:

  ComputeStaveServices(Athena::MsgStreamMember& msg):m_msg(msg) {}

  StaveServices compute( DetType::Type, DetType::Part, int layerNumber, int nModulesPerStave, int nChipsPerModule) const;
  int computeLVGaugeSerial( DetType::Type, DetType::Part, int layerNumber, 
			    int nModules, double moduleCurrent, double moduleVoltage,
			    double poweringLoss, double lossInCable, double cableLen) const;

 private:
  // the message stream (same for all derived classes)
  MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }
  mutable Athena::MsgStreamMember m_msg ATLAS_THREAD_SAFE;
  
};

#endif
