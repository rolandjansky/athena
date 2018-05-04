/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IJetFromPseudojetMT.h

#ifndef IJetFromPseudojetMT_H
#define IJetFromPseudojetMT_H

#include <string>
#include <vector>
#include "AsgTools/IAsgTool.h"
#include "xAODJet/JetContainer.h"

namespace fastjet {
class PseudoJet;
}

class PseudoJetContainer;

/// IJetFromPseudojetMT is a dual-use tool interface for a tool that constructs
/// a new ATLAS jet from a fastjet pseudojet.

class IJetFromPseudojetMT : virtual public asg::IAsgTool {
ASG_TOOL_INTERFACE(IJetFromPseudojetMT)

public:

  /// Destructor.
  virtual ~IJetFromPseudojetMT() { };

  /// Method to construct an ATLAS jet from a pseudojet, input type and 
  /// vector of ghost labels.
  /// The new jet is put in the supplied container and is returned.
  /// Returns null for failure.
  virtual xAOD::Jet* add(const fastjet::PseudoJet& pj,
                         const PseudoJetContainer&,
                         xAOD::JetContainer& jets,
                         xAOD::JetInput::Type inputtype) const = 0;

  /// Method to construct an ATLAS jet from a pseudojet and parent jet.
  /// The new jet is put in the supplied container and is returned.
  /// Returns null for failure.
  virtual xAOD::Jet* add(const fastjet::PseudoJet& pj,
                         const PseudoJetContainer&,
                         xAOD::JetContainer& jets,
                         const xAOD::Jet* pparent) const =0;

};

#endif
