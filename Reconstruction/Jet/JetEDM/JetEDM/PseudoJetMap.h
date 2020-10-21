/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PseudoJetMap.h

#ifndef jet__PseudoJetMap_H
#define jet__PseudoJetMap_H

#include <map>
#include "fastjet/PseudoJet.hh"
#include "xAODCore/CLASS_DEF.h"
#include "xAODJet/Jet.h"

// David Adams
// January 2014
//
// Container of fastjet pseudojets.

namespace jet {

typedef std::map<const xAOD::Jet*, const fastjet::PseudoJet*> PseudoJetMap;

}  // end jet namespace

CLASS_DEF( jet::PseudoJetMap, 159726523 , 1 )

#endif
