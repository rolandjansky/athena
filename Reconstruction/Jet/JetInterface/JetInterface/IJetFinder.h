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

#include <string>
#include <vector>
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

  /// Type for ghost labels.
  typedef std::vector<std::string> NameList;

  /// Destructor.
  virtual ~IJetFinder() { };

  /// Method to find jets from a vector of pseudojet inputs.
  /// The last arguments are the input type for the found jets and
  /// the list of ghost constituent labels.
  /// Returns 0 for success.
  virtual int find(const PseudoJetVector& inps, xAOD::JetContainer& jets,
                   xAOD::JetInput::Type contype, const NameList& ghostlabs) const =0;

};

#endif
