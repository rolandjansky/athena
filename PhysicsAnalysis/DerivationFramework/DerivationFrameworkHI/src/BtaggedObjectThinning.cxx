/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <vector>

#include "GaudiKernel/System.h"

// EDM include(s):
#include "xAODBase/IParticleContainer.h"
#include "xAODBTagging/BTaggingContainer.h"

// Local include(s):
#include "DerivationFrameworkHI/BtaggedObjectThinning.h"

namespace DerivationFramework {

   BtaggedObjectThinning::BtaggedObjectThinning( const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent )
      : AthAlgTool( type, name, parent ),
        m_thinningSvc( "ThinningSvc", name ) {

      // Declare the tool's interface to the framework:
      declareInterface< DerivationFramework::IThinningTool >( this );

      // Set up the tool's properties:
      declareProperty( "ThinningService", m_thinningSvc,
                       "The thinning service to use" );
      declareProperty( "SelectionString", m_selectionString,
                       "Selection string for the e/gamma objects" );
      declareProperty( "ApplyAnd", m_and,
                       "Use IThinningSvc::Operator::And instead of "
                       "IThinningSvc::Operator::Or" );
      declareProperty( "ContainerName", m_sgKey,
                       "StoreGate key of the container to thin" );
   }

   StatusCode BtaggedObjectThinning::initialize() {

      ATH_MSG_VERBOSE( "initialize() ..." );
      if( m_selectionString == "" ) {
         ATH_MSG_FATAL( "No selection string provided!" );
         return StatusCode::FAILURE;
      }
      ATH_MSG_INFO( "Selection string: " << m_selectionString );

      // Set up the text-parsing machinery for thinning the tracks directly
      // according to user cuts
      m_parser.reset( new ExpressionParserHelper( evtStore() ) );
      if( ! m_parser->parser().loadExpression( m_selectionString ) ) {
         ATH_MSG_FATAL( "Failed to interpret expression: "
                        << m_selectionString );
         return StatusCode::FAILURE;
      }

      // Check that the user specified a container key:
      if( m_sgKey == "" ) {
         ATH_MSG_FATAL( "No object collection provided for thinning." );
         return StatusCode::FAILURE;
      }
      ATH_MSG_INFO( "Using " << m_sgKey << " as the source collection" );

      // Access the thinning service:
      ATH_CHECK( m_thinningSvc.retrieve() );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode BtaggedObjectThinning::finalize() {

      // Print some statistics:
      ATH_MSG_VERBOSE("finalize() ...");
      ATH_MSG_INFO( "Processed " << m_ntot << " objects, "<< m_npass
                    << " were retained" );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode BtaggedObjectThinning::doThinning() const {

      // Retrieve main object collection
      const xAOD::BTaggingContainer* particles = nullptr;
      ATH_CHECK( evtStore()->retrieve( particles, m_sgKey ) );

      // Check the event contains objects
      const size_t nObjects = particles->size();
      if( nObjects == 0 ) {
         return StatusCode::SUCCESS;
      }

      // Set up a mask with the same entries as the full collection
      std::vector< bool > mask( nObjects, false );
      m_ntot += nObjects;

      // Execute the text parser and update the mask
      const std::vector< int > entries = m_parser->parser().evaluateAsVector();
      const size_t nEntries = entries.size();
      // check the sizes are compatible
      if( nObjects != nEntries ) {
         ATH_MSG_ERROR( "Sizes incompatible! Are you sure your selection "
                        "string used the same collection as the input "
                        "collection??" );
         return StatusCode::FAILURE;
      }
      // set mask
      for( size_t i = 0; i < nObjects; ++i ) {
         if( entries.at( i ) == 1 ) {
            mask.at( i ) = true;
         }
      }

      // Count the mask
      for( bool bit : mask ) {
         if( bit ) {
            ++m_npass;
         }
      }

      // Tell the thinning service what to do:
      const IThinningSvc::Operator::Type opType =
         ( m_and ? IThinningSvc::Operator::And : IThinningSvc::Operator::Or );
      ATH_CHECK( m_thinningSvc->filter( *particles, mask, opType ) );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace DerivationFramework
