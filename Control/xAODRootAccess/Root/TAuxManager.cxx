/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TAuxManager.cxx 607344 2014-07-18 13:27:49Z krasznaa $

// ROOT include(s):
#include <TError.h>

// Local include(s):
#include "xAODRootAccess/tools/TAuxManager.h"
#include "xAODRootAccess/tools/Message.h"
#include "xAODRootAccess/TAuxStore.h"

namespace xAOD {

   TAuxManager::TAuxManager( TAuxStore* store, ::Bool_t sharedOwner )
      : m_store(), m_storePtr( store ) {

      if( sharedOwner ) {
         m_store.reset( store );
      }
   }

   TAuxManager::TAuxManager( const TAuxManager& parent )
      : m_store( parent.m_store ), m_storePtr( parent.m_storePtr ) {

   }

   TAuxManager& TAuxManager::operator= ( const TAuxManager& rhs ) {

      // Check if anything needs to be done:
      if( this == &rhs ) {
         return *this;
      }

      // Do the assignment:
      m_store = rhs.m_store;
      m_storePtr = rhs.m_storePtr;

      return *this;
   }

   ::Int_t TAuxManager::getEntry( ::Long64_t entry, ::Int_t getall ) {

      return m_storePtr->getEntry( entry, getall );
   }

   void* TAuxManager::object() const {

      return m_storePtr;
   }

   void TAuxManager::setObject( void* ptr ) {

      if( m_store.get() ) {
         m_store.reset( reinterpret_cast< TAuxStore* >( ptr ) );
      }
      m_storePtr = reinterpret_cast< TAuxStore* >( ptr );

      return;
   }

   /// The state of a TAuxStore object is always "set". So this
   /// interface unfortunately doesn't make much sense for this
   /// manager class...
   ///
   /// @param forceSet Ignored as it does not apply
   /// @returns <code>kTRUE</code>
   ///
   ::Bool_t TAuxManager::isSet( ::Bool_t /*forceSet*/ ) const {

      return kTRUE;
   }

   /// Resetting an auxiliary store needs to be done in a smart way.
   /// Container stores need to be emptied, while object stores don't need
   /// to be touched.
   ///
   void TAuxManager::reset() {

      // Clear the store if it's a container store:
      if( m_storePtr->structMode() == TAuxStore::kContainerStore ) {
         m_storePtr->resize( 0 );
         m_storePtr->reset();
      }

      return;
   }

   TAuxStore* TAuxManager::getStore() const {

      return m_storePtr;
   }

   const SG::IConstAuxStore* TAuxManager::getConstStore() const {

      return m_storePtr;
   }

} // namespace xAOD
