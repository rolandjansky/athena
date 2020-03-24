/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// EDM include(s):
#include "xAODJet/JetContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

// Local include(s):
#include "DerivationFrameworkCalo/JetCaloClusterThinning.h"

namespace DerivationFramework {

   JetCaloClusterThinning::JetCaloClusterThinning( const std::string& type,
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
                       "StoreGate key of the jet container to use" );
      declareProperty( "TopoClCollectionSGKey", m_TopoClSGKey,
                       "StoreGate key of the \"main\" cluster container" );
      declareProperty( "AdditionalClustersKey"  , m_addClustersSGKey,
                       "Additional cluster containers to set up the same "
                       "filtering on. Assuming that they're shallow copies "
                       "of the main container." );
      declareProperty( "SelectionString", m_selectionString,
                       "Selection string for the e/gamma objects" );
      declareProperty( "ApplyAnd", m_and,
                       "Use IThinningSvc::Operator::And instead of "
                       "IThinningSvc::Operator::Or" );
   }

   StatusCode JetCaloClusterThinning::initialize() {

      // Decide which collections need to be checked for calo clusters
      ATH_MSG_VERBOSE( "initialize() ..." );
      if( m_TopoClSGKey == "" ) {
         ATH_MSG_FATAL( "No CalCaloTopoCluster collection provided for "
                        "thinning." );
         return StatusCode::FAILURE;
      }
      ATH_MSG_INFO( "Using " << m_TopoClSGKey
                    << " as the source collection for topo calo clusters" );

      if( m_sgKey == "" ) {
         ATH_MSG_FATAL( "No jet collection provided for thinning." );
         return StatusCode::FAILURE;
      }
      ATH_MSG_INFO( "Calo clusters associated with objects in " << m_sgKey
                    << " will be retained in this format with the rest being "
                       "thinned away" );

      if( m_addClustersSGKey.size() ) {
         ATH_MSG_INFO( "Thinning will also be set up for containers: "
                       << m_addClustersSGKey );
      }

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

   StatusCode JetCaloClusterThinning::finalize() {

      // Print some statistics:
      ATH_MSG_VERBOSE("finalize() ...");
      ATH_MSG_INFO( "Processed "<< m_ntotTopo << " topo clusters, of which "
                    << m_npassTopo << " were retained" );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode JetCaloClusterThinning::doThinning() const {

      // Retrieve CalCaloTopo collection if required
      const xAOD::CaloClusterContainer* importedTopoCaloCluster = nullptr;
      ATH_CHECK( evtStore()->retrieve( importedTopoCaloCluster,
                                       m_TopoClSGKey ) );

      // retrieve additional topocluster container(s)
      std::vector< const xAOD::CaloClusterContainer* > additionalTopoCaloCluster;
      for( const std::string& name : m_addClustersSGKey ) {
         const xAOD::CaloClusterContainer* tempTopoCaloCluster = nullptr;
         ATH_CHECK( evtStore()->retrieve( tempTopoCaloCluster, name ) );
         additionalTopoCaloCluster.push_back(tempTopoCaloCluster);
      }

      // Check that the event contains topoclusters
      const size_t nTopoClusters = importedTopoCaloCluster->size();
      if( nTopoClusters == 0 ) {
         return StatusCode::SUCCESS;
      }

      // Check that all cluster containers are of the same size:
      for( const xAOD::CaloClusterContainer* ccc : additionalTopoCaloCluster ) {
         if( ccc->size() != importedTopoCaloCluster->size() ) {
            ATH_MSG_FATAL( "One of the additional cluster containers is not "
                           "the right size" );
            return StatusCode::FAILURE;
         }
      }

      // Set up a mask with the same entries as the full topocluster
      // collection(s)
      std::vector< bool > topomask( nTopoClusters, false );
      m_ntotTopo += nTopoClusters;

      // Retrieve jet container
      const xAOD::JetContainer* importedJet = nullptr;
      ATH_CHECK( evtStore()->retrieve( importedJet, m_sgKey ) );
      const size_t nJets = importedJet->size();

      // Finish early if possible:
      if( nJets == 0 ) {
        // Tell the thinning service what to do:
        const IThinningSvc::Operator::Type opType =
           ( m_and ? IThinningSvc::Operator::And : IThinningSvc::Operator::Or );
        ATH_CHECK( m_thinningSvc->filter( *importedTopoCaloCluster, topomask,
                                        opType ) );
        for( const xAOD::CaloClusterContainer* ccc : additionalTopoCaloCluster ) {
           ATH_CHECK( m_thinningSvc->filter( *ccc, topomask, opType ) );
        }

        // Return gracefully:
        return StatusCode::SUCCESS;
      }

      // Select which jets to use:
      std::vector< const xAOD::Jet* > jetToCheck;
      if( m_parser ) {
         const std::vector< int > entries =
               m_parser->parser().evaluateAsVector();
         const size_t nEntries = entries.size();
         // check the sizes are compatible
         if( nJets!= nEntries ) {
            ATH_MSG_ERROR( "Sizes incompatible! Are you sure your selection "
                           "string used jets objects??" );
            return StatusCode::FAILURE;
         }
         // identify which jets to keep for the thinning check
         for( size_t i = 0; i < nJets; ++i ) {
            if( entries.at( i ) == 1 ) {
               jetToCheck.push_back( importedJet->at( i ) );
            }
         }
      } else {
         // If no selection was specified, use all the jet objects:
         for( const xAOD::Jet* jet : *importedJet ) {
            jetToCheck.push_back( jet );
         }
      }

      // Fill the mask:
      for( const xAOD::Jet* jet : jetToCheck ) {
         // Process all of its constituents:
         const auto& links = jet->constituentLinks();
         for( const auto& link : links ) {
            // Check that the link is valid:
            if( ! link.isValid() ) {
               continue;
            }
            // And that it points at *one of the* cluster containers:
            bool keyValid = false;
            if( link.dataID() == m_TopoClSGKey ) {
               keyValid = true;
            }
            for( const std::string& key : m_addClustersSGKey ) {
               if( link.dataID() == key ) {
                  keyValid = true;
                  break;
               }
            }
            if( ! keyValid ) {
               ATH_MSG_FATAL( "Jet constituent points at container (\""
                              << link.dataID() << "\") unknown to this tool" );
               return StatusCode::FAILURE;
            }
            // If it's okay, add it to the mask:
            topomask.at( link.index() ) = true;
         }
      }

      // Count up the mask contents
      for( bool bit : topomask ) {
         if( bit ) {
            ++m_npassTopo;
         }
      }

      // Tell the thinning service what to do:
      const IThinningSvc::Operator::Type opType =
         ( m_and ? IThinningSvc::Operator::And : IThinningSvc::Operator::Or );
      ATH_CHECK( m_thinningSvc->filter( *importedTopoCaloCluster, topomask,
                                        opType ) );
      for( const xAOD::CaloClusterContainer* ccc : additionalTopoCaloCluster ) {
         ATH_CHECK( m_thinningSvc->filter( *ccc, topomask, opType ) );
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace DerivationFramework

