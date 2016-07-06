/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TFileChecker.cxx 673572 2015-06-09 08:40:47Z krasznaa $

// System include(s):
#include <set>
#include <string>
#include <map>

// ROOT include(s):
#include <TClass.h>
#include <TVirtualCollectionProxy.h>

// EDM include(s):
#include "AthContainersInterfaces/IConstAuxStore.h"
#include "AthContainersInterfaces/IAuxStoreIO.h"
#include "AthContainers/AuxVectorBase.h"
#include "AthContainers/AuxTypeRegistry.h"

// Local include(s):
#include "xAODRootAccess/tools/TFileChecker.h"
#include "xAODRootAccess/tools/Message.h"
#include "xAODRootAccess/tools/ReturnCheck.h"
#include "xAODRootAccess/TEvent.h"

ClassImp( xAOD::TFileChecker )

namespace xAOD {

   /// Internal cache of dictionaries for auxiliary vector types
   static std::map< SG::auxid_t, ::TClass* > s_dictCache =
      std::map< SG::auxid_t, ::TClass* >();

   TFileChecker::TFileChecker()
   : ::TObject(), m_stopOnError( kFALSE ), m_ignoredVariables(),
     m_orpannedContainers() {

   }

   TReturnCode TFileChecker::check( TEvent& event ) {

      // Reset the internal cache(es):
      m_orpannedContainers.clear();

      // Bail right away if there are no events in the file:
      if( ! event.getEntries() ) {
         return TReturnCode::kSuccess;
      }

      // Load the first event:
      if( event.getEntry( 0 ) < 0 ) {
         Error( "check",
                XAOD_MESSAGE( "Couldn't load event 0 from the input" ) );
         return TReturnCode::kFailure;
      }

      // Get the list of containers in the file:
      std::set< std::string > containerNames;
      const EventFormat* ef = event.inputEventFormat();
      EventFormat::const_iterator ef_itr = ef->begin();
      EventFormat::const_iterator ef_end = ef->end();
      for( ; ef_itr != ef_end; ++ef_itr ) {

         // Skip the auxiliary objects:
         if( ef_itr->first.find( "Aux." ) == ( ef_itr->first.size() - 4 ) ) {
            continue;
         }
         if( ef_itr->second.parentName() != "" ) {
            continue;
         }

         // Check whether it's available, and a container type:
         if( ! event.contains< SG::AuxVectorBase >( ef_itr->first ) ) {
            continue;
         }

         // This seems to be a valid vector type to check:
         containerNames.insert( ef_itr->first );
      }

      // Loop over the events:
      const ::Long64_t entries = event.getEntries();
      for( ::Long64_t entry = 0; entry < entries; ++entry ) {

         // Load the event:
         if( event.getEntry( entry ) < 0 ) {
            Error( "check",
                   XAOD_MESSAGE( "Failed to load event %i from the input" ),
                   static_cast< int >( entry ) );
            return TReturnCode::kFailure;
         }

         // Print a progress message:
         if( ! ( entry % 100 ) ) {
            Info( "check", "Now validating entry %i / %i",
                  static_cast< int >( entry ), static_cast< int >( entries ) );
         }

         // Check all containers:
         for( const std::string& cname : containerNames ) {

            // Retrieve the container:
            const SG::AuxVectorBase* vec = 0;
            RETURN_CHECK( "xAOD::TFileChecker::check",
                          event.retrieve( vec, cname ) );

            // Check it:
            if( m_stopOnError ) {
               RETURN_CHECK( "xAOD::TFileChecker::check",
                             checkContainer( *vec, cname ) );
            } else {
               if( checkContainer( *vec, cname ).isFailure() ) {
                  Error( "check",
                         XAOD_MESSAGE( "Error found with container \"%s\" in "
                                       "event %i" ),
                         cname.c_str(), static_cast< int >( entry ) );
               }
            }
         }
      }

      // Return gracefully:
      return TReturnCode::kSuccess;
   }

   void TFileChecker::setStopOnError( ::Bool_t value ) {

      m_stopOnError = value;
      return;
   }

   ::Bool_t TFileChecker::stopOnError() const {

      return m_stopOnError;
   }

   void TFileChecker::addIgnoredVariable( const std::string& name ) {

      m_ignoredVariables.insert( name );
      return;
   }

   void
   TFileChecker::setIgnoredVariables( const std::set< std::string >& value ) {

      m_ignoredVariables = value;
      return;
   }

   const std::set< std::string >& TFileChecker::ignoredVariables() const {

      return m_ignoredVariables;
   }

   TReturnCode TFileChecker::checkContainer( const SG::AuxVectorBase& vec,
                                             const std::string& name ) {

      // Access the auxiliary store of the container:
      const SG::IConstAuxStore* cStore = vec.getConstStore();
      const SG::IAuxStoreIO* ioStore =
         dynamic_cast< const SG::IAuxStoreIO* >( cStore );
      if( ! ioStore ) {
         if( ! cStore ) {
            // No auxiliary store is found for the container at all. That's a
            // problem.
            if( m_orpannedContainers.find( name ) ==
                m_orpannedContainers.end() ) {
               Error( "checkContainer",
                      XAOD_MESSAGE( "Couldn't get I/O store for "
                                    "container: %s" ),
                      name.c_str() );
               m_orpannedContainers.insert( name );
               return TReturnCode::kFailure;
            }
            return TReturnCode::kSuccess;
         } else {
            // This can happen when the auxiliary store doesn't implement the
            // I/O interface. This is the case for types inheriting from
            // xAOD::ByteStreamAuxContainer for instance. Since all issues that
            // we check for in here are tied to the I/O happening through this
            // interface, let's not bother with such objects.
            return TReturnCode::kSuccess;
         }
      }

      // Get the size of the interface container:
      const size_t size = vec.size_v();

      // Loop over all of its auxiliary variables:
      const SG::auxid_set_t auxids = vec.getAuxIDs();
      for( SG::auxid_t auxid : auxids ) {

         // The auxiliary type registry:
         SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();

         // Get a pointer to the underlying std::vector variable. This should
         // trigger the loading of the necessary dictionaries for this type.
         const void* vecPtr = ioStore->getIOData( auxid );
         if( ! vecPtr ) {
            Error( "checkContainer",
                   XAOD_MESSAGE( "Couldn't load variable %s.%s" ),
                   name.c_str(), reg.getName( auxid ).c_str() );
            return TReturnCode::kFailure;
         }

         // Find a dictionary for this type:
         ::TClass* cl = 0;
         auto itr = s_dictCache.find( auxid );
         if( itr != s_dictCache.end() ) {
            cl = itr->second;
         } else {
            const std::type_info* typeId = reg.getVecType( auxid );
            if( ! typeId ) {
               Error( "checkContainer",
                      XAOD_MESSAGE( "Couldn't get std::type_info for "
                                    "variable %s" ),
                      reg.getName( auxid ).c_str() );
               return TReturnCode::kFailure;
            }
            cl = ::TClass::GetClass( *typeId );
            s_dictCache[ auxid ] = cl;
         }
         if( ! cl ) {
            Error( "checkContainer",
                   XAOD_MESSAGE( "Couldn't get dictionary for variable %s" ),
                   reg.getName( auxid ).c_str() );
            return TReturnCode::kFailure;
         }

         // Make sure that the type has a collection proxy:
         ::TVirtualCollectionProxy* proxy = cl->GetCollectionProxy();
         if( ! proxy ) {
            Error( "checkContainer",
                   XAOD_MESSAGE( "Couldn't get collection proxy for "
                                 "variable %s" ),
                   reg.getName( auxid ).c_str() );
            return TReturnCode::kFailure;
         }

         // Get the size of the vector using the collection proxy:
         proxy->PushProxy( ( void* ) vecPtr );
         const size_t varSize = static_cast< size_t >( proxy->Size() );
         proxy->PopProxy();

         // Check that the size is as it should be:
         if( ( size != varSize ) &&
             ( m_ignoredVariables.find( reg.getName( auxid ) ) ==
               m_ignoredVariables.end() ) ) {
            Error( "checkContainer",
                   XAOD_MESSAGE( "%s.size() (%i) != %sAux.%s.size() (%i)" ),
                   name.c_str(), static_cast< int >( size ),
                   name.c_str(), reg.getName( auxid ).c_str(),
                   static_cast< int >( varSize ) );
            return TReturnCode::kFailure;
         }
      }

      // Return gracefully:
      return TReturnCode::kSuccess;
   }

} // namespace xAOD
