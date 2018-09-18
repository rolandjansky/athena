/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// ROOT include(s):
#include <TLorentzVector.h>
#include "FourMomUtils/P4Helpers.h"

// EDM include(s):
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODBase/IParticle.h"
#include "xAODEgamma/Egamma.h"
#include "xAODMuon/Muon.h"
#include "xAODTau/TauJet.h"

// Local include(s):
#include "DerivationFrameworkSUSY/NearbyTrackParticleThinning.h"

namespace DerivationFramework {

   NearbyTrackParticleThinning::NearbyTrackParticleThinning( const std::string& type,
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
                       "StoreGate key of the particle container to use" );
      declareProperty( "TPSGKey", m_inDetSGKey,
                       "StoreGate key of the track particle container to thin" );
      declareProperty( "SelectionString", m_selectionString,
                       "Selection string for the objects" );
      declareProperty( "ConeSize", m_coneSize,
                       "Size of cone to select tracks in" );
      declareProperty( "ApplyAnd", m_and,
                       "Use IThinningSvc::Operator::And instead of "
                       "IThinningSvc::Operator::Or" );
   }

   StatusCode NearbyTrackParticleThinning::initialize() {

      // Decide which collections need to be checked for calo clusters
      ATH_MSG_VERBOSE( "initialize() ..." );

      if( m_inDetSGKey != "" ) {
         ATH_MSG_INFO( "Using " << m_inDetSGKey
                       << " as the source collection for topo calo clusters" );
      }
      if( ( m_inDetSGKey == "" ) && ( m_inDetSGKey == "" ) ){
         ATH_MSG_FATAL( "No TrackParticle collection "
                        "provided for thinning." );
         return StatusCode::FAILURE;
      }

      if( m_sgKey == "" ) {
         ATH_MSG_FATAL( "No particle collection provided for thinning." );
         return StatusCode::FAILURE;
      }
      ATH_MSG_INFO( "TrackParticles associated with objects in " << m_sgKey
                    << " will be retained in this format with the rest being "
                       "thinned away" );

      // Set up the text-parsing machinery for selectiong the particles directly
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

   StatusCode NearbyTrackParticleThinning::finalize() {

      // Print some statistics:
      ATH_MSG_VERBOSE( "finalize() ..." );
      ATH_MSG_INFO( "Processed " << m_ntot << " TrackParticles, of which "
                    << m_npass << " were retained" );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode NearbyTrackParticleThinning::doThinning() const {

      // Retrieve TrackParticle collection if required
      const xAOD::TrackParticleContainer* importedTrackParticle = nullptr;
      if( m_inDetSGKey.size() ) {
         ATH_CHECK( evtStore()->retrieve( importedTrackParticle,
                                          m_inDetSGKey ) );
      }

      // Check the event contains clusters
      const size_t nTrackParticles = importedTrackParticle ? importedTrackParticle->size():0;
      if( nTrackParticles == 0 ) {
         return StatusCode::SUCCESS;
      }

      // Set up a mask with the same entries as the full cluster collection(s)
      std::vector< bool > mask( nTrackParticles, false );
      m_ntot += nTrackParticles;

      // Retrieve particle container
      const xAOD::IParticleContainer* importedParticles = nullptr;
      ATH_CHECK( evtStore()->retrieve( importedParticles, m_sgKey ) );

      //No particles in the input
      const size_t nParticles = importedParticles->size();
      if( nParticles == 0 ) {
         return StatusCode::SUCCESS;
      }

      // Check whether we'll be able to use the container:
      const xAOD::Type::ObjectType oType = importedParticles->at( 0 )->type();
      if( ( oType != xAOD::Type::Electron ) &&
          ( oType != xAOD::Type::Photon ) &&
          ( oType != xAOD::Type::Muon ) &&
          ( oType != xAOD::Type::Tau ) && ( m_coneSize < 0.0 ) ) {
         ATH_MSG_ERROR( "This tool is designed for Egamma, Muons and Taus and "
                        << m_sgKey << " is not a one of these collections. "
                        << "For general iParticles dR needs to be specified" );
         return StatusCode::FAILURE;
      }

      // Select the particles to use:
      std::vector< const xAOD::IParticle* > particlesToCheck;
      if( m_parser ) {
         const std::vector< int > entries =
               m_parser->parser().evaluateAsVector();
         unsigned int nEntries = entries.size();
         // check the sizes are compatible
         if( nParticles != nEntries ) {
            ATH_MSG_ERROR( "Sizes incompatible! Are you sure your selection "
                           "string used the right container??");
            return StatusCode::FAILURE;
         }
         // identify which particles to keep for the thinning check
         for( size_t i = 0; i < nParticles; ++i ) {
            if( entries.at( i ) == 1 ) {
               particlesToCheck.push_back( importedParticles->at( i ) );
            }
         }
      } else {
         // If no selection was specified, use all the particles:
         for( const xAOD::IParticle* p : *importedParticles ) {
            particlesToCheck.push_back( p );
         }
      }

      // Process the particles:
      for( const xAOD::IParticle* particle : particlesToCheck ) {
         if( importedTrackParticle ) {
	   ATH_CHECK( setTracksMask( mask, particle, importedTrackParticle ) );
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
      if( importedTrackParticle ) {
         ATH_CHECK( m_thinningSvc->filter( *importedTrackParticle, mask,
                                           opType ) );
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode NearbyTrackParticleThinning::setTracksMask( std::vector< bool >& mask,
							  const xAOD::IParticle* particle,
							  const xAOD::TrackParticleContainer* tpc ) const {

     float particleEta=particle->eta();
     float particlePhi=particle->phi();

     float trackEta=0;
     float trackPhi=0;
     float deltaEta=0;
     float deltaPhi=0;
     float deltaR=0;
     for( size_t i = 0; i < tpc->size(); ++i ) {
       const xAOD::TrackParticle* trkIt = tpc->at( i );
       trackEta = trkIt->eta();
       trackPhi = trkIt->phi();

       deltaEta = particleEta-trackEta;
       deltaPhi = P4Helpers::deltaPhi(particlePhi,trackPhi);
       
       deltaR = sqrt(deltaEta*deltaEta + deltaPhi*deltaPhi);
       
       if(deltaR < m_coneSize){
	 mask[i]=true;
       }
     }

     // return gracefully
     return StatusCode::SUCCESS;
   }


} // namespace DerivationFramework
