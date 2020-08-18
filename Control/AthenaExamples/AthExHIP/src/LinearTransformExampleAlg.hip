// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "LinearTransformExampleAlg.h"

// HIP include(s).
#include <hip/hip_runtime.h>

// System include(s).
#include <limits>

/// Helper macro for checking the return codes from HIP function calls.
#define HIP_CHECK(EXP)                                                         \
   do {                                                                        \
      hipError_t errorCode = EXP;                                              \
      if( errorCode != hipSuccess ) {                                          \
         REPORT_MESSAGE( MSG::FATAL )                                          \
            << "HIP call returned: " << hipGetErrorName( errorCode )           \
            << ", " << hipGetErrorString( errorCode );                         \
         return StatusCode::FAILURE;                                           \
      }                                                                        \
   } while( false )

namespace {
   /// Linear transformation kernel
   __global__
   void hipLinearTransform( std::size_t size, float* data, float a, float b ) {

      // Get the current index.
      const std::size_t index = hipBlockIdx_x * hipBlockDim_x + hipThreadIdx_x;
      if( index >= size ) {
         return;
      }

      // Perform the linear transformation.
      data[ index ] = a * data[ index ] + b;
      return;
   }
} // private namespace

namespace AthHIPExamples {

   StatusCode LinearTransformExampleAlg::initialize() {

      // Print a bit of information about the selected device.
      hipDeviceProp_t prop;
      HIP_CHECK( hipGetDeviceProperties( &prop, m_hipDevice.value() ) );
      ATH_MSG_INFO( "Will be using device:" );
      ATH_MSG_INFO( "  - name                = " << prop.name );
      ATH_MSG_INFO( "  - totalGlobalMem      = " << prop.totalGlobalMem );
      ATH_MSG_INFO( "  - maxThreadsPerBlock  = " << prop.maxThreadsPerBlock );
      ATH_MSG_INFO( "  - multiProcessorCount = " << prop.multiProcessorCount );

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

   StatusCode LinearTransformExampleAlg::execute( const EventContext& ) const {

      // Select the requested device.
      HIP_CHECK( hipSetDevice( m_hipDevice.value() ) );

      // Create a dummy array variable that will be linearly transformed.
      static constexpr float ARRAY_ELEMENT = 3.141592f;
      std::vector< float > dummyArray( m_arraySize.value(), ARRAY_ELEMENT );

      // Allocate memory on the device for this dummy array.
      float* deviceData = nullptr;
      const std::size_t dataSize = m_arraySize.value() * sizeof( float );
      HIP_CHECK( hipMalloc( &deviceData, dataSize ) );

      // Copy the contents of the dummy array to the device.
      HIP_CHECK( hipMemcpy( deviceData, dummyArray.data(), dataSize,
                            hipMemcpyHostToDevice ) );

      // Launch the linear transformation kernel.
      static constexpr int blockSize = 256;
      const int numBlocks = ( dummyArray.size() + blockSize - 1 ) / blockSize;
      static constexpr int sharedMem = 0;
      static constexpr hipStream_t stream = nullptr;
      hipLaunchKernelGGL( hipLinearTransform, numBlocks, blockSize, sharedMem,
                          stream, dummyArray.size(), deviceData,
                          m_multiplier.value(), m_shift.value() );
      HIP_CHECK( hipGetLastError() );
      HIP_CHECK( hipDeviceSynchronize() );

      // Copy the memory back from the device.
      HIP_CHECK( hipMemcpy( dummyArray.data(), deviceData, dataSize,
                            hipMemcpyDeviceToHost ) );
      // Free the memory on the device.
      HIP_CHECK( hipFree( deviceData ) );

      // Validate the contents of the transformed array.
      const float expectedResult =
         m_multiplier.value() * ARRAY_ELEMENT + m_shift.value();
      static constexpr float allowedDeviation =
         std::numeric_limits< float >::epsilon() * 4;
      for( float element : dummyArray ) {
         if( std::abs( element - expectedResult ) > allowedDeviation ) {
            REPORT_MESSAGE( MSG::FATAL )
               << "The HIP linear transformation failed";
            return StatusCode::FAILURE;
         }
      }

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

} // namespace AthHIPExamples
