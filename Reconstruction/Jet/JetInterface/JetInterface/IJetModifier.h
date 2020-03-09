/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IJetModifier.h

// David Adams
// January 2014

/// IJetModifier is a dual-use tool interface for a tool that
/// modifies a jet collection.

#ifndef IJetModifier_H
#define IJetModifier_H

#include "AsgTools/IAsgTool.h"
#include "xAODJet/JetContainer.h"
#include "JetInterface/IJetPseudojetRetriever.h"

class IJetModifier : virtual public asg::IAsgTool {
ASG_TOOL_INTERFACE(IJetModifier)

public:

  /// Method to modify a jet collection.
  virtual StatusCode modify(xAOD::JetContainer& jets) const = 0;

};

#endif
