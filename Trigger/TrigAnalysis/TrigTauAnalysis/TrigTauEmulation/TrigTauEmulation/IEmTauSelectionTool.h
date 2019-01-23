/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IEMTAUSELECTIONTOOL_EMTAUSELECTIONTOOL_H
#define IEMTAUSELECTIONTOOL_EMTAUSELECTIONTOOL_H

#include "AsgTools/IAsgTool.h"
#include "PATCore/AcceptInfo.h"
#include "PATCore/AcceptData.h"
#include "TrigTauEmulation/ILevel1SelectionTool.h"

//EDM include
#include "xAODTrigger/EmTauRoI.h"

class IEmTauSelectionTool : public virtual ILevel1SelectionTool
{
   ASG_TOOL_INTERFACE(IEmTauSelectionTool)
    
 public:

  virtual const asg::AcceptInfo& getAcceptInfo() const = 0;
  virtual asg::AcceptData accept(const xAOD::EmTauRoI& l1tau) const = 0;
  virtual ~IEmTauSelectionTool() {};

};

#endif
