/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IJetSelector.h

#ifndef IJetSelector_H
#define IJetSelector_H

// David Adams
// March 2014

/// IJetSelector is a dual-use tool interface for a tool that selects
/// a jet.

#include "AsgTools/IAsgTool.h"
#include "xAODJet/Jet.h"

class IJetSelector : virtual public asg::IAsgTool {
ASG_TOOL_INTERFACE(IJetSelector)

public:

  /// Destructor.
  virtual ~IJetSelector() { };

  /// Method to select.
  /// Returns true if jet is selected.
  virtual int keep(const xAOD::Jet& jet) const =0;

};

#endif
