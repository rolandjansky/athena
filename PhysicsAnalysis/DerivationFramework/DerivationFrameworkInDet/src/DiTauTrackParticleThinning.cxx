/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <vector>

// EDM include(s):
#include "xAODTau/DiTauJetContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

// Local include(s):
#include "DerivationFrameworkInDet/DiTauTrackParticleThinning.h"

namespace DerivationFramework {

   DiTauTrackParticleThinning::
   DiTauTrackParticleThinning( const std::string& type,
                               const std::string& name,
                               const IInterface* parent )
      : AthAlgTool( type, name, parent ),
        m_thinningSvc( "ThinningSvc", name ) {

      // Declare the tool's interface to the framework:
      declareInterface< DerivationFramework::IThinningTool >( this );

      // Set up the tool's properties:
      declareProperty( "ThinningService", m_thinningSvc,
                       "The thinning service to use" );
      declareProperty( "DiTauKey", m_ditauSGKey,
                       "StoreGate key of the DiTau container" );
      declareProperty( "InDetTrackParticlesKey", m_inDetSGKey,
                       "StoreGate key of the track particle container" );
      declareProperty( "SelectionString", m_selectionString,
                       "Selection string for the DiTau objects" );
      declareProperty( "ApplyAnd", m_and,
                       "Use IThinningSvc::Operator::And instead of "
                       "IThinningSvc::Operator::Or" );
   }

   StatusCode DiTauTrackParticleThinning::initialize() {

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

      if( m_ditauSGKey == "" ) {
         ATH_MSG_FATAL( "No ditau collection provided for thinning." );
         return StatusCode::FAILURE;
      }
      ATH_MSG_INFO( "Inner detector track particles associated with objects in "
                    << m_ditauSGKey << " will be retained in this format with "
                                       "the rest being thinned away" );

      // Set up the text-parsing machinery for selectiong the DiTau directly
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

   StatusCode DiTauTrackParticleThinning::finalize() {

      // Print some statistics:
      ATH_MSG_VERBOSE("finalize() ...");
      ATH_MSG_INFO( "Processed " << m_ntot << " tracks, " << m_npass
                    << " were retained " );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode DiTauTrackParticleThinning::doThinning() const {

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

      // Retrieve the DiTau container:
      const xAOD::DiTauJetContainer* importedDiTaus = nullptr;
      ATH_CHECK( evtStore()->retrieve( importedDiTaus, m_ditauSGKey ) );
      const size_t nDiTaus = importedDiTaus->size();

      // Select which DiTau objects to use:
      std::vector< const xAOD::DiTauJet* > ditauToCheck;
      if( m_parser ) {
         const std::vector< int > entries =
               m_parser->parser().evaluateAsVector();
         const size_t nEntries = entries.size();
         // check the sizes are compatible
         if( nDiTaus != nEntries ) {
            ATH_MSG_FATAL( "Sizes incompatible! Are you sure your selection "
                           "string used ditaus??" );
            return StatusCode::FAILURE;
         }
         // identify which ditaus to keep for the thinning check
         for( size_t i = 0; i < nDiTaus; ++i ) {
            if( entries.at( i ) == 1 ) {
               ditauToCheck.push_back( importedDiTaus->at( i ) );
            }
         }
      } else {
         // If no selection was specified, use all the DiTau objects:
         for( const xAOD::DiTauJet* dt : *importedDiTaus ) {
            ditauToCheck.push_back( dt );
         }
      }

      // Set elements in the mask to true if there is a corresponding
      // ElementLink from a reconstructed DiTau object
      for( const xAOD::DiTauJet* dt : ditauToCheck ) {
         // Process both the "normal" and the isolated track particles of the
         // DiTau object:
         ATH_CHECK( setMasks( dt->trackLinks(), mask ) );
         ATH_CHECK( setMasks( dt->isoTrackLinks(), mask ) );
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

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode DiTauTrackParticleThinning::
   setMasks( const xAOD::DiTauJet::TrackParticleLinks_t& links,
             std::vector< bool >& mask ) const {

      // Loop over the links:
      for( const auto& tpLink : links ) {
         // Make sure that the link is what we expect:
         if( ! tpLink.isValid() ) {
            continue;
         }
         if( tpLink.dataID() != m_inDetSGKey ) {
            ATH_MSG_FATAL( "The track particles of the DiTau object "
                           "are not from container \"" << m_inDetSGKey
                           << "\"" );
            return StatusCode::FAILURE;
         }
         // I yes, set the flag for keeping the track particle that it
         // points to:
         mask.at( tpLink.index() ) = true;
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace DerivationFramework
