//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "LinearTransformStandaloneExampleAlg.h"
#include "cudaMultiply.h"

// System include(s).
#include <cmath>
#include <vector>

namespace AthCUDAExamples {

   StatusCode
   LinearTransformStandaloneExampleAlg::execute( const EventContext& ) const {

      // Create a dummy array variable that will be multiplied by some amount.
      static const std::size_t ARRAY_SIZE = 10000;
      std::vector< float > dummyArray;
      dummyArray.reserve( ARRAY_SIZE );
      static const float ARRAY_ELEMENT = 3.141592f;
      for( std::size_t i = 0; i < ARRAY_SIZE; ++i ) {
         dummyArray.push_back( ARRAY_ELEMENT );
      }

      // Call on a function, which would synchronously make some modification
      // to this vector.
      static const float MULTIPLIER = 1.23f;
      cudaMultiply( dummyArray, MULTIPLIER );

      // Check if the operation succeeded.
      static const float EXPECTED_RESULT = ARRAY_ELEMENT * MULTIPLIER;
      for( std::size_t i = 0; i < ARRAY_SIZE; ++i ) {
         if( std::abs( dummyArray[ i ] - EXPECTED_RESULT ) > 0.001 ) {
            ATH_MSG_ERROR( "The CUDA transformation failed to run" );
            return StatusCode::FAILURE;
         }
      }

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

} // namespace AthCUDAExamples
