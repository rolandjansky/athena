/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_ITRIGT1CALODATAACCESSV2_H
#define TRIGT1CALOBYTESTREAM_ITRIGT1CALODATAACCESSV2_H

#include "AsgTools/IAsgTool.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"


namespace LVL1BS {

class ITrigT1CaloDataAccessV2 : virtual public asg::IAsgTool {
	ASG_TOOL_INTERFACE(ITrigT1CaloDataAccess)
 public:
   virtual StatusCode loadTriggerTowers(xAOD::TriggerTowerContainer& container) = 0;

   // Use temporary container to call loadTriggerTowers and output
   // basic information to log (can be run from PyAthena)
   virtual StatusCode PrintTriggerTowers() = 0;
};


} // end of namespace

#endif
