/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// ROOT include(s):
#include <TLorentzVector.h>

// EDM include(s):
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODBase/IParticle.h"
#include "xAODEgamma/Egamma.h"
#include "xAODMuon/Muon.h"
#include "xAODTau/TauJet.h"

// Local include(s):
#include "DerivationFrameworkCalo/CaloClusterThinning.h"
#include "DerivationFrameworkCalo/ClustersInCone.h"

namespace DerivationFramework {

   CaloClusterThinning::CaloClusterThinning( const std::string& type,
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
                       "StoreGate key of the lepton container to use" );
      declareProperty( "CaloClCollectionSGKey", m_CaloClSGKey,
                       "StoreGate key of the calo cluster container to thin" );
      declareProperty( "TopoClCollectionSGKey", m_TopoClSGKey,
                       "StoreGate key of the topo cluster container to thin" );
      declareProperty( "SelectionString", m_selectionString,
                       "Selection string for the e/gamma objects" );
      declareProperty( "ConeSize", m_coneSize,
                       "Size of cone to select clusters in" );
      declareProperty( "ApplyAnd", m_and,
                       "Use IThinningSvc::Operator::And instead of "
                       "IThinningSvc::Operator::Or" );
   }

   StatusCode CaloClusterThinning::initialize() {

      // Decide which collections need to be checked for calo clusters
      ATH_MSG_VERBOSE( "initialize() ..." );

      if( m_CaloClSGKey != "" ) {
         ATH_MSG_INFO( "Using " << m_CaloClSGKey
                       << " as the source collection for calo clusters" );
      }
      if( m_TopoClSGKey != "" ) {
         ATH_MSG_INFO( "Using " << m_TopoClSGKey
                       << " as the source collection for topo calo clusters" );
      }
      if( ( m_CaloClSGKey == "" ) && ( m_TopoClSGKey == "" ) ){
         ATH_MSG_FATAL( "No CalCaloTopoCluster or CaloCluster collection "
                        "provided for thinning." );
         return StatusCode::FAILURE;
      }

      if( m_sgKey == "" ) {
         ATH_MSG_FATAL( "No particle collection provided for thinning." );
         return StatusCode::FAILURE;
      }
      ATH_MSG_INFO( "Calo clusters associated with objects in " << m_sgKey
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

   StatusCode CaloClusterThinning::finalize() {

      // Print some statistics:
      ATH_MSG_VERBOSE( "finalize() ..." );
      ATH_MSG_INFO( "Processed " << m_ntot << " clusters, of which "
                    << m_npass << " were retained" );
      ATH_MSG_INFO( "Processed "<< m_ntotTopo << " topo clusters, of which "
                    << m_npassTopo << " were retained" );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode CaloClusterThinning::doThinning() const {

      // Retrieve egCluster collection
      const xAOD::CaloClusterContainer* importedCaloCluster = nullptr;
      if( m_CaloClSGKey.size() ) {
         ATH_CHECK( evtStore()->retrieve( importedCaloCluster,
                                          m_CaloClSGKey ) );
      }

      // Retrieve CalCaloTopo collection if required
      const xAOD::CaloClusterContainer* importedTopoCaloCluster = nullptr;
      if( m_TopoClSGKey.size() ) {
         ATH_CHECK( evtStore()->retrieve( importedTopoCaloCluster,
                                          m_TopoClSGKey ) );
      }

      // Check the event contains clusters
      const size_t nClusters = ( importedCaloCluster ?
                                 importedCaloCluster->size() : 0 );
      const size_t nTopoClusters = ( importedTopoCaloCluster ?
                                     importedTopoCaloCluster->size() : 0 );
      if( ( nClusters == 0 ) && ( nTopoClusters == 0 ) ) {
         return StatusCode::SUCCESS;
      }

      // Set up a mask with the same entries as the full cluster collection(s)
      std::vector< bool > mask( nClusters, false ),
                          topomask( nTopoClusters, false );
      m_ntot += nClusters;
      m_ntotTopo += nTopoClusters;

      // Retrieve particle container
      const xAOD::IParticleContainer* importedParticles = nullptr;
      ATH_CHECK( evtStore()->retrieve( importedParticles, m_sgKey ) );

      const size_t nParticles = importedParticles->size();

      // Check whether we'll be able to use the container:
      
      if (nParticles>0) {
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
	  if( importedCaloCluster ) {
            ATH_CHECK( particleCluster( mask, particle, importedCaloCluster ) );
            ATH_CHECK( setClustersMask( mask, particle, importedCaloCluster ) );
	  }
	  if( importedTopoCaloCluster ) {
            ATH_CHECK( particleCluster( topomask, particle,
                                        importedTopoCaloCluster ) );
            ATH_CHECK( setClustersMask( topomask, particle,
                                        importedTopoCaloCluster ) );
	  }
      }
      }

      // Count up the mask contents
      for( bool bit : mask ) {
         if( bit ) {
            ++m_npass;
         }
      }
      for( bool bit : topomask ) {
         if( bit ) {
            ++m_npassTopo;
         }
      }

      // Tell the thinning service what to do:
      const IThinningSvc::Operator::Type opType =
         ( m_and ? IThinningSvc::Operator::And : IThinningSvc::Operator::Or );
      if( importedCaloCluster ) {
         ATH_CHECK( m_thinningSvc->filter( *importedCaloCluster, mask,
                                           opType ) );
      }
      if( importedTopoCaloCluster ) {
         ATH_CHECK( m_thinningSvc->filter( *importedTopoCaloCluster, topomask,
                                           opType ) );
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode CaloClusterThinning::
   setClustersMask( std::vector< bool >& mask,
                    const xAOD::IParticle* particle,
                    const xAOD::CaloClusterContainer* cps ) const {

      switch( particle->type() ) {

         case xAOD::Type::Electron:
         case xAOD::Type::Photon:
            {
               const xAOD::Egamma* eg =
                     static_cast< const xAOD::Egamma* >( particle );
               ClustersInCone::select( eg, m_coneSize, cps, mask );
            }
            break;

         case xAOD::Type::Muon:
            {
               const xAOD::Muon* mu =
                     static_cast< const xAOD::Muon* >( particle );
               ClustersInCone::select( mu, m_coneSize, cps, mask );
            }
            break;

         case xAOD::Type::Tau:
            break;

         default:
            ClustersInCone::select( particle, m_coneSize, cps, mask );
            break;
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode CaloClusterThinning::
   particleCluster( std::vector< bool >& mask,
                    const xAOD::IParticle* particle,
                    const xAOD::CaloClusterContainer* cps ) const {

      // Decide how to treat the particle:
      switch( particle->type() ) {

         case xAOD::Type::Electron:
         case xAOD::Type::Photon:
            {
               // Do a quick cast:
               const xAOD::Egamma* eg =
                     static_cast< const xAOD::Egamma* >( particle );
               // Check that the e/gamma has a cluster:
               if( ! eg->caloCluster() ) {
                  break;
               }
               // Check whether the e/gamma's cluster is in the container
               // specified:
               if( eg->caloCluster()->container() != cps ) {
                  ATH_MSG_DEBUG( "E/gamma cluster is not from the specified "
                                 "container" );
                  break;
               }
               // If it is, then update the mask:
               mask.at( eg->caloCluster()->index() ) = true;
            }
            break;

         case xAOD::Type::Muon:
            {
               // Do a quick cast:
               const xAOD::Muon* mu =
                     static_cast< const xAOD::Muon* >( particle );
               // Check that the muon has a cluster:
               if( ! mu->cluster() ) {
                  break;
               }
               // Check whether the muon's cluster is in the container
               // specified:
               if( mu->cluster()->container() != cps ) {
                  ATH_MSG_DEBUG( "Muon cluster is not from the specified "
                                 "container" );
                  break;
               }
               // If it is, then update the mask:
               mask.at( mu->cluster()->index() ) = true;
            }
            break;

         case xAOD::Type::Tau:
            {
               // Do a quick cast:
               const xAOD::TauJet* tau =
                     static_cast< const xAOD::TauJet* >( particle );
               // Skip taus with no jets:
               if( ! tau->jet() ) {
                  break;
               }
               // Get the 4-momentum of the tau:
               const TLorentzVector& tauP4 =
                     tau->p4( xAOD::TauJetParameters::DetectorAxis );
               // Iterate over all constituents of the associated jet:
               const xAOD::JetConstituentVector vec =
                     tau->jet()->getConstituents();
               for( const xAOD::JetConstituent* jc : vec ) {
                  // Get the 4-momentum of the constituent, and check whether
                  // it's "close enough" to the tau or not:
                  TLorentzVector cP4;
                  cP4.SetPtEtaPhiM( 1.0, jc->Eta(), jc->Phi(), 1.0 );
                  if( cP4.DeltaR( tauP4 ) > 0.2 ) {
                     continue;
                  }
                  // If it is, then make sure that it's a cluster coming from
                  // the expected container:
                  const xAOD::IParticle* rawC = jc->rawConstituent();
                  if( rawC->container() != cps ) {
                     ATH_MSG_DEBUG( "Tau cluster is not from the specified "
                                    "container" );
                     continue;
                  }
                  // If all is well, update the mask:
                  mask.at( rawC->index() ) = true;
               }
            }
            break;

         default:
            break;
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace DerivationFramework
