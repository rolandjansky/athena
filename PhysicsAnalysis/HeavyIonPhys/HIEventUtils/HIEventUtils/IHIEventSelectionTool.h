/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIEVENTUTILS_IHIEVENTSELECTIONTOOL_H__
#define HIEVENTUTILS_IHIEVENTSELECTIONTOOL_H__

#include "AsgTools/IAsgTool.h"

#include "PATCore/AcceptInfo.h"
#include "PATCore/AcceptData.h"

namespace HI
{

  class IHIEventSelectionTool : public virtual asg::IAsgTool
  {

    ASG_TOOL_INTERFACE( HI::IHIEventSelectionTool )

    public:

    virtual StatusCode initialize() = 0;
    virtual StatusCode finalize() = 0;

    virtual const asg::AcceptInfo& getAcceptInfo() const = 0;

  };

}

#endif
