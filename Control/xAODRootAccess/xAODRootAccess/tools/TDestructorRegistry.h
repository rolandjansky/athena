// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TDestructorRegistry.h 599851 2014-06-02 12:32:51Z krasznaa $
#ifndef XAODROOTACCESS_TOOLS_TDESTRUCTORREGISTRY_H
#define XAODROOTACCESS_TOOLS_TDESTRUCTORREGISTRY_H

// System include(s):
#include <map>
#include <shared_mutex>

// Forward declaration(s):
namespace std {
   class type_info;
}

namespace xAOD {

   // Forward declaration(s):
   class TVirtualDestructor;

   /// Application-wide registry of destructor objects
   ///
   /// This registry is used to keep track of how to destruct objects
   /// that are kept in the transient store, and don't have a ROOT dictionary
   /// available for them.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 599851 $
   /// $Date: 2014-06-02 14:32:51 +0200 (Mon, 02 Jun 2014) $
   ///
   class TDestructorRegistry {

   public:
      /// Function accessing the singleton instance of this type
      static TDestructorRegistry& instance();

      /// Get the destructor for a given type
      const TVirtualDestructor* get( const std::type_info& ti ) const;
      /// Add a new destructor object
      template< class T >
      void add();

   private:
      /// Hide the constructor of the type
      TDestructorRegistry();
      /// Hide the copy-constructor
      TDestructorRegistry( const TDestructorRegistry& ) = delete;

      /// Type used internally to clean up memory at the end of the process
      class TDestructorHolder {
      public:
         /// Constructor with a new TVirtualDestructor pointer
         TDestructorHolder( TVirtualDestructor* d = 0 );
         /// Destructor
         ~TDestructorHolder();

         /// The managed object
         TVirtualDestructor* m_destructor;
      };

      /// Type of the internal map
      typedef std::map< const std::type_info*, TDestructorHolder > Map_t;
      /// Internal map of known destructor objects
      Map_t m_types;
      /// Mutex for the destructor map
      mutable std::shared_timed_mutex m_mutex;

   }; // class TDestructorRegistry

} // namespace xAOD

// Include the template implementation:
#include "TDestructorRegistry.icc"

#endif // XAODROOTACCESS_TOOLS_TDESTRUCTORREGISTRY_H
