// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "AthCUDACore/Memory.cuh"

// System include(s).
#undef NDEBUG
#include <cassert>
#include <cmath>
#include <unistd.h>

int main() {

   {

      // Allocate some test arrays.
      auto array1 = AthCUDA::make_managed_array< float >( 500 );
      auto array2 = AthCUDA::make_managed_array< int >( 100 );
      auto array3 = AthCUDA::make_managed_array< double >( 1000 );

      // Make sure that they can be filled.
      for( int i = 0; i < 10; ++i ) {
         array1.get()[ i ] = 1.2f;
         array2.get()[ i ] = 123;
         array3.get()[ i ] = M_PI;
      }

   }
   // Wait for a moment, so that TBB would have time to schedule the deletion
   // of these arrays. Otherwise CUDA runtime may have already unloaded itself
   // by the time the cudaFree(...) call is made.
   sleep( 1 );

   // Allocate some (device) arrays.
   AthCUDA::array< int > array4( 100 );
   AthCUDA::array< float > array5;
   array5.resize( 200 );

   // Make sure that we can write to these.
   for( int i = 0; i < 10; ++i ) {
      array4[ i ] = i;
      array5[ i ] = i * M_PI;
   }

   // Check that resizing works as intended.
   array5.resize( 100 );
   array5.resize( 500 );
   for( int i = 0; i < 10; ++i ) {
      assert( std::abs( array5[ i ] - i * M_PI ) < 0.0001 );
   }

   // Return gracefully.
   return 0;
}
