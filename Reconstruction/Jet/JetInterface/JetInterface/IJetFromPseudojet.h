/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IJetFromPseudojet.h

#ifndef IJetFromPseudojet_H
#define IJetFromPseudojet_H

#include <string>
#include <vector>
#include "AsgTools/IAsgTool.h"
#include "xAODJet/JetContainer.h"

namespace fastjet {
class PseudoJet;
}

class PseudoJetContainer;

/// IJetFromPseudojet is a dual-use tool interface for a tool that constructs
/// a new ATLAS jet from a fastjet pseudojet.

class IJetFromPseudojet : virtual public asg::IAsgTool {
ASG_TOOL_INTERFACE(IJetFromPseudojet)

public:

  /// Destructor.
  virtual ~IJetFromPseudojet() { };

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
