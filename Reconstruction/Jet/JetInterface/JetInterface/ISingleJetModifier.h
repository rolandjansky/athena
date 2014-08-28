/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ISingleJetModifier.h

#ifndef ISingleJetModifier_H
#define ISingleJetModifier_H

// Pierre-Antoine Delsart
// February 2014

/// ISingleJetModifier is a dual-use tool interface for a tool that modifies,
/// e.g.\ adds attributes to, a jet.

#include "AsgTools/IAsgTool.h"
#include "xAODJet/Jet.h"

class ISingleJetModifier : virtual public asg::IAsgTool {
ASG_TOOL_INTERFACE(ISingleJetModifier)

public:

  /// Destructor.
  virtual ~ISingleJetModifier() { };

  /// Method to modify a jet.
  /// Returns 0 for success.
  virtual int modifyJet(xAOD::Jet& jet) const =0;

};

#endif
