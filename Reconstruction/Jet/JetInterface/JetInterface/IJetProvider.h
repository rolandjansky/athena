/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// IJetBuildTool.h

#ifndef JETINTERFACE_IJETBUILDER_H
#define JETINTERFACE_IJETBUILDER_H


/// IJetProvider is a dual-use tool interface for a tool
/// that builds a jet collection.

#include "AsgTools/IAsgTool.h"
#include "xAODJet/JetContainer.h"

class IJetProvider : virtual public asg::IAsgTool {
ASG_TOOL_INTERFACE(IJetProvider)

public:

  /// Destructor.
  virtual ~IJetProvider() { };

  /// Method to build the collection and return it to the caller.
  virtual  xAOD::JetContainer* build() const =0;


};

#endif
