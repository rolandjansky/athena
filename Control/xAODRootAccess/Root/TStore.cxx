/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ROOT include(s):
#include <TError.h>
#include <TClass.h>

// EDM include(s):
#include "AthContainers/normalizedTypeinfoName.h"

// Local include(s):
#include "xAODRootAccess/TStore.h"
#include "xAODRootAccess/TActiveStore.h"
#include "xAODRootAccess/tools/THolder.h"
#include "xAODRootAccess/tools/Message.h"
#include "xAODRootAccess/tools/Utils.h"

namespace xAOD {

   TStore::TStore()
      : m_objects(), m_keys() {

      setActive();
   }

   TStore::~TStore() {

      // Clear the store before being deleted:
      clear();

      // If this object is set up as the active store at the moment,
      // notify the active store object that this object will no longer
      // be available.
      if( TActiveStore::store() == this ) {
         TActiveStore::setStore( nullptr );
      }
   }

   void TStore::setActive() {

      TActiveStore::setStore( this );
      return;
   }

   TReturnCode TStore::remove( const std::string& key ) {

      // Look up this object:
      Objects_t::iterator itr = m_objects.find( key );
      if( itr == m_objects.end() ) {
         ::Warning( "xAOD::TStore::remove",
                    "Couldn't find object with key \"%s\"",
                    key.c_str() );
         return TReturnCode::kRecoverable;
      }

      // Delete the hoder object:
      delete itr->second;

      // Now remove this element from the maps:
      m_objects.erase( itr );
      m_keys.erase( Utils::hash( key ) );

      // We were successful:
      return TReturnCode::kSuccess;
   }

   TReturnCode TStore::remove( void* ptr ) {

      // Look for this object:
      Objects_t::iterator itr = m_objects.begin();
      Objects_t::iterator end = m_objects.end();
      for( ; itr != end; ++itr ) {

         // Check if this is the object in question:
         if( itr->second->get() != ptr ) {
            continue;
         }

         // Yes, it is...
         delete itr->second;
         m_keys.erase( Utils::hash( itr->first ) );
         m_objects.erase( itr );
         return TReturnCode::kSuccess;
      }

      // We didn't find the object in the store:
      ::Warning( "xAOD::TStore::remove",
                 "Couldn't find object with pointer %p",
                 ptr );
      return TReturnCode::kRecoverable;
   }

   void TStore::clear() {

      // Delete all the managed objects:
      Objects_t::iterator itr = m_objects.begin();
      Objects_t::iterator end = m_objects.end();
      for( ; itr != end; ++itr ) {
         delete itr->second;
      }

      // And clear the maps:
      m_objects.clear();
      m_keys.clear();

      return;
   }

   /// This is really just meant for debugging an analysis code, to see what's
   /// going on in it. Behaves a bit similar to StoreGateSvc::dump().
   ///
   void TStore::print() const {

      ::Info( "xAOD::TStore::print", "Contents of transient store:" );
      Objects_t::const_iterator itr = m_objects.begin();
      Objects_t::const_iterator end = m_objects.end();
      for( ; itr != end; ++itr ) {
         ::Info( "xAOD::TStore::print", "-----------------------------------" );
         ::Info( "xAOD::TStore::print", "  Name: %s", itr->first.c_str() );
         ::TClass* cl = itr->second->getClass();
         const std::type_info* ti = itr->second->getTypeInfo();
         ::Info( "xAOD::TStore::print", "    Type: %s",
                 ( cl ? cl->GetName() :
                   ( ti ? SG::normalizedTypeinfoName( *ti ).c_str() :
                     "Unknown" ) ) );
         ::Info( "xAOD::TStore::print", "    Pointer: %p",
                 itr->second->get() );
         ::Info( "xAOD::TStore::print", "    IsOwner: %s",
                 ( itr->second->isOwner() ? "Yes" : "No" ) );
         ::Info( "xAOD::TStore::print", "    HasDictionary: %s",
                 ( cl ? "Yes" : "No" ) );
      }
      ::Info( "xAOD::TStore::print", "-----------------------------------" );

      return;
   }

   ::Bool_t TStore::contains( const std::string& key,
                              const std::type_info& ti ) const {

      // Look up this object:
      Objects_t::const_iterator itr = m_objects.find( key );
      if( itr == m_objects.end() ) {
         return kFALSE;
      }

      // Check if the object is of the right type:
      return itr->second->getAsConst( ti, kTRUE );
   }

   ::Bool_t TStore::isConst( const std::string& key,
                             const std::type_info& ti ) const {

      // Look up this object:
      Objects_t::const_iterator itr = m_objects.find( key );
      if( itr == m_objects.end() ) {
         ::Warning( "xAOD::TStore::isConst",
                    "Object with key \"%s\" not available in store",
                    key.c_str() );
         return kFALSE;
      }

      // Check if it can even be retrieved as a constant object of this type:
      if( ! itr->second->getAsConst( ti, kTRUE ) ) {
         const std::string typeName = SG::normalizedTypeinfoName( ti );
         ::Warning( "xAOD::TStore::isConst",
                    "Object with key \"%s\" can't be retrieved as type: %s",
                    key.c_str(), typeName.c_str() );
         return kFALSE;
      }

      // Now the question is just whether it can be retrieved as a non-const
      // object as well:
      return ( ! itr->second->getAs( ti, kTRUE ) );
   }

   void* TStore::getObject( const std::string& key,
                            const std::type_info& ti ) const {

      // Look up this object:
      Objects_t::const_iterator itr = m_objects.find( key );
      if( itr == m_objects.end() ) {
         return 0;
      }

      // Try to retrieve it as the requested type:
      return itr->second->getAs( ti );
   }

   const void* TStore::getConstObject( const std::string& key,
                                       const std::type_info& ti ) const {

      // Look up this object:
      Objects_t::const_iterator itr = m_objects.find( key );
      if( itr == m_objects.end() ) {
         return 0;
      }

      // Try to retrieve it as the requested type:
      return itr->second->getAsConst( ti );
   }

   /// This internal function does the heavy lifting of recording objects into
   /// the store that have a proper ROOT dictionary.
   ///
   /// @param obj       Typeless pointer to the object being recorded
   /// @param key       Key to record the object with
   /// @param classname The type name of the object being recorded
   /// @param isOwner   If <code>kTRUE</code>, the store takes ownership of the
   ///                  object, otherwise it doesn't
   /// @returns The usual xAOD::TReturnCode types
   ///
   TReturnCode TStore::record( void* obj, const std::string& key,
                               const std::string& classname,
                               ::Bool_t isOwner ) {

      // First check if we have this dictionary cached already:
      static std::map< std::string, ::TClass* > clMap;
      ::TClass* cl = 0;
      auto clItr = clMap.find( classname );
      if( clItr != clMap.end() ) {
         // If the cached value doesn't work, then bail now:
         if( ( ! clItr->second ) || ( ! clItr->second->IsLoaded() ) ) {
            return TReturnCode::kRecoverable;
         }
         // Otherwise we're done:
         cl = clItr->second;
      }

      // If it's not cached, ask ROOT for it:
      if( ! cl ) {
         cl = ::TClass::GetClass( classname.c_str(), kTRUE, kTRUE );
         clMap[ classname ] = cl;
      }
      if( ( ! cl ) || ( ! cl->IsLoaded() ) ) {
         return TReturnCode::kRecoverable;
      }

      // Make sure that the key is not yet taken:
      if( m_objects.find( key ) != m_objects.end() ) {
         ::Error( "xAOD::TStore::record",
                  XAOD_MESSAGE( "Trying to overwrite object with key \"%s\"" ),
                  key.c_str() );
         return TReturnCode::kFailure;
      }

      // Register the new object:
      m_objects[ key ] = new THolder( obj, cl, isOwner );
      m_keys[ Utils::hash( key ) ] = key;
      return TReturnCode::kSuccess;
   }

   TReturnCode TStore::record( void* obj, const std::string& key,
                               const std::type_info& ti ) {

      // Make sure that the key is not yet taken:
      if( m_objects.find( key ) != m_objects.end() ) {
         ::Error( "xAOD::TStore::record",
                  XAOD_MESSAGE( "Trying to overwrite object with key \"%s\"" ),
                  key.c_str() );
         return TReturnCode::kFailure;
      }

      // Register the new object:
      m_objects[ key ] = new THolder( obj, ti );
      m_keys[ Utils::hash( key ) ] = key;
      return TReturnCode::kSuccess;
   }

   TReturnCode TStore::record( THolder* hldr, const std::string& key ) {

      // Make sure that the key is not yet taken:
      if( m_objects.find( key ) != m_objects.end() ) {
         ::Error( "xAOD::TStore::record",
                  XAOD_MESSAGE( "Trying to overwrite object with key \"%s\"" ),
                  key.c_str() );
         // We delete the holder object at this point. It's quite ugly in terms
         // of code readibility, but it results in fewer characters in the
         // template code...
         delete hldr;
         return TReturnCode::kFailure;
      }

      // Register the new object:
      m_objects[ key ] = hldr;
      m_keys[ Utils::hash( key ) ] = key;
      return TReturnCode::kSuccess;
   }

   /// This is a reasonably fast function. It checks whether an object with the
   /// specified hashed key is managed by the store.
   ///
   /// @param hash The hashed key of the object that we are looking for
   /// @returns <code>kTRUE</code> if the object is managed by the store, or
   ///          <code>kFALSE</code> if it is not
   ///
   ::Bool_t TStore::contains( uint32_t hash ) const {

      // Do the check quickly:
      return ( m_keys.find( hash ) != m_keys.end() );
   }

   /// This is a quite slow function. It needs to check each managed object to
   /// possibly find which one of them has this pointer.
   ///
   /// @param ptr Pointer to the object that may or may not be in the store
   /// @returns <code>kTRUE</code> if the object is managed by the store,
   ///          or <code>kFALSE</code> if it isn't
   ///
   ::Bool_t TStore::contains( const void* ptr ) const {

      // Loop over all the managed objects:
      Objects_t::const_iterator itr = m_objects.begin();
      Objects_t::const_iterator end = m_objects.end();
      for( ; itr != end; ++itr ) {
         // Check if this is the right object:
         if( itr->second->get() == ptr ) {
            return kTRUE;
         }
      }

      // We didn't find it:
      return kFALSE;
   }

   /// This is a fairly fast function. Used by ElementLinks that point to
   /// objects in this store.
   ///
   /// @param hash The hashed key for which we want to find the string key
   /// @returns The name corresponding to this hashed key if the hashed key is
   ///          known, or an empty string if it isn't.
   ///
   const std::string& TStore::getName( uint32_t hash ) const {

      // Try to find the name associated with this key:
      HashedKeys_t::const_iterator itr = m_keys.find( hash );
      if( itr != m_keys.end() ) {
         // We found it:
         return itr->second;
      }

      // We didn't find it:
      ::Warning( "xAOD::TStore::getName", "Key 0x%08x not known by the store",
                 hash );
      static const std::string dummy( "" );
      return dummy;
   }

   /// This is just as slow as the previous contains function. It needs to look
   /// at all the managed objects one by one to find under what name it is
   /// managed.
   ///
   /// @param ptr Pointer to the object that we want to know the name of
   /// @returns The name with which this object was recorded into the store. Or
   ///          an empty string of the object is not known to the store.
   ///
   const std::string& TStore::getName( const void* ptr ) const {

      // Loop over all the managed objects:
      Objects_t::const_iterator itr = m_objects.begin();
      Objects_t::const_iterator end = m_objects.end();
      for( ; itr != end; ++itr ) {
         // Check if this is the right object:
         if( itr->second->get() == ptr ) {
            return itr->first;
         }
      }
      
      // We didn't find the object:
      ::Warning( "xAOD::TStore::getName", "Object %p is not held by the store",
                 ptr );
      static const std::string dummy( "" );
      return dummy;
   }

} // namespace xAOD
