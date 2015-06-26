/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IENERGYSUMSELECTIONTOOL_ENERGYSUMSELECTIONTOOL_H
#define IENERGYSUMSELECTIONTOOL_ENERGYSUMSELECTIONTOOL_H

#include "AsgTools/IAsgTool.h"
#include "PATCore/TAccept.h"

//EDM include
#include "xAODTrigger/EnergySumRoI.h"

class IEnergySumSelectionTool : public virtual asg::IAsgTool
{

   ASG_TOOL_INTERFACE(IEnergySumSelectionTool)
    
 public:


  virtual const Root::TAccept& accept(const xAOD::EnergySumRoI& l1xe) const = 0;

  virtual ~IEnergySumSelectionTool() {};


};

#endif
