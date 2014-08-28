/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IJetFinder.h

#ifndef IJetFinder_H
#define IJetFinder_H

// David Adsams
// January 2014

/// @class IJetFinder
/// IJetFinder is a dual-use tool interface for for a tool that
/// modifies a jet collection.

#include "AsgTools/IAsgTool.h"
#include "xAODJet/JetContainer.h"

namespace fastjet {
class PseudoJet;
}
/// Typedef for a vector of pseudojets.
typedef std::vector<fastjet::PseudoJet> PseudoJetVector;

class IJetFinder : virtual public asg::IAsgTool {
ASG_TOOL_INTERFACE(IJetFinder)

public:

  /// Destructor.
  virtual ~IJetFinder() { };

  /// Method to find jets from a vector of pseudojet inputs.
  /// The last argument is used to set the input type for the found jets.
  /// Returns 0 for success.
  virtual int find(const PseudoJetVector& inps, xAOD::JetContainer& jets,
                   xAOD::JetInput::Type =xAOD::JetInput::Uncategorized) const =0;

};

#endif
