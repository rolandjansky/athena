// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "HIPFunctions.h"

// HIP include(s).
#include <hip/hip_runtime.h>

// System include(s).
#include <iostream>
#include <sstream>
#include <stdexcept>

/// Helper macro for checking the return codes from HIP function calls.
#define HIP_CHECK(EXP)                                                         \
   do {                                                                        \
      hipError_t errorCode = EXP;                                              \
      if( errorCode != hipSuccess ) {                                          \
         std::ostringstream msg;                                               \
         msg << __FILE__ << ":" << __LINE__ << " HIP call returned: "          \
             << hipGetErrorName( errorCode ) << ", "                           \
             << hipGetErrorString( errorCode );                                \
         std::cerr << msg.str() << std::endl;                                  \
         throw std::runtime_error( msg.str() );                                \
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

   void printHipDeviceInfo( int deviceId, std::ostream& out ) {

      // Collect the device's properties.
      hipDeviceProp_t prop;
      HIP_CHECK( hipGetDeviceProperties( &prop, deviceId ) );

      // Print some of the properties to the output stream.
      out << "  - name                = " << prop.name << "\n"
          << "  - totalGlobalMem      = " << prop.totalGlobalMem << "\n"
          << "  - maxThreadsPerBlock  = " << prop.maxThreadsPerBlock << "\n"
          << "  - multiProcessorCount = " << prop.multiProcessorCount;
      return;
   }

   void linearTransform( int deviceId, std::vector< float >& data, float a,
                         float b ) {

      // Select the requested device.
      HIP_CHECK( hipSetDevice( deviceId ) );

      // Allocate memory on the device for this array.
      float* deviceData = nullptr;
      const std::size_t dataSize = data.size() * sizeof( float );
      HIP_CHECK( hipMalloc( &deviceData, dataSize ) );

      // Copy the contents of the dummy array to the device.
      HIP_CHECK( hipMemcpy( deviceData, data.data(), dataSize,
                            hipMemcpyHostToDevice ) );

      // Launch the linear transformation kernel.
      static constexpr int blockSize = 256;
      const int numBlocks = ( data.size() + blockSize - 1 ) / blockSize;
      static constexpr int sharedMem = 0;
      static constexpr hipStream_t stream = nullptr;
      hipLaunchKernelGGL( hipLinearTransform, numBlocks, blockSize, sharedMem,
                          stream, data.size(), deviceData, a, b );
      HIP_CHECK( hipGetLastError() );
      HIP_CHECK( hipDeviceSynchronize() );

      // Copy the memory back from the device.
      HIP_CHECK( hipMemcpy( data.data(), deviceData, dataSize,
                            hipMemcpyDeviceToHost ) );
      // Free the memory on the device.
      HIP_CHECK( hipFree( deviceData ) );

      return;
   }

} // namespace AthHIPExamples
