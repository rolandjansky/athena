//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "AthCUDAInterfaces/StreamHolder.h"
#include "AthCUDAInterfaces/IStreamPoolSvc.h"

namespace AthCUDA {

   StreamHolder::StreamHolder()
   : m_stream( nullptr ), m_pool( nullptr ) {

   }

   StreamHolder::StreamHolder( void* stream, IStreamPoolSvc* pool )
   : m_stream( stream ), m_pool( pool ) {

   }

   StreamHolder::StreamHolder( StreamHolder&& parent )
   : m_stream( parent.m_stream ), m_pool( parent.m_pool ) {

      parent.m_stream = nullptr;
      parent.m_pool = nullptr;
   }

   StreamHolder::~StreamHolder() {

      if( ( m_stream != nullptr ) && ( m_pool != nullptr ) ) {
         m_pool->yieldStream( *this );
      }
   }

   StreamHolder& StreamHolder::operator=( StreamHolder&& rhs ) {

      // Check if anything needs to be done.
      if( this == &rhs ) {
         return *this;
      }

      // Perform the move.
      m_stream = rhs.m_stream;
      rhs.m_stream = nullptr;
      m_pool = rhs.m_pool;
      rhs.m_pool = nullptr;
      return *this;
   }

   StreamHolder::operator bool() const {

      return ( m_stream != nullptr );
   }

} // namespace AthCUDA
