// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODROOTACCESS_THOLDERCACHE_H
#define XAODROOTACCESS_THOLDERCACHE_H

// System include(s):
#include <map>
#include <shared_mutex>

// Forward declaration(s):
namespace std {
   class type_info;
}
class TClass;

namespace xAOD {

   namespace Internal {

      /// @short Singleton, thread-safe THolder cache
      ///
      /// The xAOD::THolder class needs to keep some global caches internally,
      /// due to the way it manages its own memory. This singleton helps doing
      /// that in a thread-safe way.
      ///
      /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
      ///
      class THolderCache {

      public:
         /// Singleton accessor
         static THolderCache& instance();

         /// Get the dictionary for a given type info
         ::TClass* getClass( const std::type_info& ti ) const;
         /// Add the dictionary for a given type
         void addClass( const std::type_info& ti, ::TClass* cl );

         /// Get the reference count of an object in memory
         int getRef( void* ptr ) const;
         /// Increment the reference count of an object in memory
         int incRef( void* ptr );
         /// Decrease the reference count of an object in memory
         int decRef( void* ptr );

      private:
         /// Hide the constructor of the class from the outside
         THolderCache();
         /// Delete the copy constructor
         THolderCache( const THolderCache& ) = delete;

         /// The type map
         std::map< const std::type_info*, TClass* > m_typeMap;
         /// The reference count map
         std::map< void*, int > m_refMap;

         /// Mutex for the type map
         mutable std::shared_timed_mutex m_typeMapMutex;
         /// Mutex for the reference count map
         mutable std::shared_timed_mutex m_refMapMutex;

      }; // class THolderCache

   } // namespace Internal

} // namespace xAOD

#endif // XAODROOTACCESS_THOLDERCACHE_H
