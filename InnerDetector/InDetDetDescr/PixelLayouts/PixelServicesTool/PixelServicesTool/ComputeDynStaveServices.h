/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ComputeDynStaveServices_H
#define ComputeDynStaveServices_H

#include "PixelServicesTool/StaveDynServices.h"
#include "AthenaKernel/MsgStreamMember.h"

class ComputeDynStaveServices {
public:

  ComputeDynStaveServices(const Athena::MsgStreamMember& msg):m_msg(msg) {}

  StaveDynServices compute( DetTypeDyn::Type, DetTypeDyn::Part, int layerNumber, int nModulesPerStave, int nChipsPerModule) const;
  int computeLVGaugeSerial( DetTypeDyn::Type, DetTypeDyn::Part, int layerNumber, 
			    int nModules, double moduleCurrent, double moduleVoltage,
			    double poweringLoss, double lossInCable, double cableLen) const;

 private:
  // the message stream (same for all derived classes)
  MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }
  const Athena::MsgStreamMember m_msg;
  
};

#endif
