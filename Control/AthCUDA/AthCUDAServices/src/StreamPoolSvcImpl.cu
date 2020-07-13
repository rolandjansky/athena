// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "StreamPoolSvcImpl.h"

// AthCUDA include(s).
#include "AthCUDACore/Macros.cuh"
#include "AthCUDACore/StreamHolderHelpers.cuh"

// TBB include(s).
#include <tbb/concurrent_queue.h>

// System include(s).
#include <cassert>

namespace AthCUDA {

   class StreamPoolSvcImplData {

   public:
      /// Custom destructor, deleting the CUDA streams.
      ~StreamPoolSvcImplData() {
         while( ! m_streams.empty() ) {
            cudaStream_t stream = nullptr;
            m_streams.pop( stream );
            assert( stream != nullptr );
            CUDA_IGNORE( cudaStreamDestroy( stream ) );
         }
      }

      /// The concurrent pool of streams that @c StreamPoolSvcImpl manages
      tbb::concurrent_bounded_queue< cudaStream_t > m_streams;

   }; // struct StreamPoolSvcImplData

   StreamPoolSvcImpl::StreamPoolSvcImpl( unsigned int nStreams,
                                         IStreamPoolSvc& svc )
   : m_svc( &svc ), m_data( new StreamPoolSvcImplData() ) {

      // Allocate the requested number of streams.
      for( unsigned int i = 0; i < nStreams; ++i ) {
         cudaStream_t stream = nullptr;
         CUDA_EXP_CHECK( cudaStreamCreate( &stream ) );
         m_data->m_streams.push( stream );
      }
   }

   StreamPoolSvcImpl::StreamPoolSvcImpl( StreamPoolSvcImpl&& parent )
   : m_svc( parent.m_svc ), m_data( parent.m_data ) {

      parent.m_data = 0;
   }

   StreamPoolSvcImpl::~StreamPoolSvcImpl() {

      // Delete the data object.
      if( m_data != nullptr ) {
         delete m_data;
      }
   }

   StreamPoolSvcImpl& StreamPoolSvcImpl::operator=( StreamPoolSvcImpl&& rhs ) {

      // Check if anything needs to be done.
      if( this == &rhs ) {
         return *this;
      }

      // Perform the move.
      m_svc  = rhs.m_svc;
      m_data = rhs.m_data;
      rhs.m_data = nullptr;

      // Return this object.
      return *this;
   }

   StreamHolder StreamPoolSvcImpl::getAvailableStream() {

      // A security check.
      assert( m_data != nullptr );

      // Get the next available stream for the user.
      cudaStream_t stream = nullptr;
      m_data->m_streams.pop( stream );
      return StreamHolder( stream, m_svc );
   }

   void StreamPoolSvcImpl::yieldStream( StreamHolder& stream ) {

      // A security check.
      assert( m_data != nullptr );

      // Put the stream back into our queue.
      m_data->m_streams.push( getStream( stream ) );
      return;
   }

} // namespace AthCUDA
