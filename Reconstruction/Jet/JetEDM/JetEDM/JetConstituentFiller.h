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

#include <string>
#include <vector>
#include "xAODJet/Jet.h"
#include "JetEDM/PseudoJetVector.h"

namespace xAOD {
  class IParticle;
}

namespace jet {

class JetConstituentFiller {

public :

  JetConstituentFiller(bool isTrig=false) :m_isTrigger(isTrig){}

  typedef std::vector<std::string> NameList;

  /// Build and fill constituents of jet from its PseudoJet (or from ppj)
  /// Returns the number of pseudojet constituents without user info
  /// or < 0 for error.
  /// If the jet does not have an associated pseudojet, then the
  /// last argument is used in its place.
  /// The second argments holds the list of ghost association names.
  int extractConstituents(xAOD::Jet& jet, const NameList* pghostlabs,
                          const fastjet::PseudoJet* ppj =0);

  /// Build and fill constituents of jet from its PseudoJet (or from ppj)
  /// Returns the number of pseudojet constituents without user info
  /// or < 0 for error.
  /// If the jet does not have an associated pseudojet, then the
  /// last argument is used in its place.
  /// This is for backwards compatability with code that targets release 19.1
  int extractConstituents(xAOD::Jet& jet, const fastjet::PseudoJet* ppj =0);

  /// Returns the jet's constituents as a vector of PseudoJet 
  /// if ignoreGhosts==true, ghost constituents are removed from the output.
  static PseudoJetVector constituentPseudoJets(const xAOD::Jet& jet, bool ignoreGhosts=true);

 protected:
  bool m_isTrigger;
    
};

}  // end jet namespace

#endif
