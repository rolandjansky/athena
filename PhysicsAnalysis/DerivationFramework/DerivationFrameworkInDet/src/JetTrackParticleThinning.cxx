/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <vector>

// EDM include(s):
#include "xAODJet/JetContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

// Local include(s):
#include "DerivationFrameworkInDet/JetTrackParticleThinning.h"

namespace DerivationFramework {

   JetTrackParticleThinning::
   JetTrackParticleThinning( const std::string& type,
                             const std::string& name,
                             const IInterface* parent )
      : AthAlgTool( type, name, parent ),
        m_thinningSvc( "ThinningSvc", name ) {

      // Declare the tool's interface to the framework:
      declareInterface< DerivationFramework::IThinningTool >( this );

      // Set up the tool's properties:
      declareProperty( "ThinningService", m_thinningSvc,
                       "The thinning service to use" );
      declareProperty( "JetKey", m_jetSGKey,
                       "StoreGate key of the jet container to use" );
      declareProperty( "InDetTrackParticlesKey", m_inDetSGKey,
                       "StoreGate key of the track particle container" );
      declareProperty( "SelectionString", m_selectionString,
                       "Selection string for the jet objects" );
      declareProperty( "DeltaRMatch", m_dRMatch,
                       "DeltaR matching criteria in addition to ghost-association" );
      declareProperty( "ApplyAnd", m_and,
                       "Use IThinningSvc::Operator::And instead of "
                       "IThinningSvc::Operator::Or" );
   }

   StatusCode JetTrackParticleThinning::initialize() {

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

      if( m_jetSGKey == "" ) {
         ATH_MSG_FATAL( "No jet collection provided for thinning." );
         return StatusCode::FAILURE;
      }
      ATH_MSG_INFO( "Inner detector track particles associated with objects in "
                    << m_jetSGKey << " will be retained in this format with "
                                     "the rest being thinned away" );

      // Set up the text-parsing machinery for selectiong the jet directly
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

   StatusCode JetTrackParticleThinning::finalize() {

      // Print some statistics:
      ATH_MSG_VERBOSE("finalize() ...");
      ATH_MSG_INFO( "Processed " << m_ntot << " tracks, " << m_npass
                    << " were retained " );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode JetTrackParticleThinning::doThinning() const {

      // Retrieve main TrackParticle collection
      const xAOD::TrackParticleContainer* importedTrackParticles = nullptr;
      ATH_CHECK( evtStore()->retrieve( importedTrackParticles, m_inDetSGKey ) );

      // Check the event contains tracks
      const size_t nTracks = importedTrackParticles->size();
      if( nTracks == 0 ) {
         return StatusCode::SUCCESS;
      }

      // Set up a mask with the same entries as the full TrackParticle
      // collection
      std::vector< bool > mask( nTracks, false );
      m_ntot += nTracks;

      // Retrieve the jet container:
      const xAOD::JetContainer* importedJets = nullptr;
      ATH_CHECK( evtStore()->retrieve( importedJets, m_jetSGKey ) );
      const size_t nJets = importedJets->size();

      // Select which jets to use:
      std::vector< const xAOD::Jet* > jetToCheck;
      if( m_parser ) {
         const std::vector< int > entries =
               m_parser->parser().evaluateAsVector();
         const size_t nEntries = entries.size();
         // check the sizes are compatible
         if( nJets != nEntries ) {
            ATH_MSG_ERROR( "Sizes incompatible! Are you sure your selection "
                           "string used jets??" );
            return StatusCode::FAILURE;
         }
         // identify which jets to keep for the thinning check
         for( size_t i = 0; i < nJets; ++i ) {
            if( entries.at( i ) == 1 ) {
               jetToCheck.push_back( importedJets->at( i ) );
            }
         }
      } else {
         // If no selection was specified, use all the jet objects:
         for( const xAOD::Jet* jet : *importedJets ) {
            jetToCheck.push_back( jet );
         }
      }

      // Set elements in the mask to true if there is a corresponding
      // ElementLink from a reconstructed jet object
      for( const xAOD::Jet* jet : jetToCheck ) {

         // Get the track particles associated to this jet:
         std::vector< const xAOD::TrackParticle* > jetTracks;
         if( ! jet->getAssociatedObjects( xAOD::JetAttribute::GhostTrack,
                                          jetTracks ) ) {
            ATH_MSG_WARNING( "Associated tracks not found" );
            continue;
         }

         // Process the track particles:
         for( const xAOD::TrackParticle* tp : jetTracks ) {
            // Check that it is coming from the expected container:
            if( tp->container() != importedTrackParticles ) {
               ATH_MSG_FATAL( "Track particle associated to jet is not coming "
                              "from container \"" << m_inDetSGKey << "\"" );
               return StatusCode::FAILURE;
            }
            // If it is, update the mask:
            mask.at( tp->index() ) = true;
         }
      }

      // Set elements in the mask to true if they lie within dR of a 
      // reconstructed jet object if configured to do so
      if(m_dRMatch > 0.0) {
        for ( const xAOD::TrackParticle* tp : *importedTrackParticles ) {

          // If mask is already set to true, skip track
          if( mask.at( tp->index() ) ) continue;

          for( const xAOD::Jet* jet : jetToCheck ) {
            // Check if a jet lies within dR of track

            if(tp->p4().DeltaR(jet->p4()) <= m_dRMatch) {
              // If so, update the mask
              mask.at( tp->index() ) = true;
              // and skip the rest of the jets
              break;
            }
          }
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

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace DerivationFramework
