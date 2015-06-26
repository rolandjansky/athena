/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IJETROISELECTIONTOOL_JETROISELECTIONTOOL_H
#define IJETROISELECTIONTOOL_JETROISELECTIONTOOL_H

#include "AsgTools/IAsgTool.h"
#include "PATCore/TAccept.h"

//EDM include
#include "xAODTrigger/JetRoI.h"

class IJetRoISelectionTool : public virtual asg::IAsgTool
{

   ASG_TOOL_INTERFACE(IJetRoISelectionTool)
    
 public:


  virtual const Root::TAccept& accept(const xAOD::JetRoI& l1jet) const = 0;

  virtual ~IJetRoISelectionTool() {};


};

#endif
