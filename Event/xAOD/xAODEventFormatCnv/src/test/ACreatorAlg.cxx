//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "ACreatorAlg.h"

// EDM include(s).
#include "xAODEventFormatCnv/test/A.h"
#include "xAODEventFormatCnv/test/AVec.h"
#include "xAODEventFormatCnv/test/AAuxContainer.h"

// Framework include(s).
#include "StoreGate/WriteHandle.h"

// System include(s).
#include <cmath>
#include <memory>

// Declare the type to Gaudi.
DECLARE_COMPONENT( xAODMakerTest::ACreatorAlg )

namespace xAODMakerTest {

   StatusCode ACreatorAlg::initialize() {

      // Initialise the data handle(s).
      ATH_CHECK( m_outputKey.initialize() );

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

   StatusCode ACreatorAlg::execute( const EventContext& ctx ) const {

      // Create a dummy electron container.
      auto vec = std::make_unique< AVec >();
      auto aux = std::make_unique< AAuxContainer >();
      vec->setStore(aux.get());

      // Fill it with some nonsensical content.
      for( int i = 0; i < 10; ++i ) {
         vec->push_back( std::make_unique< A >() );
         auto a = vec->back();
         a->aFloat( 1.0f );
         a->anInt( 1 );
         a->pVInt( std::vector< int >(10, 1) );
         a->pVFloat( std::vector< float >(10, 1.0f) );
      }

      // Record it into the event store.
      SG::WriteHandle< AVec > wh( m_outputKey, ctx );
      ATH_CHECK( wh.record( std::move( vec ), std::move( aux ) ) );

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

} // namespace xAODMakerTest
