/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <vector>

// EDM include(s):
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauxAODHelpers.h"
#include "xAODTracking/TrackParticleContainer.h"

// Local include(s):
#include "DerivationFrameworkInDet/TauTrackParticleThinning.h"
#include "DerivationFrameworkInDet/TracksInCone.h"

namespace DerivationFramework {

   TauTrackParticleThinning::
   TauTrackParticleThinning( const std::string& type,
                             const std::string& name,
                             const IInterface* parent )
      : AthAlgTool( type, name, parent ),
        m_thinningSvc( "ThinningSvc", name ) {

      // Declare the tool's interface to the framework:
      declareInterface< DerivationFramework::IThinningTool >( this );

      // Set up the tool's properties:
      declareProperty( "ThinningService", m_thinningSvc,
                       "The thinning service to use" );
      declareProperty( "TauKey", m_tauSGKey,
                       "StoreGate key of the tau container to use" );
      declareProperty( "InDetTrackParticlesKey", m_inDetSGKey,
                       "StoreGate key of the track particle container" );
      declareProperty( "SelectionString", m_selectionString,
                       "Selection string for the e/gamma objects" );
      declareProperty( "ConeSize", m_coneSize,
                       "Size of cone to select track particles in" );
      declareProperty( "ApplyAnd", m_and,
                       "Use IThinningSvc::Operator::And instead of "
                       "IThinningSvc::Operator::Or" );
      declareProperty( "DoTauTracksThinning", m_doTauTracksThinning,
                       "Apply thinning to tau tracks in addition to ID tracks" );
      declareProperty( "TauTracksKey", m_tauTracksSGKey,
                       "StoreGate key of the tau track container" );
   }

   StatusCode TauTrackParticleThinning::initialize() {

      // Decide which collections need to be checked for ID TrackParticles
      ATH_MSG_VERBOSE( "initialize() ..." );
      if( m_inDetSGKey == "" ) {
         ATH_MSG_FATAL( "No inner detector track collection provided for "
                        "thinning." );
         return StatusCode::FAILURE;
      }
      ATH_MSG_INFO( "Using " << m_inDetSGKey
                    << " as the source collection for inner detector track "
                       "particles" );

      if( m_tauSGKey == "" ) {
         ATH_MSG_FATAL( "No tau collection provided for thinning." );
         return StatusCode::FAILURE;
      }
      ATH_MSG_INFO( "Inner detector track particles associated with objects in "
                    << m_tauSGKey << " will be retained in this format with "
                                     "the rest being thinned away" );

      // Set up the text-parsing machinery for selectiong the taus directly
      // according to user cuts
      if( m_selectionString != "" ) {
         m_parser.reset( new ExpressionParserHelper( evtStore() ) );
         if( ! m_parser->parser().loadExpression( m_selectionString ) ) {
            ATH_MSG_FATAL( "Failed to interpret expression: "
                           << m_selectionString );
            return StatusCode::FAILURE;
         }
      }

      // Access the thinning service:
      ATH_CHECK( m_thinningSvc.retrieve() );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode TauTrackParticleThinning::finalize() {

      // Print some statistics:
      ATH_MSG_VERBOSE("finalize() ...");
      ATH_MSG_INFO( "Processed " << m_ntot << " tracks, " << m_npass
                    << " were retained " );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode TauTrackParticleThinning::doThinning() const {

      // Retrieve main TrackParticle collection
      const xAOD::TrackParticleContainer* importedTrackParticles = nullptr;
      ATH_CHECK( evtStore()->retrieve( importedTrackParticles, m_inDetSGKey ) );

      // Check that the event contains tracks
      const size_t nTracks = importedTrackParticles->size();
      if( nTracks == 0 ) {
         return StatusCode::SUCCESS;
      }

      // Set up a mask with the same entries as the full TrackParticle
      // collection
      std::vector< bool > mask( nTracks, false );
      m_ntot += nTracks;

      // Retrieve the tau container:
      const xAOD::TauJetContainer* importedTaus = nullptr;
      ATH_CHECK( evtStore()->retrieve( importedTaus, m_tauSGKey ) );
      const size_t nTaus = importedTaus->size();

      // Select which taus to use:
      std::vector< const xAOD::TauJet* > tauToCheck;
      if( m_parser ) {
         const std::vector< int > entries =
               m_parser->parser().evaluateAsVector();
         const size_t nEntries = entries.size();
         // check the sizes are compatible
         if( nTaus != nEntries ) {
            ATH_MSG_ERROR( "Sizes incompatible! Are you sure your selection "
                           "string used taus??" );
            return StatusCode::FAILURE;
         }
         // identify which taus to keep for the thinning check
         for( size_t i = 0; i < nTaus; ++i ) {
            if( entries.at( i ) == 1 ) {
               tauToCheck.push_back( importedTaus->at( i ) );
            }
         }
      } else {
         // If no selection was specified, use all the tau objects:
         for( const xAOD::TauJet* tau : *importedTaus ) {
            tauToCheck.push_back( tau );
         }
      }

      // Helper object for selecting track particles in a cone around the taus
      static DerivationFramework::TracksInCone trIC;

      // Set elements in the mask to true if there is a corresponding
      // ElementLink from a reconstructed tau object
      for( const xAOD::TauJet* tau : tauToCheck ) {
         // Get all the associated charged track particles:
         auto tpLinks =
            xAOD::TauHelpers::trackParticleLinks( tau,
               xAOD::TauJetParameters::TauTrackFlag::classifiedCharged );
         // Process the links:
         for( const auto& tpLink : tpLinks ) {
            // Check that it is what we expect:
            if( ! tpLink.isValid() ) {
               continue;
            }
            if( tpLink.dataID() != m_inDetSGKey ) {
               ATH_MSG_FATAL( "Charged track particle does not come from "
                              "container \"" << m_inDetSGKey << "\"" );
               return StatusCode::FAILURE;
            }
            // If it is, set the mask for it:
            mask.at( tpLink.index() ) = true;
         }
         // Select the track particles in a cone if it was requested (NOT RECOMMENDED):
         if( m_coneSize > 0.0 ) {
            trIC.select( tau, m_coneSize, importedTrackParticles, mask );
         }
      }

      // Count up the mask contents
      for( bool bit : mask ) {
         if( bit ) {
            ++m_npass;
         }
      }

      // Tell the thinning service what to do:
      const IThinningSvc::Operator::Type opType =
         ( m_and ? IThinningSvc::Operator::And : IThinningSvc::Operator::Or );
      ATH_CHECK( m_thinningSvc->filter( *importedTrackParticles, mask,
                                        opType ) );
      
      // Apply thinning to tau track collection if requested
      if( m_doTauTracksThinning ) {
	const xAOD::TauTrackContainer* importedTauTracks = nullptr;
	ATH_CHECK( evtStore()->retrieve( importedTauTracks, m_tauTracksSGKey ) );
	
	if( importedTauTracks->size() == 0 ) {
	  return StatusCode::SUCCESS;
	}    
	
	std::vector< bool > mask_tautracks( importedTauTracks->size(), false );
	
	for( const xAOD::TauJet* tau : tauToCheck ) {
	  // Get all the associated charged tau tracks:
	  auto ttLinks = tau->tauTrackLinks(xAOD::TauJetParameters::TauTrackFlag::classifiedCharged );
	  // Process the links:
	  for( const auto& ttLink : ttLinks ) {
            if( ! ttLink.isValid() ) {
	      continue;
            }
            if( ttLink.dataID() != m_tauTracksSGKey ) {
               ATH_MSG_FATAL( "Charged tau track does not come from "
                              "container \"" << m_tauTracksSGKey << "\"" );
               return StatusCode::FAILURE;
            }
            // If it is, set the mask for it:
            mask_tautracks.at( ttLink.index() ) = true;
         }
         // Select the tau tracks in a cone if it was requested (NOT RECOMMENDED):
	  if( m_coneSize > 0.0 ) {
	    trIC.select( tau, m_coneSize, importedTauTracks, mask_tautracks );
	  }
	}
	
	// don't expect And-type thinning operation will be needed for tau tracks
	ATH_CHECK( m_thinningSvc->filter( *importedTauTracks, mask_tautracks,
					  IThinningSvc::Operator::Or ) );
      }
      
      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace DerivationFramework
