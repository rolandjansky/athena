/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ToolTester.cxx 300458 2014-04-25 09:08:18Z krasznaa $

// EDM include(s):
#include "xAODMuon/MuonContainer.h"

// Local include(s):
#include "ToolTester.h"

namespace CP {

   ToolTester::ToolTester( const std::string& name, ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc ),
        m_selTool( "CP::MuonSelectionToolExample/MuonSelectionToolExample", this ),
        m_smearTool( "CP::MuonSmearingToolExample/MuonSmearingToolExample", this ),
        m_effTool( "CP::MuonEfficiencyToolExample/MuonEfficiencyToolExample", this ) {

      declareProperty( "SGKey", m_sgKey = "Muons" );

      declareProperty( "MuonSelectionToolExample", m_selTool );
      declareProperty( "MuonSmearingToolExample", m_smearTool );
      declareProperty( "MuonEfficiencyToolExample", m_effTool );
   }

   StatusCode ToolTester::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initialising - Package version: " << PACKAGE_VERSION );
      ATH_MSG_DEBUG( "SGKey = " << m_sgKey );
      ATH_MSG_DEBUG( "MuonSelectionTool  = " << m_selTool );
      ATH_MSG_DEBUG( "MuonSmearingTool   = " << m_smearTool );
      ATH_MSG_DEBUG( "MuonEfficiencyTool = " << m_effTool );

      // Retrieve the tools:
      ATH_CHECK( m_selTool.retrieve() );
      ATH_CHECK( m_smearTool.retrieve() );
      ATH_CHECK( m_effTool.retrieve() );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode ToolTester::execute() {

      // Retrieve the muons:
      const xAOD::MuonContainer* muons = 0;
      ATH_CHECK( evtStore()->retrieve( muons, m_sgKey ) );
      ATH_MSG_INFO( "Number of muons: " << muons->size() );

      // Loop over them:
      xAOD::MuonContainer::const_iterator mu_itr = muons->begin();
      xAOD::MuonContainer::const_iterator mu_end = muons->end();
      for( ; mu_itr != mu_end; ++mu_itr ) {

         // Select "good" muons:
         if( ! m_selTool->accept( **mu_itr ) ) continue;

         // Print some info about the selected muon:
         ATH_MSG_INFO( "  Selected muon: eta = " << ( *mu_itr )->eta()
                       << ", phi = " << ( *mu_itr )->phi()
                       << ", pt = " << ( *mu_itr )->pt() );
         ATH_MSG_INFO( "    Primary track: eta = "
                       << ( *mu_itr )->primaryTrackParticle()->eta()
                       << ", phi = "
                       << ( *mu_itr )->primaryTrackParticle()->phi()
                       << ", pt = "
                       << ( *mu_itr )->primaryTrackParticle()->pt() );
         const xAOD::TrackParticle* idtrack =
            ( *mu_itr )->trackParticle( xAOD::Muon::InnerDetectorTrackParticle );
         if( idtrack ) {
            ATH_MSG_INFO( "         ID track: eta = " << idtrack->eta()
                          << ", phi = " << idtrack->phi()
                          << ", pt = " << idtrack->pt() );
         }

         // Create a calibrared muon:
         xAOD::Muon* mu = 0;
         if( ! m_smearTool->correctedCopy( **mu_itr, mu ) ) {
            ATH_MSG_WARNING( "Failed to smear the muon!" );
            continue;
         }
         ATH_MSG_INFO( "Calibrated muon: eta = " << mu->eta()
                       << ", phi = " << mu->phi()
                       << ", pt = " << mu->pt() );

         // Use the "simple interface" of the tool(s):
         float eff = 0.0, sf = 0.0;
         if( ! m_effTool->getEfficiency( *mu, eff ) ) {
            ATH_MSG_WARNING( "Couldn't get muon efficiency!" );
            delete mu;
            continue;
         }
         ATH_MSG_INFO( "       efficiency = " << eff );
         if( ! m_effTool->getScaleFactor( *mu, sf ) ) {
            ATH_MSG_WARNING( "Couldn't get muon scale factor!" );
            delete mu;
            continue;
         }
         ATH_MSG_INFO( "       scaleFactor = " << sf );

         // Call the tool on it in "decrating mode":
         if( ! m_effTool->applyEfficiency( *mu ) ) {
            ATH_MSG_WARNING( "Couldn't decorate muon!" );
            delete mu;
            continue;
         }
         ATH_MSG_INFO( "       efficiency = "
                       << mu->auxdata< float >( "MuonEfficiency" ) );

         // Delete the calibrated muon:
         delete mu;

      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace CP
