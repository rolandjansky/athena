/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_IZDCDATAACCESSV2_H
#define TRIGT1CALOBYTESTREAM_IZDCDATAACCESSV2_H

#include "AsgTools/IAsgTool.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"


//namespace LVL1BS {

class IZdcDataAccessV2 : virtual public asg::IAsgTool {
	ASG_TOOL_INTERFACE(IZdcDataAccess)
 public:
   virtual StatusCode loadTriggerTowers(xAOD::TriggerTowerContainer& container) = 0;

   // Use temporary container to call loadTriggerTowers and output
   // basic information to log (can be run from PyAthena)
   virtual StatusCode PrintTriggerTowers() = 0;
};


//} // end of namespace

#endif
