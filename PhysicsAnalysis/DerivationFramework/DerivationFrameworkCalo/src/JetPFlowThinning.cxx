/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// EDM include(s):
#include "xAODJet/JetContainer.h"
#include "xAODPFlow/PFOContainer.h"
#include "xAODPFlow/PFOAuxContainer.h"
#include "xAODPFlow/PFO.h"


// Local include(s):
#include "DerivationFrameworkCalo/JetPFlowThinning.h"

namespace DerivationFramework {

   JetPFlowThinning::JetPFlowThinning( const std::string& type,
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
      declareProperty( "PFOCollectionSGKey", m_PFOSGKey,
                       "StoreGate key of the \"main\" pfo container" );
      declareProperty( "AdditionalPFOKey"  , m_addPFOSGKey,
                       "Additional pfo containers to set up the same "
                       "filtering on. Assuming that they're shallow copies "
                       "of the main container." );
      declareProperty( "SelectionString", m_selectionString,
                       "Selection string for the e/gamma objects" );
      declareProperty( "ApplyAnd", m_and,
                       "Use IThinningSvc::Operator::And instead of "
                       "IThinningSvc::Operator::Or" );
   }

   StatusCode JetPFlowThinning::initialize() {

      // Decide which collections need to be checked for PFO
      ATH_MSG_VERBOSE( "initialize() ..." );
      if( m_PFOSGKey == "" ) {
         ATH_MSG_FATAL( "No PFO collection provided for "
                        "thinning." );
         return StatusCode::FAILURE;
      }
      ATH_MSG_INFO( "Using " << m_PFOSGKey
                    << " as the source collection for PFO" );

      if( m_sgKey == "" ) {
         ATH_MSG_FATAL( "No jet collection provided for thinning." );
         return StatusCode::FAILURE;
      }
      ATH_MSG_INFO( "PFO associated with objects in " << m_sgKey
                    << " will be retained in this format with the rest being "
                       "thinned away" );

      if( m_addPFOSGKey.size() ) {
         ATH_MSG_INFO( "Thinning will also be set up for containers: "
                       << m_addPFOSGKey );
      }

      // Set up the text-parsing machinery for selecting the jet directly
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

   StatusCode JetPFlowThinning::finalize() {

      // Print some statistics:
      ATH_MSG_VERBOSE("finalize() ...");

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode JetPFlowThinning::doThinning() const {

      // Retrieve PFO collection if required
      const xAOD::PFOContainer* importedPFONeutral = nullptr;
      ATH_CHECK( evtStore()->retrieve( importedPFONeutral,  m_PFOSGKey + "NeutralParticleFlowObjects") );

      const xAOD::PFOContainer* importedPFOCharged = nullptr;
      ATH_CHECK( evtStore()->retrieve( importedPFOCharged,  m_PFOSGKey+"ChargedParticleFlowObjects" ) );


      // retrieve additional PFO container(s)
      std::vector< const xAOD::PFOContainer* > additionalPFOCharged;
      std::vector< const xAOD::PFOContainer* > additionalPFONeutral;

      for( const std::string& name : m_addPFOSGKey ) {
         const xAOD::PFOContainer* tempPFOCharged = nullptr;
         ATH_CHECK( evtStore()->retrieve( tempPFOCharged, name+"ChargedParticleFlowObjects" ) );
         additionalPFOCharged.push_back(tempPFOCharged);

         const xAOD::PFOContainer* tempPFONeutral = nullptr;
         ATH_CHECK( evtStore()->retrieve( tempPFONeutral, name+"NeutralParticleFlowObjects" ) );
         additionalPFONeutral.push_back(tempPFONeutral);
      }

      // Check that the event contains PFO
      const size_t nPFONeutral = importedPFONeutral->size();
      const size_t nPFOCharged = importedPFOCharged->size() ;
      if( nPFOCharged + nPFONeutral == 0 ) {
         return StatusCode::SUCCESS;
      }


      // Set up a mask with the same entries as the full PFO collection(s)
      std::vector< bool > pfomaskNeutral( nPFONeutral, false );
      std::vector< bool > pfomaskCharged( nPFOCharged, false );

      // Retrieve jet container
      const xAOD::JetContainer* importedJet = nullptr;
      ATH_CHECK( evtStore()->retrieve( importedJet, m_sgKey ) );
      const size_t nJets = importedJet->size();

      // Finish early if possible:
      if( nJets == 0 ) {
        // Tell the thinning service what to do:
        const IThinningSvc::Operator::Type opType =
           ( m_and ? IThinningSvc::Operator::And : IThinningSvc::Operator::Or );


        ATH_CHECK( m_thinningSvc->filter( *importedPFONeutral, pfomaskNeutral, opType ) );
        ATH_CHECK( m_thinningSvc->filter( *importedPFOCharged, pfomaskCharged, opType ) );

        for( const xAOD::PFOContainer* cpfo : additionalPFONeutral ) {
           ATH_CHECK( m_thinningSvc->filter( *cpfo, pfomaskNeutral, opType ) );
        }
        for( const xAOD::PFOContainer* cpfo : additionalPFOCharged ) {
           ATH_CHECK( m_thinningSvc->filter( *cpfo, pfomaskCharged, opType ) );
        }

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

            bool isCharged = std::abs((dynamic_cast<const xAOD::PFO*>(*link))->charge()) > FLT_MIN;
            // If it's charged, add it to the charged mask
            if(isCharged) pfomaskCharged.at( link.index() ) = true;
            // Otherwise, add it to the neutral mask
            else pfomaskNeutral.at( link.index() ) = true;
         }
      }
 
      // Tell the thinning service what to do:
      const IThinningSvc::Operator::Type opType =
         ( m_and ? IThinningSvc::Operator::And : IThinningSvc::Operator::Or );


      ATH_CHECK( m_thinningSvc->filter( *importedPFONeutral, pfomaskNeutral, opType ) );
      ATH_CHECK( m_thinningSvc->filter( *importedPFOCharged, pfomaskCharged, opType ) );

      for( const xAOD::PFOContainer* cpfo : additionalPFONeutral ) {
         ATH_CHECK( m_thinningSvc->filter( *cpfo, pfomaskNeutral, opType ) );
      }
      for( const xAOD::PFOContainer* cpfo : additionalPFOCharged ) {
         ATH_CHECK( m_thinningSvc->filter( *cpfo, pfomaskCharged, opType ) );
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace DerivationFramework
