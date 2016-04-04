/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2
#ifndef ILEVEL1SELECTIONTOOL_H
#define ILEVEL1SELECTIONTOOL_H

#include "AsgTools/IAsgTool.h"
#include "TrigTauEmulation/ISelectionTool.h"

// Pure virtual so that all our L1 emulation tools can inherit from this
// Don't implement this class as Level1SelectionTool - implement only derived classes ISomethingSelectionTool and SomethingSelectionTool
class ILevel1SelectionTool : public virtual ISelectionTool
{
  ASG_TOOL_INTERFACE(ILevel1SelectionTool)

  public:
    virtual ~ILevel1SelectionTool() {};

};

#endif
