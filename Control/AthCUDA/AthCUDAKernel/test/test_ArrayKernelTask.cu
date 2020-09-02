// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "AthCUDAKernel/ArrayKernelTask.cuh"

// AthCUDA include(s).
#include "AthCUDAInterfaces/StreamHolder.h"
#include "AthCUDACore/Info.h"
#include "AthCUDACore/Macros.cuh"

// System include(s).
#include <array>
#undef NDEBUG
#include <cassert>
#include <cmath>
#include <unistd.h>

/// Functor processing one variable
class Functor1 {
public:
   ATHCUDA_HOST_AND_DEVICE
   void operator()( std::size_t i, float* array1 ) {

      array1[ i ] *= 1.2f;
   }
}; // class Functor1

/// Functor processing two variables
class Functor2 {
public:
   ATHCUDA_HOST_AND_DEVICE
   void operator()( std::size_t i, float* array1, float multiplier,
                    double* array2 ) {

      array2[ i ] *= multiplier * array1[ i ];
   }
}; // class Functor2

/// Helper function initialising array elements
template< typename T, std::size_t ASIZE >
void initializeArray( std::array< T, ASIZE >& array );

int main() {

   // Make some arrays on the host.
   static const float MULTIPLIER = 1.34f;
   static const std::size_t ARRAY_SIZES = 100;
   std::array< float, ARRAY_SIZES > array1;
   std::array< double, ARRAY_SIZES > array2;

   // Allocate a CUDA stream for the test.
   cudaStream_t stream = nullptr;
   if( AthCUDA::Info::instance().nDevices() > 0 ) {
      CUDA_EXP_CHECK( cudaStreamCreate( &stream ) );
   }
   AthCUDA::StreamHolder sholder( stream, nullptr );

   // Test a kernel with just 1 array.
   AthCUDA::KernelStatus dummy;
   initializeArray( array1 );
   {
      auto kernel1 =
         AthCUDA::make_ArrayKernelTask< Functor1 >( dummy, ARRAY_SIZES,
                                                    array1.data() );
      kernel1->execute( sholder );

      // Wait for the task to finish.
      if( stream ) {
         CUDA_EXP_CHECK( cudaDeviceSynchronize() );
      }
      kernel1->finished( 0, AthCUDA::IKernelTask::Asynchronous );
   }

   // Check that it produced the expected results.
   float maxError1 = 0;
   for( std::size_t i = 0; i < ARRAY_SIZES; ++i ) {
      maxError1 =
         std::max( maxError1,
                   std::abs( array1[ i ] -
                             static_cast< float >( i * 1.2f * 1.2f ) ) );
   }
   std::cout << "maxError1 = " << maxError1 << std::endl;
   assert( maxError1 < 0.001 );

   // Test a kernel with 2 arrays.
   initializeArray( array1 );
   initializeArray( array2 );
   {
      auto kernel2 =
         AthCUDA::make_ArrayKernelTask< Functor2 >( dummy, ARRAY_SIZES,
                                                    array1.data(), MULTIPLIER,
                                                    array2.data() );
      kernel2->execute( sholder );

      // Wait for the task to finish, and then destroy the CUDA stream.
      if( stream ) {
         CUDA_EXP_CHECK( cudaDeviceSynchronize() );
         CUDA_EXP_CHECK( cudaStreamDestroy( stream ) );
      }
      kernel2->finished( 0, AthCUDA::IKernelTask::Asynchronous );

   }

   // Check that it produced the expected results.
   double maxError2 = 0;
   for( std::size_t i = 0; i < ARRAY_SIZES; ++i ) {
      maxError2 =
         std::max( maxError2,
                   std::abs( array2[ i ] -
                             i * 1.2f * MULTIPLIER *
                             static_cast< double >( array1[ i ] ) ) );
   }
   std::cout << "maxError2 = " << maxError2 << std::endl;
   assert( maxError2 < 0.001 );

   // Wait for a moment, for all CUDA memory to be deleted, before terminating
   // the application.
   sleep( 1 );

   // Return gracefully.
   return 0;
}

template< typename T, std::size_t ASIZE >
void initializeArray( std::array< T, ASIZE >& array ) {

   for( std::size_t i = 0; i < ASIZE; ++i ) {
      array[ i ] = i * 1.2f;
   }
   return;
}
