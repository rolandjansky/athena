/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IJetFinder.h

#ifndef IJetFinder_H
#define IJetFinder_H

/// @class IJetFinder
/// IJetFinder is a dual-use tool interface for for a tool that
/// modifies a jet collection.

#include <string>
#include <vector>
#include "AsgTools/IAsgTool.h"
#include "xAODJet/JetContainer.h"

namespace fastjet {
class PseudoJet;
 class ClusterSequence;
}

class PseudoJetContainer;


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
  virtual int find(const PseudoJetContainer& cont,
                   xAOD::JetContainer & finalJets,
                   xAOD::JetInput::Type contype) const = 0;

  // Trigger-friendly form of find - no writes to event store.
  virtual int findNoSave(const PseudoJetContainer& cont, 
                         xAOD::JetContainer & finalJets,
                         xAOD::JetInput::Type contype,
                         fastjet::ClusterSequence*&) const = 0;


};

#endif
