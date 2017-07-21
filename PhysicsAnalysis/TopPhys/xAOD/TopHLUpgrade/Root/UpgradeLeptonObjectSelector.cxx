/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Filename: UpgradeLeptonObjectSelector.cxx
// Description:
// Author: Mark Owen
// Created: Sun Feb 22 15:18:45 2015

#include "TopHLUpgrade/UpgradeLeptonObjectSelector.h"

using namespace top;

UpgradeLeptonObjectSelector::UpgradeLeptonObjectSelector( Options opt /* = Options() */ )
  : m_opt( opt ){ /* Deliberately Empty */ }

/* virtual */ bool
UpgradeLeptonObjectSelector::apply( const xAOD::TruthParticle & truthParticle ) {

  // --------------------------------------------------
  // Always require 'reco' to pass
  if( truthParticle.auxdata<int>("passReco") !=1 ) { return false; }

  // --------------------------------------------------
  // Apply kinematic cut on the (smeared) pT:
  //     must exceed 25 GeV
  if ( truthParticle.pt() < m_opt.pt_min ){ return false; }

  // --------------------------------------------------
  // Apply kinematic cut on the eta:
  //     must be less than or equal to 2.5
  if ( std::abs( truthParticle.eta() ) > m_opt.eta_max ){ return false; }


  // --------------------------------------------------
  // Everything that reaches this point has passed the selection
  return true;

}
