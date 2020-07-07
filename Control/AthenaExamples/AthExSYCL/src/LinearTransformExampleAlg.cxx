//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "LinearTransformExampleAlg.h"

// SYCL include(s).
#include <CL/sycl.hpp>

// System include(s).
#include <cmath>

namespace AthSYCL {

   StatusCode
   LinearTransformExampleAlg::execute( const EventContext& ) const {

      // Create a vector array that would be manipulated.
      std::vector< float > dummyArray;
      static const std::size_t ARRAY_SIZE = 10000;
      dummyArray.reserve( ARRAY_SIZE );
      static const float ARRAY_ELEMENT = 3.141592f;
      for( std::size_t i = 0; i < ARRAY_SIZE; ++i ) {
         dummyArray.push_back( ARRAY_ELEMENT );
      }

      // Set up a SYCL buffer on top of this STL object.
      cl::sycl::buffer< cl::sycl::cl_float > buffer( dummyArray.begin(),
                                                     dummyArray.end() );

      // Set up the SYCL queue.
      cl::sycl::queue queue( m_deviceSelector );
      cl::sycl::range< 1 > workItems( buffer.get_count() );

#ifndef TRISYCL_CL_SYCL_HPP
      // Let the user know what device the calculation is running on.
      const cl::sycl::device& device = queue.get_device();
      ATH_MSG_DEBUG( "Using device "
                     << device.get_info< cl::sycl::info::device::name >()
                     << " ("
                     << device.get_info< cl::sycl::info::device::version >()
                     << ")" );
#endif // not TRISYCL_CL_SYCL_HPP

      // Multiply these values using SYCL.
      static const float MULTIPLIER = 1.23f;
      queue.submit( [&]( cl::sycl::handler& handler ) {
            auto acc =
               buffer.get_access< cl::sycl::access::mode::read_write >( handler );
            handler.parallel_for< class SYCLMultiply >( workItems,
               [=]( cl::sycl::id< 1 > id ) {
                  acc[ id ] *= MULTIPLIER;
               } );
         } );

      // Copy the results back to the host. Note that the execution will wait
      // here for a potentially offloaded operation to finish first. Put the
      // reader accessor into a scope, to make sure it is deleted after it is
      // not needed anymore.
      {
         auto bReader =
            buffer.get_access< cl::sycl::access::mode::read >();
         for( std::size_t i = 0; i < buffer.get_count(); ++i ) {
            dummyArray[ i ] = bReader[ i ];
         }
      }

      // Check those results.
      static const float EXPECTED_RESULT = ARRAY_ELEMENT * MULTIPLIER;
      for( std::size_t i = 0; i < ARRAY_SIZE; ++i ) {
         if( std::abs( dummyArray[ i ] - EXPECTED_RESULT ) > 0.001 ) {
            ATH_MSG_ERROR( "The SYCL transformation failed to run" );
            return StatusCode::FAILURE;
         }
      }

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

} // namespace AthSYCL
