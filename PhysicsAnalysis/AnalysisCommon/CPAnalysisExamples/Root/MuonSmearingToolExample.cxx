/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonSmearingToolExample.cxx 300810 2014-06-06 12:28:18Z krasznaa $

// ROOT include(s):
#include <TRandom.h>

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"

// Local include(s):
#include "CPAnalysisExamples/MuonSmearingToolExample.h"

#include "PATInterfaces/SystematicSet.h"
#include "PATInterfaces/SystematicRegistry.h"

namespace CP {

   MuonSmearingToolExample::MuonSmearingToolExample( const std::string& name )
      : asg::AsgTool( name ) {

      // Teach the base class what systematic variations are supported
      // by this tool:
      m_affectingSystematics = {
         SystematicVariation( "mu_id",  1 ),
         SystematicVariation( "mu_id", -1 ),
         SystematicVariation( "mu_ms",  1 ),
         SystematicVariation( "mu_ms", -1 ),
         SystematicVariation( "mu_cb",  1 ),
         SystematicVariation( "mu_cb", -1 )
      };
      // Set all systematics as recommended
      m_recommendedSystematics = m_affectingSystematics;

   }

   StatusCode MuonSmearingToolExample::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initialising..." );

      // Register systematics with the registry
      SystematicRegistry& registry = SystematicRegistry::getInstance();
      if( registry.registerSystematics(*this) != SystematicCode::Ok )
         return StatusCode::FAILURE;

      // Set default for running without systematics
      applySystematicVariation( SystematicSet() ).ignore();

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   CorrectionCode MuonSmearingToolExample::applyCorrection( xAOD::Muon& mu ) {

      // Retrieve the event information:
      const xAOD::EventInfo* ei = 0;
      if( evtStore()->retrieve( ei, "EventInfo" ).isFailure() ) {
         ATH_MSG_WARNING( "No EventInfo object could be retrieved" );
         ATH_MSG_WARNING( "Random number generation not configured correctly" );
      }

      // Get the run and event numbers:
      const uint32_t runNumber = ei ? ei->runNumber() : 0;
      const unsigned long long eventNumber = ei ? ei->eventNumber() : 0;

      // Construct a seed for the random number generator:
      const UInt_t seed = 1000 * runNumber + eventNumber;
      gRandom->SetSeed( seed );

      // Decide about the parameters of the Gaus function based on the currently
      // active systematics settings:
      ::Double_t mean = 1.0;
      ::Double_t sigma = 0.1;
      if( appliedSystematics().find( SystematicVariation( "mu_id", 1 ) ) !=
          appliedSystematics().end() ) {
         mean = 1.1;
      } else if( appliedSystematics().find( SystematicVariation( "mu_id", -1 ) ) !=
                 appliedSystematics().end() ) {
         mean = 0.9;
      } else if( appliedSystematics().find( SystematicVariation( "mu_ms", 1 ) ) !=
                 appliedSystematics().end() ) {
         sigma = 0.2;
      } else if( appliedSystematics().find( SystematicVariation( "mu_ms", -1 ) ) !=
                 appliedSystematics().end() ) {
         sigma = 0.05;
      } else if( appliedSystematics().find( SystematicVariation( "mu_cb", 1 ) ) !=
                 appliedSystematics().end() ) {
         mean = 1.1;
         sigma = 0.2;
      } else if( appliedSystematics().find( SystematicVariation( "mu_cb", -1 ) ) !=
                 appliedSystematics().end() ) {
         mean = 0.9;
         sigma = 0.05;
      }

      // Smear the pT of the muon:
      mu.setP4( mu.pt() * gRandom->Gaus( mean, sigma ), mu.eta(), mu.phi() );

      // Return gracefully:
      return CorrectionCode::Ok;
   }

   SystematicCode MuonSmearingToolExample::sysApplySystematicVariation
   ( const SystematicSet& systConfig ) {

      // Only a single systematic can be applied at a time:
      if( systConfig.size() > 1 ) {
         return SystematicCode::Unsupported;
      }

      // Otherwise let's assume that things are correct:
      return SystematicCode::Ok;
   }

} // namespace CP
