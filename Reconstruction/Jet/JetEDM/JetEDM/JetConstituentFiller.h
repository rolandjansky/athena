/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetConstituentFiller.h

#ifndef jet__JetConstituentFiller_H
#define jet__JetConstituentFiller_H

// David Adams and Pierre-Antoine Delsart
// January 2014
//
// Utility that copies the pseudojet constituents to
// ATLAS jet constituents (including ghosts).

#include <vector>
#include "xAODJet/Jet.h"
#include "JetEDM/PseudoJetVector.h"

namespace xAOD {
  class IParticle;
}

namespace jet {

class JetConstituentFiller {

public :

  /// Build and fill constituents of jet from its PseudoJet (or from ppj)
  /// Returns the number of pseudojet constituents without user info
  /// or < 0 for error.
  /// If the jet does not have an associated pseudojet, then the
  /// second argument is used in its place.
  int extractConstituents(xAOD::Jet& jet, const fastjet::PseudoJet* ppj =0);

  /// Returns the jet's constituents as a vector of PseudoJet 
  /// if ignoreGhosts==true, ghost constiuents are removed from the output.
  static PseudoJetVector constituentPseudoJets(const xAOD::Jet& jet, bool ignoreGhosts=true);
};

}  // end jet namespace

#endif
