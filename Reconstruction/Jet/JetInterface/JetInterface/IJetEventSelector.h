/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// IJetEventSelector.h

#ifndef IJetEventSelector_H
#define IJetEventSelector_H

/// IJetEventSelector is a dual-use tool interface for a tool that selects
/// a jet based on EventInfo selections (e.g. mu, nPV)

#include "AsgTools/IAsgTool.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"

class IJetEventSelector : virtual public asg::IAsgTool {
ASG_TOOL_INTERFACE(IJetEventSelector)

public:

  /// Destructor.
  virtual ~IJetEventSelector() { };

  /// Method to select.
  /// Returns true if eventinfo condition is passed.
  virtual int keep(const xAOD::EventInfo &e, const xAOD::JetContainer & jets) const =0;

};

#endif
