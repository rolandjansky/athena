/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetToolRunner.h

#ifndef JetToolRunner_H
#define JetToolRunner_H

// David Adams.
// January 2014
//
// This is the top-level tool for jet reconstruction.
// It calls a series of tools with interface IJetExecute.

#include "AsgTools/ToolHandleArray.h"
#include "AsgTools/AsgTool.h"
#include "EventShapeInterface/IEventShapeTool.h"
#include "JetInterface/IJetExecuteTool.h"
#include "TStopwatch.h"

/// Top-level tool for jet reconstruction.
/// Properties:
///   Tools - list of tools to run
///   Timer - Timing flag 1 for top-level, 2 for each called tool

class JetToolRunner
: virtual public asg::AsgTool,
  virtual public IJetExecuteTool {
ASG_TOOL_CLASS(JetToolRunner, IJetExecuteTool)

public:

  JetToolRunner(const std::string& myname);

  // Initialization. Check all tools here.
  StatusCode initialize();

  // Finalization. Write summary report.
  StatusCode finalize();

  // Loop over tools.
  int execute() const;

  // Display the configuration.
  void print() const;

private:

  // Properties.
  ToolHandleArray<IEventShapeTool> m_evstools {this, "EventShapeTools", {}};
  ToolHandleArray<IJetExecuteTool> m_exetools {this, "Tools", {}};
  int m_timer;

  // Clocks.
  mutable unsigned int m_nevt;
  mutable TStopwatch m_clock;
  mutable std::vector<TStopwatch> m_evsclocks;
  mutable std::vector<TStopwatch> m_execlocks;

};

#endif
