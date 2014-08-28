/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IJetGroomer.h

#ifndef IJetGroomer_H
#define IJetGroomer_H

// David Adams
// January 2014

/// IJetGroomer is a dual-use tool interface for grooming jets,
/// i.e.\ transforming one jet into another.

#include "AsgTools/IAsgTool.h"
#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"

class IJetGroomer : virtual public asg::IAsgTool {
ASG_TOOL_INTERFACE(IJetGroomer)

public:

  /// Destructor.
  virtual ~IJetGroomer() { };

  /// Transform jet. If accepted, the new jet is added to the container.
  /// Return codes:
  ///  -  0 for success.
  ///  - >0 if jet is rejected
  ///  - <0 for error
  virtual int groom(const xAOD::Jet& jin, xAOD::JetContainer& jout) const =0;

};

#endif
