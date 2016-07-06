/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TObjectManager.cxx 731792 2016-03-23 10:27:41Z krasznaa $

// ROOT include(s):
#include <TBranch.h>
#include <TError.h>

// Local include(s):
#include "xAODRootAccess/tools/TObjectManager.h"
#include "xAODRootAccess/tools/THolder.h"

namespace xAOD {

   TObjectManager::TObjectManager( ::TBranch* br, THolder* holder,
                                   ::Bool_t renewOnRead )
      : m_branch( br ), m_holder( holder ), m_entry( -1 ), m_isSet( kTRUE ),
        m_renewOnRead( renewOnRead ) {

   }

   TObjectManager::TObjectManager( const TObjectManager& parent )
      : TVirtualManager(), m_branch( parent.m_branch ),
        m_holder( 0 ), m_entry( parent.m_entry ),
        m_isSet( parent.m_isSet ), m_renewOnRead( parent.m_renewOnRead ) {

      if( parent.m_holder ) {
         m_holder = new THolder( *parent.m_holder );
      }
   }

   TObjectManager::~TObjectManager() {

      // Delete the holder object if we have one:
      if( m_holder ) {
         delete m_holder;
      }
   }

   TObjectManager& TObjectManager::operator=( const TObjectManager& parent ) {

      // Check if we need to do anything:
      if( this == &parent ) {
         return *this;
      }

      m_branch = parent.m_branch;
      if( m_holder ) {
         delete m_holder;
         m_holder = 0;
      }
      if( parent.m_holder ) {
         m_holder = new THolder( *parent.m_holder );
      }
      m_entry       = parent.m_entry;
      m_isSet       = parent.m_isSet;
      m_renewOnRead = parent.m_renewOnRead;

      return *this;
   }

   ::TBranch* TObjectManager::branch() const {

      return m_branch;
   }

   /// This sort of access to the internal TBranch object is needed when
   /// connecting to the input tree. One has to make sure that the correct
   /// pointer is being pointed to by the input tree.
   ///
   /// @return A pointer to the internal branch pointer
   ///
   ::TBranch** TObjectManager::branchPtr() {

      return &m_branch;
   }

   /// @return A pointer to the internal data holding object
   ///
   const THolder* TObjectManager::holder() const {

      return m_holder;
   }

   /// @return A pointer to the internal data holding object
   ///
   THolder* TObjectManager::holder() {

      return m_holder;
   }

   /// This function is used to load the contents of a branch only when it
   /// needs to be done. It keeps track of which entry was already loaded for
   /// a branch/object, and only asks the branch to load an entry when it
   /// really has to be done.
   ///
   /// @param entry The entry to load for the held object
   /// @return 0 if no new entry was read, the number of read bytes otherwise
   ///
   ::Int_t TObjectManager::getEntry( ::Long64_t entry, ::Int_t getall ) {

      // Check if anything needs to be done:
      if( entry == m_entry ) return 0;

      // Remember which entry we're loading at the moment:
      m_entry = entry;

      // Make sure that the branch is not in "MakeClass mode":
      if( m_branch->GetMakeClass() ) {
         m_branch->SetMakeClass( 0 );
      }

      // Renew the object in memory if we are in such a mode:
      if( m_renewOnRead ) {
         m_holder->renew();
      }

      // Load the entry:
      ::Int_t bytes = m_branch->GetEntry( entry, getall );

      // This is a tricky thing... Parts of the code try to minimise some
      // operations to only happen when an object is getting updated. But
      // it can happen that an interface or auxiliary store object doesn't
      // actually store any data itself. So TBranch::GetEntry will return 0.
      // But we still need to tell the code looking at this return value
      // that we did go to a new event. So we should never return 0 at
      // this point...
      if( bytes ) {
         return bytes;
      } else {
         return 1;
      }
   }

   /// This function gives an easy access to the object managed by this
   /// object.
   ///
   /// @return A typeless pointer to the object being managed
   ///
   void* TObjectManager::object() const {

      return m_holder->get();
   }

   /// This is just a convenient way of calling THolder::Set from TEvent.
   ///
   /// @param obj The object to replace the previously managed one
   ///
   void TObjectManager::setObject( void* obj ) {

      m_holder->set( obj );
      m_isSet = kTRUE;
      return;
   }

   /// @param forceSet Ignored, as full objects can't be missing
   ///
   /// @returns <code>kTRUE</code> if the object for this event was set,
   ///          <code>kFALSE</code> otherwise
   ///
   ::Bool_t TObjectManager::isSet( Bool_t /*forceSet*/ ) const {

      return m_isSet;
   }

   /// This function needs to be called after an event was filled into
   /// the output TTree. It tells the manager object that it needs to wait
   /// for another object to be set up for the upcoming event.
   ///
   void TObjectManager::reset() {

      m_isSet = kFALSE;
      return;
   }

} // namespace xAOD
