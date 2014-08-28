/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IJetBuildTool.h

#ifndef IJetBuildTool_H
#define IJetBuildTool_H

// David Adams
// January 2014

/// IJetBuildTool is a dual-use tool interface for a tool
/// that builds a jet collection.

#include "AsgTools/IAsgTool.h"
#include "xAODJet/JetContainer.h"

class IJetBuildTool : virtual public asg::IAsgTool {
ASG_TOOL_INTERFACE(IJetBuildTool)

public:

  /// Destructor.
  virtual ~IJetBuildTool() { };

  /// Method to build the collection and return it to the caller.
  /// Returns 0 for error.
  virtual const xAOD::JetContainer* build() const =0;

};

#endif
