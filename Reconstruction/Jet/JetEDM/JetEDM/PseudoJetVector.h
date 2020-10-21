/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PseudoJetVector.h

#ifndef jet__PseudoJetVector_H
#define jet__PseudoJetVector_H

#include <vector>
#include "fastjet/PseudoJet.hh"
#include "xAODCore/CLASS_DEF.h"

// David Adams
// January 2014
//
// Container of fastjet pseudojets.

namespace jet {

typedef std::vector<fastjet::PseudoJet> PseudoJetVector;

}  // end jet namespace

CLASS_DEF( std::vector<fastjet::PseudoJet> , 240603737 , 1 )

#endif
