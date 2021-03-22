/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AuxStoreWrapper.cxx 695881 2015-09-21 08:47:05Z will $

// Gaudi/Athena include(s):
#include "SGTools/DataProxy.h"
#include "AthenaKernel/ClassID_traits.h"
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "AthContainersInterfaces/IConstAuxStore.h"
#include "AthContainersInterfaces/IAuxStore.h"
#include "AthContainersInterfaces/IAuxStoreHolder.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/AuxVectorBase.h"
#include "AthContainers/AuxElement.h"

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
      : AthAlgorithm( name, svcLoc ), m_cachesSet( false ),
        m_clidSvc( "ClassIDSvc", name ) {

      declareProperty( "SGKeys", m_keys,
                       "StoreGate keys of the store objects to be wrapped" );
      declareProperty( "ClassIDSvc", m_clidSvc, "Service providing CLID info" );
   }

   StatusCode AuxStoreWrapper::initialize() {

      ATH_MSG_INFO( "Initialising - Package version: " << PACKAGE_VERSION );
      ATH_MSG_DEBUG( "  StoreGate keys: " << m_keys );

      // Retrieve the needed component(s):
      ATH_CHECK( m_clidSvc.retrieve() );

      // Pack the SG keys into a structure that's more efficient during
      // event processing:
      m_keysSet.insert( m_keys.begin(), m_keys.end() );

      // Reset the caches:
      m_cachesSet = false;
      m_toConvert.clear();
      m_clids.clear();

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode AuxStoreWrapper::execute() {

      // The StoreGate content before the object wrapping:
      ATH_MSG_VERBOSE( "Event store before wrapping:\n" << evtStore()->dump() );

      // If we don't have a list of keys to be converted yet, get one now:
      if( ! m_cachesSet ) {

         // Ask StoreGate for all the objects that it's holding on to:
         const std::vector< const SG::DataProxy* > proxies =
            evtStore()->proxies();

         // Loop over them:
         for( const SG::DataProxy* proxy : proxies ) {

            ATH_MSG_VERBOSE( "Evaluating object with name \"" << proxy->name()
                             << "\" and CLID " << proxy->clID() );

            // Check if we need to worry about this object:
            if( !m_keysSet.empty() ) {
               if( m_keysSet.find( proxy->name() ) == m_keysSet.end() ) {
                  // This SG key was not mentioned in the jobOptions...
                  ATH_MSG_VERBOSE( "Conversion for this key was not "
                                   "requested" );
                  // But if this is a proxy for an interface container for which
                  // we wrap the auxiliary container, then let's remember it.
                  if( m_keysSet.find( proxy->name() + "Aux." ) !=
                      m_keysSet.end() ) {
                     std::string typeName;
                     ATH_CHECK( m_clidSvc->getTypeNameOfID( proxy->clID(),
                                                            typeName ) );
                     if( typeName.substr( 0, 6 ) == "xAOD::" ) {
                        m_clids[ proxy->name() ] = proxy->clID();
                     }
                  }
                  continue;
               }
            }
            ATH_MSG_VERBOSE( "The wrapping of this object was requested" );

            // Remember the CLID of this type, if it's an xAOD type:
            std::string typeName;
            ATH_CHECK( m_clidSvc->getTypeNameOfID( proxy->clID(),
                                                   typeName ) );
            if( ( typeName.substr( 0, 6 ) == "xAOD::" ) &&
                ( typeName != "xAOD::ByteStreamAuxContainer_v1" ) ) {
               m_clids[ proxy->name() ] = proxy->clID();
            }

            // Check if the key of the object looks like it is an auxiliary
            // store. (To try to avoid unnecessary warnings.)
            if( proxy->name().find( "Aux." ) !=
                ( proxy->name().size() - 4 ) ) {
               ATH_MSG_VERBOSE( "Key doesn't look like that of an auxiliary "
                                << "store" );
               continue;
            }
            ATH_MSG_VERBOSE( "StoreGate key looks like that of an auxiliary "
                             << "store..." );

            // This object/container will be converted:
            m_toConvert.insert( proxy->name() );
         }

         // The caches are now set:
         m_cachesSet = true;
      }

      // Now do the wrapping on the selected objects/containers:
      for( const std::string& name : m_toConvert ) {

         // Tell the user what's happening:
         ATH_MSG_VERBOSE( "Now wrapping object: " << name );

         // Access the object with an SG::IAuxStore pointer:
         const SG::IAuxStore* original = nullptr;
         if( ! evtStore()->retrieve( original, name ).isSuccess() ) {
            ATH_MSG_WARNING( "Couldn't retrieve object \"" << name << "\" "
                             << "with interface SG::IAuxStore" );
            continue;
         }

         // Check if the object implements the IAuxStoreHolder interface
         // as well.
         const SG::IAuxStoreHolder* holder =
            dynamic_cast< const SG::IAuxStoreHolder* >( original );
         if( ! holder ) {
            // If not, let's just assume that it describes a container.
            CHECK( changeContainer( original, name ) );
            continue;
         }

         // Decide how to replace it exactly:
         if( holder->getStoreType() == SG::IAuxStoreHolder::AST_ObjectStore ) {
            CHECK( changeElement( original, name ) );
         } else {
            CHECK( changeContainer( original, name ) );
         }
      }

      // The StoreGate content before the object wrapping:
      ATH_MSG_VERBOSE( "Event store after wrapping:\n" << evtStore()->dump() );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode AuxStoreWrapper::changeContainer( const SG::IAuxStore* store,
                                                const std::string& name ) {

      // Make StoreGate forget about this object:
      auto aux_clid_itr = m_clids.find( name );
      if( aux_clid_itr == m_clids.end() ) {
         ATH_MSG_FATAL( "Didn't find a CLID for key: " << name );
         ATH_MSG_FATAL( "This is an internal logic error" );
         return StatusCode::FAILURE;
      }
      evtStore()->releaseObject( aux_clid_itr->second, name );
      ATH_MSG_VERBOSE( "Released object with CLID " << aux_clid_itr->second
                       << " and name \"" << name << "\"" );

      // Get the pointer for the proxy of the auxiliary store:
      SG::DataProxy* storeProxy =
         evtStore()->proxy( ClassID_traits< SG::IAuxStore >::ID(), name );
      if( ! storeProxy ) {
         ATH_MSG_FATAL( "Couldn't get proxy for the "
                        "SG::IAuxStore base class of: " << name );
         return StatusCode::FAILURE;
      }
      // Now make StoreGate forget about this proxy completely:
      CHECK( evtStore()->removeProxy( storeProxy, nullptr, true ) );

      // Create the new object as a wrapper around the original:
      xAOD::AuxContainerBase* holder = new xAOD::AuxContainerBase();
      holder->setStore( const_cast< SG::IAuxStore* >( store ) );

      // Record the new container with the same key:
      CHECK( evtStore()->overwrite( holder, name, false, true ) );

      ATH_MSG_DEBUG( "Overwrote store object with key \""
                     << name << "\" with an xAOD::AuxContainerBase "
                     << "object" );

      // The key of the interface container:
      const std::string intName = name.substr( 0, name.size() - 4 );

      // The CLID for the interface container:
      CLID intId = 0;
      auto clid_itr = m_clids.find( intName );
      if( clid_itr == m_clids.end() ) {
         ATH_MSG_WARNING( "Didn't cache a CLID for key: " << intName );
         ATH_MSG_WARNING( "Interface container can't be updated, the job will "
                          "likely crash" );
         return StatusCode::SUCCESS;
      }
      intId = clid_itr->second;

      // Try to access the proxy for the interface container:
      SG::DataProxy* proxy = evtStore()->proxy( intId, intName );
      if( ! proxy ) {
         // If there is none, return at this point:
         ATH_MSG_VERBOSE( "Interface container with key \"" << intName
                          << "\" not available" );
         return StatusCode::SUCCESS;
      }

      // Get the raw pointer to the object:
      void* ptr = SG::DataProxy_cast( proxy, intId );
      if( ! ptr ) {
         REPORT_ERROR( StatusCode::FAILURE )
            << "Couldn't retrieve raw pointer to interface container";
         return StatusCode::FAILURE;
      }

      // Since all the "wrapped" objects should be xAOD objects, for which
      // the interface containers all implement a straight inheritance tree
      // back to SG::AuxVectorBase, the following ugly operation should be
      // safe:
      SG::AuxVectorBase* interface =
         reinterpret_cast< SG::AuxVectorBase* >( ptr );

      // And now connect the interface to the store depending on the const-ness
      // of the interface:
      if( proxy->isConst() ) {
         interface->setConstStore( holder );
      } else {
         interface->setStore( holder );
      }

      ATH_MSG_DEBUG( "Interface container with key \"" << intName
                     << "\" updated" );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode AuxStoreWrapper::changeElement( const SG::IAuxStore* store,
                                              const std::string& name ) {

      // Make StoreGate forget about this object:
      auto aux_clid_itr = m_clids.find( name );
      if( aux_clid_itr == m_clids.end() ) {
         ATH_MSG_FATAL( "Didn't find a CLID for key: " << name );
         ATH_MSG_FATAL( "This is an internal logic error" );
         return StatusCode::FAILURE;
      }
      evtStore()->releaseObject( aux_clid_itr->second, name );
      ATH_MSG_VERBOSE( "Released object with CLID " << aux_clid_itr->second
                       << " and name \"" << name << "\"" );

      // Create the new as a wrapper around the original:
      xAOD::AuxInfoBase* holder = new xAOD::AuxInfoBase();
      holder->setStore( const_cast< SG::IAuxStore* >( store ) );

      // Record the new container with a different key:
      CHECK( evtStore()->overwrite( holder, name, false, true ) );

      ATH_MSG_DEBUG( "Wrapped store object with key \""
                     << name << "\" into xAOD::AuxInfoBase "
                     << "object" );

       // The key of the interface object:
      const std::string intName = name.substr( 0, name.size() - 4 );

      // The CLID for the interface object:
      CLID intId = 0;
      auto clid_itr = m_clids.find( intName );
      if( clid_itr == m_clids.end() ) {
         ATH_MSG_WARNING( "Didn't cache a CLID for key: " << intName );
         ATH_MSG_WARNING( "Interface object can't be updated, the job will "
                          "likely crash" );
         return StatusCode::SUCCESS;
      }
      intId = clid_itr->second;

      // Try to access the proxy for the interface object:
      SG::DataProxy* proxy = evtStore()->proxy( intId, intName );
      if( ! proxy ) {
         // If there is none, return at this point:
         ATH_MSG_VERBOSE( "Interface object with key \"" << intName
                          << "\" not available" );
         return StatusCode::SUCCESS;
      }

      // Get the raw pointer to the object:
      void* ptr = SG::DataProxy_cast( proxy, intId );
      if( ! ptr ) {
         REPORT_ERROR( StatusCode::FAILURE )
            << "Couldn't retrieve raw pointer to interface object";
         return StatusCode::FAILURE;
      }

      // Since all the "wrapped" objects should be xAOD objects, for which
      // the interface objects all implement a straight inheritance tree
      // back to SG::AuxElement, the following ugly operation should be
      // safe:
      SG::AuxElement* interface =
         reinterpret_cast< SG::AuxElement* >( ptr );

      // And now connect the interface to the store depending on the const-ness
      // of the interface:
      if( proxy->isConst() ) {
         interface->setConstStore( holder );
      } else {
         interface->setStore( holder );
      }

      ATH_MSG_DEBUG( "Interface object with key \"" << intName
                     << "\" updated" );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
