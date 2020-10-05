//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "cudaMultiply.h"

// CUDA include(s).
#ifdef __CUDACC__
#   include <cuda.h>
#endif // __CUDACC__

// System include(s).
#include <iostream>

#ifdef __CUDACC__

/// Simple macro to run CUDA commands with
#define CUDA_CHECK( EXP )                                                     \
   do {                                                                       \
      const cudaError_t ce = EXP;                                             \
      if( ce != cudaSuccess ) {                                               \
         std::cerr << "Failed to execute: " << #EXP << std::endl;             \
         std::cerr << "Reason: " << cudaGetErrorString( ce ) << std::endl;    \
         return;                                                              \
      }                                                                       \
   } while( false )

namespace AthCUDAExamples {

   /// Very simple kernel performing a multiplication on an array.
   __global__
   void cudaMultiplyKernel( int n, float* array, float multiplier ) {

      const int index = blockIdx.x * blockDim.x + threadIdx.x;
      if( index >= n ) {
         return;
      }

      array[ index ] *= multiplier;
      return;
   }

   /// GPU implementation of @c cudaMultiply
   void cudaMultiply( std::vector< float >& array, float multiplier ) {

      // If no CUDA device is available, complain.
      int nCudaDevices = 0;
      CUDA_CHECK( cudaGetDeviceCount( &nCudaDevices ) );
      if( nCudaDevices == 0 ) {
         return;
      }

      // Allocate the array on the/a device, and copy the host array's content
      // to the device.
      float* deviceArray = nullptr;
      CUDA_CHECK( cudaMalloc( &deviceArray, sizeof( float ) * array.size() ) );
      CUDA_CHECK( cudaMemcpy( deviceArray, array.data(),
                              sizeof( float ) * array.size(),
                              cudaMemcpyHostToDevice ) );

      // Run the kernel.
      static const int blockSize = 256;
      const int numBlocks = ( array.size() + blockSize - 1 ) / blockSize;
      cudaMultiplyKernel<<< numBlocks, blockSize >>>( array.size(),
                                                      deviceArray,
                                                      multiplier );
      CUDA_CHECK( cudaDeviceSynchronize() );

      // Copy the array back to the host's memory.
      CUDA_CHECK( cudaMemcpy( array.data(), deviceArray,
                              sizeof( float ) * array.size(),
                              cudaMemcpyDeviceToHost ) );

      // Free the memory on the device.
      CUDA_CHECK( cudaFree( deviceArray ) );
      return;
   }

} // namespace AthCUDAExamples

#else

namespace AthCUDAExamples {

   /// CPU implementation of @c cudaMultiply
   void cudaMultiply( std::vector< float >& array, float multiplier ) {

      for( float& element : array ) {
         element *= multiplier;
      }
   }

} // namespace AthCUDAExamples

#endif // __CUDACC__
