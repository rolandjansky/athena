/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "xAODCore/tools/IOStats.h"
#include "CxxUtils/checker_macros.h"

namespace xAOD {

   IOStats& IOStats::instance() {

      static IOStats obj ATLAS_THREAD_SAFE;
      return obj;
   }

   ReadStats& IOStats::stats() {

      // Try to access a cached pointer:
      ReadStats* stats = m_ptr.get();

      // If a pointer is not cached yet, do so now:
      if( ! stats ) {
         const std::thread::id id = std::this_thread::get_id();
         std::lock_guard< std::mutex > lock( m_mutex );
         stats = &( m_stats[ id ] );
         m_ptr.reset( stats );
      }

      // Return the (now) cached object:
      return *stats;
   }

   ReadStats IOStats::merged() const {

      // Get a lock on the map:
      std::lock_guard< std::mutex > lock( m_mutex );

      // Merge the objects from all the threads:
      ReadStats result;
      for( auto& pair : m_stats ) {
         result += pair.second;
      }

      // Return the merged object:
      return result;
   }

   IOStats::IOStats()
      : m_stats(),
        // Make sure that the thread specific pointer doesn't try to delete
        // the object it points to when the thread ends:
        m_ptr( []( ReadStats* ){} ),
        m_mutex() {

   }

} // namespace xAOD
