/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __IHIEVENTSELECTIONTOOL_H__
#define __IHIEVENTSELECTIONTOOL_H__

#include "AsgTools/IAsgTool.h"
#include "PATCore/TAccept.h"

namespace HI
{

  class IHIEventSelectionTool : public virtual asg::IAsgTool
  {
    
    ASG_TOOL_INTERFACE( HI::IHIEventSelectionTool )
    
    public:      
    
    virtual StatusCode initialize() = 0;
    virtual StatusCode finalize() = 0;
    
    virtual const Root::TAccept& getTAccept() const = 0;
    virtual const Root::TAccept& accept() const = 0;
    
  };

}
  
#endif
