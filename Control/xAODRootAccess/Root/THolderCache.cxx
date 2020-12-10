// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

// Local include(s):
#include "THolderCache.h"
#include <mutex>

namespace xAOD {

   namespace Internal {

      // These mutex and lock types were chosen to provide good performance for
      // reading the "type map" and "reference map" variables very often, while
      // only writing to them sparingly.
      //
      // This is especially true for the "type map", which is not modified in
      // a typical job after the first event. The "reference map" is a bit
      // different, that does get modified throughout the whole job. It just
      // seemed easier to use the same mutex/lock types for both variable. But
      // The mutex/lock for the "reference map" could be re-visited if
      // performance tests show contention for it.

      /// Helper type definition
      typedef std::shared_lock< std::shared_timed_mutex > shared_lock_t;
      /// Helper type definition
      typedef std::unique_lock< std::shared_timed_mutex > unique_lock_t;

      THolderCache& THolderCache::instance() {

         static THolderCache cache;
         return cache;
      }

      std::pair< bool, ::TClass* >
      THolderCache::getClass( const std::type_info& ti ) const {

         // Get a "read lock":
         shared_lock_t lock( m_typeMapMutex );

         // Look for the type in the cache:
         auto itr = m_typeMap.find( &ti );
         if( itr != m_typeMap.end() ) {
            return std::pair< bool, ::TClass* >( true, itr->second );
         } else {
            return std::pair< bool, ::TClass* >( false, nullptr );
         }
      }

      void THolderCache::addClass( const std::type_info& ti, ::TClass* cl ) {

         // Get a "write lock":
         unique_lock_t lock( m_typeMapMutex );

         // Extend the map:
         m_typeMap[ &ti ] = cl;
      }

      int THolderCache::getRef( void* ptr ) const {

         // Get a "read lock":
         shared_lock_t lock( m_refMapMutex );

         // Get the reference count:
         auto itr = m_refMap.find( ptr );
         if( itr != m_refMap.end() ) {
            return itr->second;
         } else {
            return 0;
         }
      }

      int THolderCache::incRef( void* ptr ) {

         // Get a "write lock":
         unique_lock_t lock( m_refMapMutex );

         // Increment the reference count, and return the new value:
         return ++( m_refMap[ ptr ] );
      }

      int THolderCache::decRef( void* ptr ) {

         // Get a "write lock":
         unique_lock_t lock( m_refMapMutex );

         // Check if we know about this pointer:
         auto itr = m_refMap.find( ptr );
         if( itr == m_refMap.end() ) {
            return 0;
         }

         // Decrease the reference count, and remember the value:
         const int count = --( m_refMap[ ptr ] );

         // If the reference count went down to zero, let's forget about this
         // pointer:
         if( count == 0 ) {
            m_refMap.erase( itr );
         }

         // Return the new count:
         return count;
      }

      THolderCache::THolderCache()
         : m_typeMap(), m_refMap(), m_typeMapMutex(), m_refMapMutex() {

      }

   } // namespace Internal

} // namespace xAOD
