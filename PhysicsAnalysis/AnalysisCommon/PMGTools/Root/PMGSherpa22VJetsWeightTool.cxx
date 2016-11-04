/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PMGSherpa22VJetsWeightTool.cxx 764400 2016-07-26 17:47:39Z tripiana $

// EDM include(s):
#include "xAODJet/JetContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "FourMomUtils/xAODP4Helpers.h"

// Local include(s):
#include "PMGTools/PMGSherpa22VJetsWeightTool.h"

namespace PMGTools {

  PMGSherpa22VJetsWeightTool::
  PMGSherpa22VJetsWeightTool( const std::string& name )
    : asg::AsgTool( name ),
      m_corrFactors( { { 1.0007-1, 0.9904-1, 0.9884-1, 0.9128-1, 0.8114-1,
	      0.7833-1, 0.6872-1, 0.6424-1, 0.6424-1 } } ) {
    
    // Declare the properties of the tool:
    declareProperty( "TruthJetContainer",
		     m_truthJetContainer = "AntiKt4TruthWZJets" );
    declareProperty( "TruthParticleContainer",
		     m_truthParticleContainer = "TruthParticles" );
  }
  
  StatusCode PMGSherpa22VJetsWeightTool::initialize() {
    
    // Tell the user what's happening:
    ATH_MSG_INFO( "Initializing " << name() << "..." );
    ATH_MSG_DEBUG( "Will be using:" );
    ATH_MSG_DEBUG( "  TruthJetContainer      = " << m_truthJetContainer );
    ATH_MSG_DEBUG( "  TruthParticleContainer = " << m_truthParticleContainer );
    
    // Ensure that the overall normalisation agrees with the Strict
    // integration setting
    static const double NORM = 1.007;
    for( double& corr : m_corrFactors ) {
      corr *= NORM;
    }
    
    // Return gracefully:
    return StatusCode::SUCCESS;
  }
  
  double PMGSherpa22VJetsWeightTool::getWeight() const {
    
    // Check if the configured truth jet container is available:
    if( ! evtStore()->contains< xAOD::JetContainer >( m_truthJetContainer ) ) {
      ATH_MSG_WARNING( "No TruthJetContainer (\"" << m_truthJetContainer
                       << "\") found! Dummy null weight retrieved." );
      return 0.;
    }
    
    // Calculate the weight:
    return getSherpa22VJets_NJetCorrection( m_truthJetContainer );
  }
  
  double PMGSherpa22VJetsWeightTool::
  getSherpa22VJets_NJetCorrection( size_t njettruth ) const {
    
    // See https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/CentralMC15ProductionList#NEW_Sherpa_v2_2_V_jets_NJet_rewe
    // Function of njettruth which is the number of truth jets with
    // pt>20 and |eta|<4.5
    
    // Use the last bin if we have more jets than what we have correction
    // factors for.
    if( njettruth >= m_corrFactors.size() ) {
      njettruth = m_corrFactors.size() - 1;
    }
    
    // Return the right value:
    return 1.0 + m_corrFactors[ njettruth ];
  }
  
  double PMGSherpa22VJetsWeightTool::
  getSherpa22VJets_NJetCorrection( const std::string &jetContainer ) const {
    
    // Count truth jets in collection, return correction:
    const size_t nTruthJets = getSherpa22VJets_NJet( jetContainer );
    // Use this in the correction calculation:
    return getSherpa22VJets_NJetCorrection( nTruthJets );
  }
  
  size_t PMGSherpa22VJetsWeightTool::
  getSherpa22VJets_NJet( const std::string& jetContainer ) const {
    
    //
    // Count number of truthjets compatible with evaluation of correction
    //
    
    // Check whether to do overlap removal with truth leptons:
    const bool doRemoval = ( jetContainer == "AntiKt4TruthJets" );
    
    // Retrieve the truth jets
    const xAOD::JetContainer* truthJets( 0 );
    if( evtStore()->retrieve( truthJets, jetContainer ).isFailure() ) {
      ATH_MSG_ERROR( "Couldn't retrieve truth jet container: "
                     << jetContainer );
      return 0;
    }
    
    size_t nTruthJets = 0;  
    
    // Retrieve the truth particles to get the leptons
    const xAOD::TruthParticleContainer* truthParticles( 0 );
    if( doRemoval ) {
      if( evtStore()->retrieve( truthParticles,
                                m_truthParticleContainer ).isFailure() ) {
	ATH_MSG_ERROR( "Couldn't retrieve truth particle container: "
		       << m_truthParticleContainer );
	return 0;
      }
    }
    
    // Loop over all truth jets passing basic cuts:
    for( const xAOD::Jet* truthJet : *truthJets ) {
      
      // Start with a simple kinematic cut:
      if( truthJet->pt() <= 20000. || std::abs( truthJet->eta() ) >= 4.5 ) {
	continue;
      }
      
      // Remove hadronic taus e.g. from Ztautau, Wtaunu decays
      static const SG::AuxElement::ConstAccessor< int >
	acc( "HadronConeExclTruthLabelID" );
      if( acc.isAvailable( *truthJet ) && ( acc( *truthJet ) == 15 ) ) {
	continue;
      }
      
      // If no overlap removal is to be made, we're done already:
      if( ! doRemoval ) {
	++nTruthJets;
	continue;
      }
      
      // Loop over the truth particles, to veto some jets:
      bool vetoJet = false;
      for( const xAOD::TruthParticle* truthParticle : *truthParticles ) {
	
	// Only select final state electrons and muons:
	if( ( truthParticle->status() != 1 ) ||
	    ( ( std::abs( truthParticle->pdgId() ) != 11 ) &&
	      ( std::abs( truthParticle->pdgId() ) != 13 ) ) ) {
	  continue;
	}
	// ...which also pass some basic kinematic cuts:
	if( ( truthParticle->pt() <= 20000.0 ) ||
	    ( std::abs( truthParticle->eta() ) >= 4.5 ) ) {
	  continue;
	}
	
	// Veto the jet if it's close to this lepton:
	static const double MIN_DISTANCE = 0.2 * 0.2;
	const double dR2 =
	  xAOD::P4Helpers::deltaR2( truthJet, truthParticle, true );
	if( dR2 < MIN_DISTANCE ) {
	  vetoJet = true;
	  break;
	}
      }
      
      // If the jet was not vetoed:
      if( ! vetoJet ) {
	++nTruthJets;
      }
    }
    
    // Return the calculated value:
    return nTruthJets;
  }

}
