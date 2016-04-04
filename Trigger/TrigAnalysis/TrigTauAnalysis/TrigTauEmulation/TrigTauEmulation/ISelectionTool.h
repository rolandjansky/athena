/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2
#ifndef ISELECTIONTOOL_H
#define ISELECTIONTOOL_H

#include "AsgTools/IAsgTool.h"

// Pure virtual so that all our L1 emulation tools can inherit from this
// Don't implement this class as SelectionTool - implement only derived classes ISomethingSelectionTool and SomethingSelectionTool
class ISelectionTool : public virtual asg::IAsgTool
{
  ASG_TOOL_INTERFACE(ISelectionTool)

  public:

    virtual ~ISelectionTool() {};

};

#endif
