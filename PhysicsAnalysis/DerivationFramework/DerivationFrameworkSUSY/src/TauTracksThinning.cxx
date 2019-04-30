/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <vector>

// EDM include(s):
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauTrackContainer.h"
#include "xAODTau/TauxAODHelpers.h"
#include "xAODTracking/TrackParticleContainer.h"

// Local include(s):
#include "DerivationFrameworkSUSY/TauTracksThinning.h"

namespace DerivationFramework {

   TauTracksThinning::
   TauTracksThinning( const std::string& type,
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
      declareProperty( "TauTracksKey", m_tracksSGKey,
                       "StoreGate key of the tau tracks container" );
      declareProperty( "IDTracksKey", m_IDtracksSGKey,
                       "StoreGate key of the associated ID track particle container" );
      declareProperty( "SelectionString", m_selectionString,
                       "Selection string for the e/gamma objects" );
      declareProperty( "ApplyAnd", m_and,
                       "Use IThinningSvc::Operator::And instead of "
                       "IThinningSvc::Operator::Or" );
   }

   StatusCode TauTracksThinning::initialize() {
      // Decide which collections need to be checked for tau tracks
      ATH_MSG_VERBOSE( "initialize() ..." );
      if( m_tracksSGKey == "" ) {
         ATH_MSG_FATAL( "No tau tracks collection provided for "
                        "thinning." );
         return StatusCode::FAILURE;
      }
      ATH_MSG_INFO( "Using " << m_tracksSGKey
                    << " as the source collection for tau tracks " );

      if( m_tauSGKey == "" ) {
         ATH_MSG_FATAL( "No tau jets collection provided for thinning." );
         return StatusCode::FAILURE;
      }
      ATH_MSG_INFO( "Tau tracks associated with objects in "
                    << m_tauSGKey << " will be retained in this format with "
                                     "the rest being thinned away" );

      if(!m_IDtracksSGKey.empty())
	ATH_MSG_INFO( "Associated ID track particles collection " << m_IDtracksSGKey
		      << " will be thinned accordingly" );
      
      // Set up the text-parsing machinery for selecting the taus directly
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

   StatusCode TauTracksThinning::finalize() {

      // Print some statistics:
      ATH_MSG_VERBOSE("finalize() ...");
      ATH_MSG_INFO( "Processed " << m_ntot << " tracks, " << m_npass
                    << " were retained " );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode TauTracksThinning::doThinning() const {
      // Retrieve main Tau Tracks collection
      const xAOD::TauTrackContainer* importedTrackParticles = nullptr;
      ATH_CHECK( evtStore()->retrieve( importedTrackParticles, m_tracksSGKey ) );

      // Check that the event contains tracks
      const size_t nTracks = importedTrackParticles->size();
      if( nTracks == 0 ) {
         return StatusCode::SUCCESS;
      }

      // Set up a mask with the same entries as the full tau tracks
      // collection
      std::vector< bool > mask( nTracks, false );
      m_ntot += nTracks;

      // Retrieve the tau jets container:
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

      // Set elements in the mask to true if there is a corresponding
      // ElementLink from a reconstructed tau object
      for( const xAOD::TauJet* tau : tauToCheck ) {
	// Get all the associated charged track particles:	
	std::vector<const xAOD::TauTrack *> allTracks = tau->allTracks();
	for(const xAOD::TauTrack *track : allTracks){
	  mask.at( track->index() ) = true;
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
      ATH_CHECK( m_thinningSvc->filter( *importedTrackParticles, mask, opType ) );


      // Associated ID track particle thinning
      if(!m_IDtracksSGKey.empty()) {
	
	const xAOD::TrackParticleContainer* importedIDTrackParticles = nullptr;
	ATH_CHECK( evtStore()->retrieve( importedIDTrackParticles, m_IDtracksSGKey ) );
	
	const size_t nIDTracks = importedIDTrackParticles->size();
	std::vector< bool > maskID( nIDTracks, false );
	
	for( const xAOD::TauJet* tau : tauToCheck ) {
	  std::vector<const xAOD::TauTrack *> allTracks = tau->allTracks();
	  for(const xAOD::TauTrack *track : allTracks) {
	    maskID.at(track->track()->index()) = true;
	  }
	}
	// Warning: use the same Or/And setting as for tau tracks
	ATH_CHECK( m_thinningSvc->filter( *importedIDTrackParticles, maskID, opType ) );
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace DerivationFramework
