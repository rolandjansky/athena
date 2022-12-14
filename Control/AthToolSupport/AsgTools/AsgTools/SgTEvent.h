/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ASGTOOLS_SGTEVENT_H
#define ASGTOOLS_SGTEVENT_H

// System include(s):
#include <memory>
#include <string>
#include <vector>

// Local include(s):
#include "AsgMessaging/StatusCode.h"

// Complain if we try using this header in Athena:
#if !defined(XAOD_STANDALONE) && !defined(__CPPCHECK__)
#   error( "This header should only be used for standalone ASG" )
#endif

// Forward declaration(s):
namespace xAOD {
   class TEvent;
   class TStore;
} // namespace xAOD

namespace asg {

   /// Wrapper for TEvent to make it look like StoreGate.
   ///
   /// In order to be able to interact with xAOD::TEvent using the same sort
   /// of code that we use to interact with StoreGateSvc in Athena, we wrap
   /// it into such an object.
   ///
   /// This class also takes care of placing transient objects into
   /// xAOD::TStore.
   ///
   /// @author David Adams <dladams@bnl.gov>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 669898 $
   /// $Date: 2015-05-26 15:49:13 +0200 (Tue, 26 May 2015) $
   ///
   class SgTEvent {

   public:
      /// Constructor specifying concrete TEvent and TStore objects
      SgTEvent( xAOD::TEvent* pevm = 0, xAOD::TStore* ptds = 0 );

      /// Return the underlying event manager.
      xAOD::TEvent* event() const;

      /// Return the underlying transient data store.
      xAOD::TStore* tds() const;

      /// @name Functions providing access to the persistent and transient data
      /// @{

      /// Check if an object is available for constant access
      template< typename T >
      bool contains( const std::string& name ) const;
      /// Check if an object is available for non-constant access
      template< typename T >
      bool transientContains( const std::string& name ) const;

      /// Function retrieving a constant or non-constant object
      template< typename T >
      T* retrieve( const std::string& name ) const;

      /// Retrieve a container from memory
      template< typename T >
      StatusCode retrieve( T*& pobj, const std::string& name );
      /// Retrieve a container from the input file or the memory
      template< typename T >
      StatusCode retrieve( const T*& pobj, const std::string& name ) const;

      /// Record an object/container using a smart pointer for ownership
      template< typename T >
      StatusCode record( std::unique_ptr<T> pobj, const std::string& cname );

      /// Record an object/container
      template< typename T >
      StatusCode record( T* pobj, const std::string& cname );

      /// Overwrite an object/container in the transient store
      template< typename T >
      StatusCode overwrite( T* pobj, const std::string& name,
                            bool allowMods, bool resetOnly = true,
                            bool noHist = false );

      /** provide list of all keys associated with an object.
       * usage: store->keys(key_vector)
       * @param a vector of strings that will be filled with the list of keys
       */
      template< typename T >
      void keys( std::vector< std::string >& vkeys ) const;

      /// @}

   private:
      /// Function initialising the object
      StatusCode initialize() const;

      /// Pointer to the TEvent that this object interacts with
      mutable xAOD::TEvent* m_pevm;
      /// Pointer to the TStore that this object interacts with
      mutable xAOD::TStore* m_ptds;

   }; // class SgTEvent

} // namespace asg

// Include the template implementation:
#include "AsgTools/SgTEvent.icc"

#endif // ASGTOOLS_SGTEVENT_H
