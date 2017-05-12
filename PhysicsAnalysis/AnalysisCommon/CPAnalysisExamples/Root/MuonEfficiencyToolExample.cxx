/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonEfficiencyToolExample.cxx 299754 2014-03-28 15:37:02Z krasznaa $

// Local include(s):
#include "CPAnalysisExamples/MuonEfficiencyToolExample.h"

namespace CP {

   MuonEfficiencyToolExample::MuonEfficiencyToolExample( const std::string& name )
      : asg::AsgTool( name ) {

      declareProperty( "DummyProperty", m_dummyProperty = 10 );
   }

   StatusCode MuonEfficiencyToolExample::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initialising... " );
      ATH_MSG_DEBUG( "Dummy property = " << m_dummyProperty );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   CorrectionCode MuonEfficiencyToolExample::getEfficiency( const xAOD::Muon& mu,
                                                     float& eff ) {

      // Check if we have an efficiency for this muon:
      if( std::abs( mu.eta() ) > 2.4 ) {
         return CorrectionCode::OutOfValidityRange;
      }

      // If yes, then:
      eff = 0.9;
      return CorrectionCode::Ok;
   }

   CorrectionCode MuonEfficiencyToolExample::applyEfficiency( xAOD::Muon& mu ) {

      // Get the efficiency:
      float eff = 0.0;
      if( getEfficiency( mu, eff ) == CorrectionCode::Error ) {
         return CorrectionCode::Error;
      }

      // Decorate the muon:
      mu.auxdata< float >( "MuonEfficiency" ) = eff;
      return CorrectionCode::Ok;
   }

   CorrectionCode MuonEfficiencyToolExample::getScaleFactor( const xAOD::Muon& mu,
                                                      float& sf ) {

      // Check if we have a scale factor for this muon:
      if( std::abs( mu.eta() ) > 2.4 ) {
         return CorrectionCode::OutOfValidityRange;
      }

      // If yes, then:
      sf = 1.05;
      return CorrectionCode::Ok;
   }

   CorrectionCode MuonEfficiencyToolExample::applyScaleFactor( xAOD::Muon& mu ) {

      // Get the efficiency:
      float sf = 0.0;
      if( getScaleFactor( mu, sf ) == CorrectionCode::Error ) {
         return CorrectionCode::Error;
      }

      // Decorate the muon:
      mu.auxdata< float >( "MuonScaleFactor" ) = sf;
      return CorrectionCode::Ok;
   }

} // namespace CP
