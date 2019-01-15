/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// EDM include(s):
#include "xAODEgamma/EgammaContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronxAODHelpers.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"

// Local include(s):
#include "DerivationFrameworkInDet/EgammaTrackParticleThinning.h"
#include "DerivationFrameworkInDet/TracksInCone.h"

namespace DerivationFramework {

   EgammaTrackParticleThinning::
   EgammaTrackParticleThinning( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
      : AthAlgTool( type, name, parent ),
        m_thinningSvc( "ThinningSvc", name ) {

      // Declare the tool's interface to the framework:
      declareInterface< DerivationFramework::IThinningTool >( this );

      // Set up the tool's properties:
      declareProperty( "ThinningService", m_thinningSvc,
                       "The thinning service to use" );
      declareProperty( "SGKey", m_sgKey,
                       "StoreGate key of the e/gamma container" );
      declareProperty( "InDetTrackParticlesKey", m_inDetSGKey,
                       "StoreGate key of the track particle container" );
      declareProperty( "GSFTrackParticlesKey", m_gsfSGKey,
                       "StoreGate key of the GSF track particle container" );
      declareProperty( "SelectionString", m_selectionString,
                       "Selection string for the e/gamma objects" );
      declareProperty( "BestMatchOnly", m_bestMatchOnly,
                       "Select only the best matched track particles?" );
      declareProperty( "ConeSize", m_coneSize,
                       "Size of cone to select track particles in" );
      declareProperty( "ApplyAnd", m_and,
                       "Use IThinningSvc::Operator::And instead of "
                       "IThinningSvc::Operator::Or" );
   }

   StatusCode EgammaTrackParticleThinning::initialize() {

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

      if( m_sgKey == "" ) {
         ATH_MSG_FATAL( "No e-gamma collection provided for thinning." );
         return StatusCode::FAILURE;
      }
      ATH_MSG_INFO( "Inner detector track particles associated with objects in "
                    << m_sgKey << " will be retained in this format with the "
                                  "rest being thinned away" );

      if( m_gsfSGKey == "" ) {
         ATH_MSG_FATAL( "No GSF track collection provided for thinning." );
         return StatusCode::FAILURE;
      }
      ATH_MSG_INFO( "GSF track particles associated with objects in "
                    << m_gsfSGKey << " will be retained in this format with "
                                     "the rest being thinned away");

      // Set up the text-parsing machinery for selectiong the e/gamma directly
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

   StatusCode EgammaTrackParticleThinning::finalize() {

      // Print some statistics:
      ATH_MSG_VERBOSE( "finalize() ..." );
      ATH_MSG_INFO( "Processed " << m_ntot << " tracks, of which "
                    << m_npass<< " were retained" );
      ATH_MSG_INFO( "Processed " << m_ntotGSF << " GSF tracks, of which "
                    << m_nGSFPass << " were retained" );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode EgammaTrackParticleThinning::doThinning() const {

      // Retrieve main TrackParticle collection
      const xAOD::TrackParticleContainer* importedTrackParticles = nullptr;
      ATH_CHECK( evtStore()->retrieve( importedTrackParticles, m_inDetSGKey ) );

      // Retrieve GSF TrackParticle collection if required
      const xAOD::TrackParticleContainer* importedGSFTrackParticles = nullptr;
      ATH_CHECK( evtStore()->retrieve( importedGSFTrackParticles,
                                       m_gsfSGKey ) );

      // Check the event contains tracks
      const size_t nTracks = importedTrackParticles->size();
      const size_t nGSF = importedGSFTrackParticles->size();
      if( ( nTracks == 0 ) || ( nGSF == 0 ) ) {
         return StatusCode::SUCCESS;
      }

      // Set up a mask with the same entries as the full TrackParticle
      // collection(s)
      std::vector< bool > mask( nTracks, false ), gsfMask( nGSF, false );
      m_ntot += nTracks;
      m_ntotGSF += nGSF;

      // Retrieve e-gamma container
      const xAOD::EgammaContainer* importedEgamma = nullptr;
      ATH_CHECK( evtStore()->retrieve( importedEgamma, m_sgKey ) );
      const size_t nEgammas = importedEgamma->size();
      
      // Select which e/gamma objects to use:
      std::vector< const xAOD::Egamma* > egToCheck;
      if( m_parser ) {
         const std::vector< int > entries =
               m_parser->parser().evaluateAsVector();
         const size_t nEntries = entries.size();
         // check the sizes are compatible
         if( nEgammas != nEntries ) {
            ATH_MSG_FATAL( "Sizes incompatible! Are you sure your selection "
                           "string used e-gamma objects??" );
            return StatusCode::FAILURE;
         }
         // identify which e-gammas to keep for the thinning check
         for( size_t i = 0; i < nEgammas; ++i ) {
            if( entries.at( i ) == 1 ) {
               egToCheck.push_back( importedEgamma->at( i ) );
            }
         }
      } else {
         // If no selection was specified, use all e/g objects:
         for( const xAOD::Egamma* eg : *importedEgamma ) {
            egToCheck.push_back( eg );
         }
      }

      // Are we dealing with electrons or photons?

      if (nEgammas > 0) {
	const xAOD::Type::ObjectType egType = importedEgamma->at( 0 )->type();

	if( egType == xAOD::Type::Electron ) {
	  ATH_CHECK( setElectronMasks( mask, gsfMask, egToCheck,
				       importedTrackParticles,
				       importedGSFTrackParticles ) );
	} else if( egType == xAOD::Type::Photon ) {
	  ATH_CHECK( setPhotonMasks( mask, gsfMask, egToCheck,
				     importedTrackParticles,
				     importedGSFTrackParticles ) );
	} else {
	  ATH_MSG_FATAL( "The received objects are neither electrons or "
                        "photons?!?" );
	  return StatusCode::FAILURE;
	}
      }

      // Count up the mask contents
      for( bool bit : mask ) {
         if( bit ) {
            ++m_npass;
         }
      }
      for( bool bit : gsfMask ) {
         if( bit ) {
            ++m_nGSFPass;
         }
      }

      // Tell the thinning service what to do:
      const IThinningSvc::Operator::Type opType =
         ( m_and ? IThinningSvc::Operator::And : IThinningSvc::Operator::Or );
      ATH_CHECK( m_thinningSvc->filter( *importedTrackParticles, mask,
                                        opType ) );
      ATH_CHECK( m_thinningSvc->filter( *importedGSFTrackParticles, gsfMask,
                                        opType ) );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode EgammaTrackParticleThinning::
   setPhotonMasks( std::vector< bool >& tpMask,
                   std::vector< bool >& gsfTpMask,
                   const std::vector< const xAOD::Egamma* >& egammas,
                   const xAOD::TrackParticleContainer* tps,
                   const xAOD::TrackParticleContainer* gsfTps ) const {

      // Helper tool for selecting track particles in a cone:
      static DerivationFramework::TracksInCone trIC;

      // Loop over the photons:
      for( const xAOD::Egamma* eg : egammas ) {

         // Cast it to the right type:
         const xAOD::Photon* photon = dynamic_cast< const xAOD::Photon* >( eg );
         if( ! photon ) {
            ATH_MSG_FATAL( "Did not get a photon object" );
            return StatusCode::FAILURE;
         }

         // Select track particles around it:
         if( m_coneSize > 0.0 ) {
            trIC.select( photon, m_coneSize, tps, tpMask );
         }

         // Select the track particles belonging to vertices used by the photon:
         const auto& vertexLinks = photon->vertexLinks();
         for( const auto& vxLink : vertexLinks ) {
            // Avoid invalid links:
            if( ! vxLink.isValid() ) {
               continue;
            }
            // Access the track particles of the vertex:
            const auto& tpLinks = ( *vxLink )->trackParticleLinks();
            for( const auto& tpLink : tpLinks ) {
               // Make sure the link is what we expect:
               if( ! tpLink.isValid() ) {
                  continue;
               }
               if( ( *tpLink )->container() != gsfTps ) {
                  ATH_MSG_FATAL( "Track particle of the vertex not coming from "
                                 "the expected container (" << m_gsfSGKey
                                 << ")" );
                  return StatusCode::FAILURE;
               }
               // If yes, mark it for being kept:
               gsfTpMask.at( tpLink.index() ) = true;

               // Find the original track particle of this GSF track particle:
               const xAOD::TrackParticle* origTp =
                  xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF( *tpLink );
               if( ! origTp ) {
                  ATH_MSG_WARNING( "No original track particle found for "
                                   "GSF track particle" );
                  continue;
               }
               if( origTp->container() != tps ) {
                  ATH_MSG_FATAL( "Original track particle of the vertex not "
                                 "coming from the expected container ("
                                 << m_inDetSGKey << ")" );
                  return StatusCode::FAILURE;
               }
               // If all is well, mark it for being kept:
               tpMask.at( origTp->index() ) = true;
            }
            // If we're only interested in the best match, stop after the first
            // vertex:
            if( m_bestMatchOnly ) {
               break;
            }
         }
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode EgammaTrackParticleThinning::
   setElectronMasks( std::vector< bool >& tpMask,
                     std::vector< bool >& gsfTpMask,
                     const std::vector< const xAOD::Egamma* >& egammas,
                     const xAOD::TrackParticleContainer* tps,
                     const xAOD::TrackParticleContainer* gsfTps ) const {

      // Helper tool for selecting track particles in a cone:
      static DerivationFramework::TracksInCone trIC;

      // Loop over the electrons:
      for( const xAOD::Egamma* eg : egammas ) {

         // Cast it to the right type:
         const xAOD::Electron* electron =
               dynamic_cast< const xAOD::Electron* >( eg );
         if( ! electron ) {
            ATH_MSG_FATAL( "Did not get an electron object" );
            return StatusCode::FAILURE;
         }

         // Select track particles around it:
         if( m_coneSize > 0.0 ) {
            trIC.select( electron, m_coneSize, tps, tpMask );
         }

         // Loop over the track particles associated to this electron:
         const auto& tpLinks = electron->trackParticleLinks();
         for( const auto& tpLink : tpLinks ) {
            // Make sure the link is what we expect:
            if( ! tpLink.isValid() ) {
               continue;
            }
            if( ( *tpLink )->container() != gsfTps ) {
               ATH_MSG_FATAL( "Track particle of the electron not coming from "
                              "the expected container (" << m_gsfSGKey << ")" );
               return StatusCode::FAILURE;
            }
            // If yes, mark it for being kept:
            gsfTpMask.at( tpLink.index() ) = true;

            // Find the original track particle of this GSF track particle:
            const xAOD::TrackParticle* origTp =
               xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF( *tpLink );
            if( ! origTp ) {
               ATH_MSG_WARNING( "No original track particle found for "
                                "GSF track particle" );
               continue;
            }
            if( origTp->container() != tps ) {
               ATH_MSG_FATAL( "Original track particle of the electron not "
                              "coming from the expected container ("
                              << m_inDetSGKey << ")" );
               return StatusCode::FAILURE;
            }
            // If all is well, mark it for being kept:
            tpMask.at( origTp->index() ) = true;

            // If we're only interested in the best match, stop after the first
            // track particle:
            if( m_bestMatchOnly ) {
               break;
            }
         }
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace DerivationFramework
