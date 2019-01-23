/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IJETROISELECTIONTOOL_JETROISELECTIONTOOL_H
#define IJETROISELECTIONTOOL_JETROISELECTIONTOOL_H

#include "AsgTools/IAsgTool.h"
#include "PATCore/AcceptInfo.h"
#include "PATCore/AcceptData.h"

#include "TrigTauEmulation/ILevel1SelectionTool.h"

//EDM include
#include "xAODTrigger/JetRoI.h"

class IJetRoISelectionTool : public virtual ILevel1SelectionTool
{

   ASG_TOOL_INTERFACE(IJetRoISelectionTool)
    
 public:


  virtual const asg::AcceptInfo& getAcceptInfo() const = 0;
  virtual asg::AcceptData accept(const xAOD::JetRoI& l1jet) const = 0;

  virtual ~IJetRoISelectionTool() {};


};

#endif
