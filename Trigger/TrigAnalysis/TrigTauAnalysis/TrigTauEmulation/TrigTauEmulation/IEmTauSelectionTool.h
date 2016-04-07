/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IEMTAUSELECTIONTOOL_EMTAUSELECTIONTOOL_H
#define IEMTAUSELECTIONTOOL_EMTAUSELECTIONTOOL_H

#include "AsgTools/IAsgTool.h"
#include "PATCore/TAccept.h"
#include "TrigTauEmulation/ILevel1SelectionTool.h"

//EDM include
#include "xAODTrigger/EmTauRoI.h"

class IEmTauSelectionTool : public virtual ILevel1SelectionTool
{
   ASG_TOOL_INTERFACE(IEmTauSelectionTool)
    
 public:

  virtual const Root::TAccept& accept(const xAOD::EmTauRoI& l1tau) const = 0;
  virtual ~IEmTauSelectionTool() {};

};

#endif
