/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "xAODCore/tools/IOStats.h"

namespace xAOD {

   IOStats& IOStats::instance() {

      static IOStats obj;
      return obj;
   }

   ReadStats& IOStats::stats() {

      // Make sure that the thread specific object exists:
      if( ! m_ptr.get() ) {
         m_ptr.reset( new ReadStatsPtr() );
      }

      // If we already cached the pointer for this thread, then go no further:
      if( m_ptr->m_ptr ) {
         return *( m_ptr->m_ptr );
      }

      // If not, then acquire a lock, and set up the object now:
      std::unique_lock< std::mutex > lock( m_mutex );
      const std::thread::id id = std::this_thread::get_id();
      m_ptr->m_ptr = &( m_stats[ id ] );

      // And finally return the newly setup pointer:
      return *( m_ptr->m_ptr );
   }

   ReadStats IOStats::merged() const {

      // Get a lock on the map:
      std::unique_lock< std::mutex > lock( m_mutex );

      // Merge the objects from all the threads:
      ReadStats result;
      for( auto& pair : m_stats ) {
         result += pair.second;
      }

      // Return the merged object:
      return result;
   }

   IOStats::IOStats()
      : m_stats(), m_ptr(), m_mutex() {

   }

} // namespace xAOD
