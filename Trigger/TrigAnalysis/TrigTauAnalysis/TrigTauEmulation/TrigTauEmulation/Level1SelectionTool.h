/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2
#ifndef HAVE_LEVEL1SELECTIONTOOL_H
#define HAVE_LEVEL1SELECTIONTOOL_H

// Framework includes
#include "AsgTools/AsgTool.h"

// Local includes
#include "TrigTauEmulation/ILevel1SelectionTool.h"
#include "TrigTauEmulation/SelectionTool.h"

class Level1SelectionTool : public virtual ILevel1SelectionTool, public SelectionTool
{
  ASG_TOOL_CLASS(Level1SelectionTool, ILevel1SelectionTool)

  public:
    ~Level1SelectionTool() {};
    virtual StatusCode initialize() { return StatusCode::SUCCESS; };

  protected:
  
    Level1SelectionTool(const std::string& name) : SelectionTool(name) { };
    Level1SelectionTool(const Level1SelectionTool& other) : SelectionTool(other.name() + "_copy") { };
    Level1SelectionTool& operator=(const Level1SelectionTool& other) = delete;

};
#endif
