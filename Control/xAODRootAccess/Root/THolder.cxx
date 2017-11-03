/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: THolder.cxx 796983 2017-02-14 05:09:12Z ssnyder $

// ROOT include(s):
#include <TClass.h>
#include <TError.h>

// EDM include(s):
#include "AthContainers/normalizedTypeinfoName.h"
#include "AthContainers/AuxVectorBase.h"
#include "AthContainers/AuxElement.h"

// Local include(s):
#include "xAODRootAccess/tools/THolder.h"
#include "xAODRootAccess/tools/Message.h"
#include "xAODRootAccess/tools/TDestructorRegistry.h"
#include "THolderCache.h"

namespace xAOD {

   THolder::THolder()
      : m_typeKind (OTHER),
        m_object( 0 ), m_type( 0 ), m_typeInfo( 0 ), m_owner( kFALSE ) {

   }

   THolder::THolder( void* object, ::TClass* type, ::Bool_t owner )
     :  m_typeKind( setTypeKind(type) ),
        m_object( object ), m_type( type ),
        m_typeInfo( type ? m_type->GetTypeInfo() : 0 ), m_owner( owner ) {

      // Complain if the passed dictionary will be unusable later on:
      if( m_type && ( ! m_type->IsLoaded() ) ) {
         ::Warning( "xAOD::THolder::THolder", "Received an emulated dictionary "
                    "for type: %s", m_type->GetName() );
      }

      // Increase the object count:
      if( m_object && m_owner ) {
         Internal::THolderCache::instance().incRef( m_object );
      }
   }

   THolder::THolder( void* object, const std::type_info& type, ::Bool_t owner )
     :  m_typeKind (OTHER),
        m_object( object ), m_type( 0 ), m_typeInfo( &type ), m_owner( owner ) {

      // Increase the object count:
      if( m_object && m_owner ) {
         Internal::THolderCache::instance().incRef( m_object );
      }
   }

   /// The copy constructor takes the pointer from the parent object, and based
   /// on whether the parent owned the object or not, increments the shared
   /// count.
   ///
   /// @param parent The parent object that should be copied
   ///
   THolder::THolder( const THolder& parent )
     :  m_typeKind( parent.m_typeKind ),
        m_object( parent.m_object ), m_type( parent.m_type ),
        m_typeInfo (parent.m_typeInfo),
        m_owner( parent.m_owner ) {

      // Increase the object count:
      if( m_object && m_owner ) {
         Internal::THolderCache::instance().incRef( m_object );
      }
   }

   /// This is a tricky one. Since the object is being moved, meaning that the
   /// parent will be deleted right after this operation, instead of
   /// incrementing and then decrementing the shared count, we just tell the
   /// parent that it doesn't own its object anymore. In this case if it did own
   /// it, the shared count will remain valid. Since it's this object owning
   /// it now. If it didn't own the object in the first place, then no hard no
   /// foul.
   ///
   /// @param parent The parent object that should be moved
   ///
   THolder::THolder( THolder&& parent )
     :  m_typeKind( parent.m_typeKind ),
        m_object( parent.m_object ), m_type( parent.m_type ),
        m_typeInfo (parent.m_typeInfo),
        m_owner( parent.m_owner ) {

      // Tell the parent that it no longer owns the object:
      parent.m_owner = kFALSE;
   }

   THolder::~THolder() {

      // Delete the object using its dictionary:
      if( m_object && m_owner &&
          ( ! Internal::THolderCache::instance().decRef( m_object ) ) ) {
         deleteObject();
      }
   }

   /// The copy operator, just like the copy constructor makes the object
   /// aware that it doesn't own the object that it has the pointer to.
   ///
   /// @param rhs The object that has to be copied
   /// @returns A reference to this same object
   ///
   THolder& THolder::operator=( const THolder& rhs ) {

      // Check if we need to do anything:
      if( &rhs == this ) return *this;

      // Clean up the previously managed object:
      if( m_object && m_owner &&
          ( ! Internal::THolderCache::instance().decRef( m_object ) ) ) {
         deleteObject();
      }

      // Do the copy:
      m_object   = rhs.m_object;
      m_typeKind = rhs.m_typeKind;
      m_type     = rhs.m_type;
      m_typeInfo = rhs.m_typeInfo;
      m_owner    = rhs.m_owner;

      // Increase the object count:
      if( m_object && m_owner ) {
         Internal::THolderCache::instance().incRef( m_object );
      }

      // Return this same object:
      return *this;
   }

   /// This move operator is necessary to be able to use this type as a value
   /// type of STL containers.
   ///
   /// See the comments given for the move constructor for an explanation of the
   /// behaviour of this operator.
   ///
   /// @param rhs The object that has to be moved
   /// @returns A reference to this same object
   ///
   THolder& THolder::operator=( THolder&& rhs ) {

      // Check if we need to do anything:
      if( &rhs == this ) return *this;

      // Clean up the previously managed object:
      if( m_object && m_owner &&
          ( ! Internal::THolderCache::instance().decRef( m_object ) ) ) {
         deleteObject();
      }

      // Do the copy:
      m_object   = rhs.m_object;
      m_typeKind = rhs.m_typeKind;
      m_type     = rhs.m_type;
      m_typeInfo = rhs.m_typeInfo;
      m_owner    = rhs.m_owner;

      // Instead of doing anything with the shared count here, just make the
      // parent not own the object anymore. The logic is the same as discussed
      // in the move constructor.
      rhs.m_owner = kFALSE;

      // Return this same object:
      return *this;
   }

   void* THolder::get() const {

      return m_object;
   }

   void** THolder::getPtr() {

      return &m_object;
   }

   void THolder::set( void* obj ) {

      // Check if we need to do anything:
      if( m_object == obj ) return;

      // Delete the previous object:
      if( m_object && m_owner &&
          ( ! Internal::THolderCache::instance().decRef( m_object ) ) ) {
         deleteObject();
      }

      // Hold on to the new object from now on:
      m_object = obj;

      // Increase the object count:
      if( m_object && m_owner ) {
         Internal::THolderCache::instance().incRef( m_object );
      }

      return;
   }

   ::Bool_t THolder::isOwner() const {

      return m_owner;
   }

   void THolder::setOwner( ::Bool_t state ) {

      // Check if anything needs to be done:
      if( state == m_owner ) return;

      if( m_object && m_owner &&
          ( ! Internal::THolderCache::instance().decRef( m_object ) ) ) {
         ::Warning( "xAOD::THolder::setOwner",
                    "Deleting object %p no longer held by any owner",
                    m_object );
         deleteObject();
      }

      m_owner = state;

      if( m_object && m_owner ) {
         Internal::THolderCache::instance().incRef( m_object );
      }

      return;
   }

   /// This function is used for retrieving an object as one of its
   /// bases. It is used when the caller requires a non-const pointer to the
   /// managed object.
   ///
   /// @param tid The type as which the object is to be retrieved
   /// @param silent When <code>kTRUE</code>, the call will fail
   ///               silently when unsuccessful
   /// @returns A non-const pointer that can be cast to the requested type
   ///
   void* THolder::getAs( const std::type_info& tid,
                         ::Bool_t silent ) const {

      // If there is no dictionary for the object, then the logic is pretty
      // simple:
      if( ! m_type ) {
         // Check if the user asked for the right type. Remember that in this
         // case we can't return an object as its base type.
         if( tid != *m_typeInfo ) {
            if( ! silent ) {
               const std::string heldType =
                  SG::normalizedTypeinfoName( *m_typeInfo );
               const std::string reqType =
                  SG::normalizedTypeinfoName( tid );
               ::Warning( "xAOD::THolder::getAs",
                          "Trying to retrieve %s object with a %s pointer",
                          heldType.c_str(), reqType.c_str() );
            }
            return 0;
         }
         // The user did ask for the correct type:
         return m_object;
      }

      // Check if we already know about this type:
      ::TClass* userClass = Internal::THolderCache::instance().getClass( tid );
      // If not, look for it now:
      if( ! userClass ) {
         userClass = ::TClass::GetClass( tid );
         Internal::THolderCache::instance().addClass( tid, userClass );
      }

      // If we still don't have a dictionary, that's an issue:
      if( ! userClass ) {
         if( ! silent ) {
            ::Error( "xAOD::THolder::getAs",
                     XAOD_MESSAGE( "Couldn't access the dictionary for user "
                                   "type: %s" ),
                     SG::normalizedTypeinfoName( tid ).c_str() );
         }
         return 0;
      }

      // Check if the user requested a valid base class for the held type:
      const Int_t offset = m_type->GetBaseClassOffset( userClass );
      if( offset < 0 ) {
         if( ! silent ) {
            ::Warning( "xAOD::THolder::getAs",
                       "User class \"%s\" is not a valid base "
                       "of \"%s\"",
                       SG::normalizedTypeinfoName( tid ).c_str(),
                       m_type->GetName() );
         }
         return 0;
      }

      // If all is fine, do the cast:
      return ( static_cast< char* >( m_object ) + offset );
   }

   /// This function is used for retrieving an object as one of its
   /// bases. It is used when the caller need a const pointer to the
   /// managed object.
   ///
   /// @param tid The type as which the object is to be retrieved
   /// @param silent When <code>kTRUE</code>, the call will fail
   ///               silently when unsuccessful
   /// @returns A const pointer that can be cast to the requested type
   ///
   const void* THolder::getAsConst( const std::type_info& tid,
                                    ::Bool_t silent ) const {

      // In the generic case we just forward this call to the non-const
      // implementation:
      return getAs( tid, silent );
   }

   ::TClass* THolder::getClass() const {

      return m_type;
   }

   const std::type_info* THolder::getTypeInfo() const {

      return m_typeInfo;
   }

   /// This function is mostly used in "Athena access mode", to delete the
   /// managed objects from memory between events, and recreate them from
   /// scratch.
   ///
   void THolder::renew() {

      // Delete the object using its dictionary:
      if( m_object && m_owner &&
          ( ! Internal::THolderCache::instance().decRef( m_object ) ) ) {
         deleteObject();
      }

      // Create a new object:
      m_owner = kTRUE;
      m_object = m_type->New();
      Internal::THolderCache::instance().incRef( m_object );

      // Return gracefuly:
      return;
   }

   void THolder::deleteObject() {

      // Check if we even have an object:
      if( ! m_object ) {
         return;
      }

      // Delete the object then:
      if( m_type ) {
         if( m_type->IsLoaded() ) {
            m_type->Destructor( m_object );
         }
         // In certain situations it can happen that the memory cleanup for a
         // particular TEvent object happens after ROOT already started to
         // unload libraries from memory. In this case, since the application
         // is anyway on its final leg, let's not bother the user with
         // additional warnings about not being able to clean up after the
         // managed object.
      } else if( m_typeInfo ) {
         const TVirtualDestructor* d =
            TDestructorRegistry::instance().get( *m_typeInfo );
         if( d ) {
            d->destruct( m_object );
         } else {
            ::Error( "xAOD::THolder::deleteObject",
                     XAOD_MESSAGE( "Couldn't delete managed object" ) );
         }
      } else {
         ::Error( "xAOD::THolder::deleteObject",
                  XAOD_MESSAGE( "Couldn't delete managed object" ) );
      }

      return;
   }


   THolder::TypeKind THolder::setTypeKind( const TClass* type )
   {
      static const TClass* dvClass =
         TClass::GetClass( typeid( SG::AuxVectorBase ) );
      static const TClass* aeClass =
         TClass::GetClass( typeid( SG::AuxElement ) );

      if (type) {
        if (type->InheritsFrom( dvClass )) {
          return DATAVECTOR;
        }
        if (type->InheritsFrom( aeClass )) {
          return AUXELEMENT;
        }
      }
      return OTHER;
   }

} // namespace xAOD
