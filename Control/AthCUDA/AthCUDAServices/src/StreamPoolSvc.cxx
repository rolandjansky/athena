//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "StreamPoolSvc.h"

// Project include(s).
#include "AthCUDACore/Info.h"

// System include(s).
#include <cassert>

namespace AthCUDA {

   StatusCode StreamPoolSvc::initialize() {

      // If there are no CUDA capable devices available, then don't try to
      // set up any streams.
      if( Info::instance().nDevices() == 0 ) {
         ATH_MSG_INFO( "CUDA device(s) not available. Not setting up "
                       "streams." );
         return StatusCode::SUCCESS;
      }

      // Create the implementation object.
      m_impl =
         std::make_unique< StreamPoolSvcImpl >( m_nStreams.value(), *this );

      // Tell the user what happened.
      ATH_MSG_INFO( "Allocated " << m_nStreams.value() << " stream(s)" );

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

   StatusCode StreamPoolSvc::finalize() {

      // Destroy the implementation object.
      m_impl.reset();

      // Tell the user what happened.
      ATH_MSG_INFO( "Destroyed all streams" );

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

   bool StreamPoolSvc::isEmpty() const {

      return ( m_nStreams.value() == 0 );
   }

   StreamHolder StreamPoolSvc::getAvailableStream() {

      // If we don't have streams available, return right away.
      if( m_nStreams.value() == 0 ) {
         return StreamHolder();
      }

      // A security check.
      assert( m_impl );

      // Get the next available stream for the user.
      return m_impl->getAvailableStream();
   }

   void StreamPoolSvc::yieldStream( StreamHolder& stream ) {

      // A security check.
      assert( m_impl );

      // Put the stream back into our queue.
      m_impl->yieldStream( stream );
      return;
   }

} // namespace AthCUDA
