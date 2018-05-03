/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IJetFinderMT.h

#ifndef IJetFinderMT_H
#define IJetFinderMT_H

/// @class IJetFinderMT
/// IJetFinderMT is a dual-use tool interface for for a tool that
/// modifies a jet collection.

#include <string>
#include <vector>
#include "AsgTools/IAsgTool.h"
#include "xAODJet/JetContainer.h"
//#include "JetRec/PseudoJetContainer.h"

namespace fastjet {
class PseudoJet;
}

class PseudoJetContainer;


/// Typedef for a vector of pseudojets.
typedef std::vector<fastjet::PseudoJet> PseudoJetVector;

class IJetFinderMT : virtual public asg::IAsgTool {
ASG_TOOL_INTERFACE(IJetFinderMT)

public:

  /// Type for ghost labels.
  typedef std::vector<std::string> NameList;

  /// Destructor.
  virtual ~IJetFinderMT() { };

  /// Method to find jets from a vector of pseudojet inputs.
  /// The last arguments are the input type for the found jets and
  /// the list of ghost constituent labels.
  /// Returns 0 for success.
  virtual int find(const PseudoJetContainer& cont, xAOD::JetContainer & finalJets,
                   xAOD::JetInput::Type contype) const = 0;

  // Trigger-friendly form of find - no writes to event store.
  virtual int findNoSave(const PseudoJetContainer& cont, 
                         xAOD::JetContainer & finalJets,
                         xAOD::JetInput::Type contype) const = 0;


};

#endif
