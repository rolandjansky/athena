/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AuxStoreWrapper.cxx 586335 2014-03-05 13:50:34Z krasznaa $

// Gaudi/Athena include(s):
#include "SGTools/DataProxy.h"
#include "SGTools/ClassID_traits.h"
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "AthContainersInterfaces/IAuxStore.h"
#include "AthContainersInterfaces/IAuxStoreHolder.h"

// xAOD include(s):
#include "xAODCore/AuxContainerBase.h"
#include "xAODCore/AuxInfoBase.h"

// Local include(s):
#include "AuxStoreWrapper.h"

namespace {
   /// Helper operator for printing the contents of vectors
   template< typename T >
   MsgStream& operator<< ( MsgStream& out,
                           const std::vector< T >& vec ) {

      out << "[";
      for( size_t i = 0; i < vec.size(); ++i ) {
         out << vec[ i ];
         if( i < ( vec.size() - 1 ) ) {
            out << ", ";
         }
      }
      out << "]";
      return out;
   }
} // private namespace

namespace xAODMaker {

   AuxStoreWrapper::AuxStoreWrapper( const std::string& name,
                                     ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc ) {

      declareProperty( "SGKeys", m_keys,
                       "StoreGate keys of the store objects to be wrapped" );
   }

   StatusCode AuxStoreWrapper::initialize() {

      ATH_MSG_INFO( "Initialising - Package version: " << PACKAGE_VERSION );
      ATH_MSG_DEBUG( "  StoreGate keys: " << m_keys );

      // Pack the SG keys into a structure that's more efficient during
      // event processing:
      m_keysSet.insert( m_keys.begin(), m_keys.end() );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode AuxStoreWrapper::execute() {

      // Ask StoreGate for all the objects that it's holding on to:
      const std::vector< const SG::DataProxy* > proxies =
         evtStore()->proxies();

      // List of objects already "converted":
      std::set< std::string > converted;

      // Loop over them:
      std::vector< const SG::DataProxy* >::const_iterator itr = proxies.begin();
      std::vector< const SG::DataProxy* >::const_iterator end = proxies.end();
      for( ; itr != end; ++itr ) {

         ATH_MSG_VERBOSE( "Evaluating object with name \"" << ( *itr )->name()
                          << "\" and CLID " << ( *itr )->clID() );

         // Check if we need to worry about this object:
         if( m_keysSet.size() ) {
            if( m_keysSet.find( ( *itr )->name() ) == m_keysSet.end() ) {
               // This SG key was not mentioned in the jobOptions...
               ATH_MSG_VERBOSE( "Conversion for this key was not requested" );
               continue;
            }
         }
         ATH_MSG_VERBOSE( "The wrapping of this object was requested" );

         // Check if the key of the object looks like it is an auxiliary store.
         // (To try to avoid unnecessary warnings.)
         if( ( *itr )->name().find( "Aux." ) !=
             ( ( *itr )->name().size() - 4 ) ) {
            ATH_MSG_VERBOSE( "Key doesn't look like that of an auxiliary "
                             << "store" );
            continue;
         }
         ATH_MSG_VERBOSE( "StoreGate key looks like that of an auxiliary "
                          << "store..." );

         // Check if we already tried to convert it. It should only be tried
         // once...
         if( ! converted.insert( ( *itr )->name() ).second ) {
            ATH_MSG_VERBOSE( "\"" << ( *itr )->name() << "\" was already "
                             "converted" );
            continue;
         }
         ATH_MSG_VERBOSE( "Object was not yet processed" );

         // Retrieve a private copy of the object:
         std::auto_ptr< SG::IAuxStore >
            ptr( evtStore()->readPrivateCopy< SG::IAuxStore >( ( *itr )->name() ) );
         if( ! ptr.get() ) {
            ATH_MSG_WARNING( "Private copy for object \""
                             << ( *itr )->name()
                             << "\" could not be retrieved!" );
            ATH_MSG_WARNING( "Slimming for the object not possible!" );
            continue;
         }

         // Take over the memory management from std::auto_ptr. It's the
         // created AuxContainerBase and AuxInfoBase objects that take ownership
         // of this object in the end.
         SG::IAuxStore* store = ptr.release();

         // Check if the object implements the IAuxStoreHolder interface
         // as well.
         const SG::IAuxStoreHolder* holder =
            dynamic_cast< const SG::IAuxStoreHolder* >( store );
         if( ! holder ) {
            // If not, let's just assume that it describes a container.
            CHECK( changeContainer( store, *itr ) );
            continue;
         }

         // Decide how to replace it exactly:
         if( holder->getStoreType() == SG::IAuxStoreHolder::AST_ObjectStore ) {
            CHECK( changeElement( store, *itr ) );
         } else {
            CHECK( changeContainer( store, *itr ) );
         }
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode AuxStoreWrapper::changeContainer( SG::IAuxStore* store,
                                                const SG::DataProxy* proxy ) {

      // Create the new object:
      xAOD::AuxContainerBase* holder = new xAOD::AuxContainerBase();
      holder->setStore( store );

      // Record the new container with the same key:
      CHECK( evtStore()->overwrite( holder, proxy->name(), false, false ) );

      ATH_MSG_DEBUG( "Wrapped store object with key \""
                     << proxy->name() << "\" into xAOD::AuxContainerBase "
                     << "object" );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode AuxStoreWrapper::changeElement( SG::IAuxStore* store,
                                              const SG::DataProxy* proxy ) {

      // Create the new object:
      xAOD::AuxInfoBase* holder = new xAOD::AuxInfoBase();
      holder->setStore( store );

      // Record the new container with a different key:
      CHECK( evtStore()->overwrite( holder, proxy->name(), false, false ) );

      ATH_MSG_DEBUG( "Wrapped store object with key \""
                     << proxy->name() << "\" into xAOD::AuxInfoBase "
                     << "object" );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
