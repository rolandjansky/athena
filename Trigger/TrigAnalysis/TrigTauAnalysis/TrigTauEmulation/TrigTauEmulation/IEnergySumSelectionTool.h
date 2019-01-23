/*
  Copyright (C) 2002-2017, 2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IENERGYSUMSELECTIONTOOL_ENERGYSUMSELECTIONTOOL_H
#define IENERGYSUMSELECTIONTOOL_ENERGYSUMSELECTIONTOOL_H

#include "AsgTools/IAsgTool.h"
#include "PATCore/AcceptInfo.h"
#include "PATCore/AcceptData.h"

#include "TrigTauEmulation/ILevel1SelectionTool.h"

//EDM include
#include "xAODTrigger/EnergySumRoI.h"

class IEnergySumSelectionTool : public virtual ILevel1SelectionTool
{

   ASG_TOOL_INTERFACE(IEnergySumSelectionTool)
    
 public:


  virtual const asg::AcceptInfo& getAcceptInfo() const = 0;

  virtual asg::AcceptData accept(const xAOD::EnergySumRoI& l1xe) const = 0;

  virtual ~IEnergySumSelectionTool() {};


};

#endif
