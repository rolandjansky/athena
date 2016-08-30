/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2
#ifndef HAVE_SELECTIONTOOL_H
#define HAVE_SELECTIONTOOL_H

// Framework includes
#include "AsgTools/AsgTool.h"

// Local includes
#include "TrigTauEmulation/ISelectionTool.h"
#include "TrigTauEmulation/extension.h"

using namespace bitpowder::lib;

class SelectionTool : public Extension<SelectionTool*>, public virtual ISelectionTool, public asg::AsgTool
{
  ASG_TOOL_CLASS(SelectionTool, ISelectionTool)

  public:
    ~SelectionTool() {};
    virtual StatusCode initialize() { return StatusCode::SUCCESS; } ;

  protected:
  
    SelectionTool(const std::string& name) : asg::AsgTool(name) {};
    SelectionTool(const SelectionTool& other) : asg::AsgTool(other.name() + "_copy") {};
    SelectionTool& operator=(const SelectionTool& other) = delete;


};
#endif
