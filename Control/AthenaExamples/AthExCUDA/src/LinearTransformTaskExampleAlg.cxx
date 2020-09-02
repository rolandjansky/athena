//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "LinearTransformTaskExampleAlg.h"
#include "CudaMultiplyTask.h"

// AthCUDA include(s).
#include "AthCUDAKernel/KernelStatus.h"

namespace AthCUDAExamples {

   StatusCode LinearTransformTaskExampleAlg::initialize() {

      // Retrieve the necessary component(s).
      ATH_CHECK( m_kernelRunnerSvc.retrieve() );

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

   StatusCode
   LinearTransformTaskExampleAlg::execute( const EventContext& ) const {

      // Create a dummy array variable that will be multiplied by some amount.
      static const std::size_t ARRAY_SIZE = 10000;
      std::vector< float > dummyArray;
      dummyArray.reserve( ARRAY_SIZE );
      static const float ARRAY_ELEMENT = 3.141592f;
      for( std::size_t i = 0; i < ARRAY_SIZE; ++i ) {
         dummyArray.push_back( ARRAY_ELEMENT );
      }

      // Create and launch the calculation using a task object.
      AthCUDA::KernelStatus status;
      static const float MULTIPLIER = 1.23f;
      auto task = make_CudaMultiplyTask( status, dummyArray, MULTIPLIER );
      ATH_CHECK( m_kernelRunnerSvc->execute( std::move( task ) ) );

      // Now wait for the task to finish.
      if( status.wait() != 0 ) {
         ATH_MSG_ERROR( "Something went wrong in the execution of the CUDA "
                        "task" );
         return StatusCode::FAILURE;
      }

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
