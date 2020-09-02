/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <map>

// Framework include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "AthContainersInterfaces/IConstAuxStore.h"
#include "AthContainersInterfaces/IAuxStoreIO.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/normalizedTypeinfoName.h"
#include "AthLinks/ElementLinkBase.h"

// Local include(s):
#include "ElementLinkResetAlg.h"

namespace xAODMaker {

   ElementLinkResetAlg::ElementLinkResetAlg( const std::string& name,
                                             ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc ) {

      declareProperty( "SGKeys", m_keys );
   }

   StatusCode ElementLinkResetAlg::initialize() {

      // Tell the user what's happening:
      ATH_MSG_INFO( "Initialising - Package version: " << PACKAGE_VERSION );
      ATH_MSG_DEBUG( "SGKeys = " << m_keys );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode ElementLinkResetAlg::execute() {

      // Collect all the container(s):
      std::vector< std::pair< const SG::IConstAuxStore*, std::string > > stores;
      if( m_keys.size() ) {
         for( const std::string& key : m_keys ) {
            const SG::IConstAuxStore* store = 0;
            ATH_CHECK( evtStore()->retrieve( store, key ) );
            stores.push_back( std::make_pair( store, key ) );
         }
      } else {
         SG::ConstIterator< SG::IConstAuxStore > begin, end;
         ATH_CHECK( evtStore()->retrieve( begin, end ) );
         for( auto itr = begin; itr != end; ++itr ) {
            const SG::IConstAuxStore* store = 0;
            ATH_CHECK( evtStore()->retrieve( store, itr.key() ) );
            stores.push_back( std::make_pair( store, itr.key() ) );
         }
      }
      ATH_MSG_DEBUG( "Number of IConstAuxStore objects retrieved: "
                     << stores.size() );

      // Reset the ElementLinks in all of them:
      for( const auto& storeKey : stores ) {
         ATH_MSG_VERBOSE( "Reseting element links in store: "
                          << storeKey.second );
         ATH_CHECK( reset( *( storeKey.first ), storeKey.second ) );
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode ElementLinkResetAlg::reset( const SG::IConstAuxStore& store,
                                          const std::string& key ) {

      // If the container is empty, return right away:
      if( ! store.size() ) {
         return StatusCode::SUCCESS;
      }

      // Get all the IDs stored in this object:
      const SG::auxid_set_t& auxids = store.getAuxIDs();

      // The auxiliary type registry:
      SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();

      // Loop over them:
      for( SG::auxid_t auxid : auxids ) {

         // Check/cache its type:
         if( m_typeCache.size() <= auxid ) {
            m_typeCache.resize( auxid + 1 );
         }
         if( ! m_typeCache[ auxid ].isSet ) {
            const std::string tname =
               SG::normalizedTypeinfoName( *( reg.getType( auxid ) ) );
            static const std::string pat1 = "ElementLink<";
            static const std::string pat2 = "std::vector<ElementLink<";
            if( tname.substr( 0, pat1.size() ) == pat1 ) {
               m_typeCache[ auxid ].isEL = true;
            } else if( tname.substr( 0, pat2.size() ) == pat2 ) {
               m_typeCache[ auxid ].isELVec = true;
            }
            m_typeCache[ auxid ].isSet = true;
            ATH_MSG_VERBOSE( "Type for \"" << tname << "\": isEL = "
                             << m_typeCache[ auxid ].isEL << ", isELVec = "
                             << m_typeCache[ auxid ].isELVec );
         }

         // If it's not an EL type, then don't bother:
         if( ! ( m_typeCache[ auxid ].isEL || m_typeCache[ auxid ].isELVec ) ) {
            continue;
         }

         // Get a pointer to the vector variable. We need to cast away
         // its constness in this ugly way, we can't afford to only ask for
         // non-const pointers from the store. Since the ElementLink to be
         // reset may very well be a const variable that was accessed already,
         // and now needs its cache wiped.
         void* ptr = const_cast< void* >( store.getData( auxid ) );

         // If the pointer is null, then something dodgy happened with this
         // (dynamic) variable.
         if( ! ptr ) {
            // Check if this is a static variable. If it is, it's not an error
            // to get a null pointer for it. Since such a case can only happen
            // when a new static variable was introduced into the EDM, and we're
            // reading an old input file that doesn't have this variable in it
            // yet. Which is an okay scenario.
            const SG::IAuxStoreIO* storeIO =
               dynamic_cast< const SG::IAuxStoreIO* >( &store );
            if( ( ! storeIO ) || ( storeIO->getDynamicAuxIDs().find( auxid ) !=
                                   storeIO->getDynamicAuxIDs().end() ) ) {
               REPORT_MESSAGE( MSG::ERROR )
                  << "Invalid pointer received for variable: " << key
                  << reg.getName( auxid );
            } else {
               ATH_MSG_DEBUG( "Static variable " << key << reg.getName( auxid )
                              << " is empty" );
            }
            continue;
         }

         // Get the variable's element size:
         const size_t eltSize = reg.getEltSize( auxid );

         // Loop over its elements:
         const size_t sz_i = store.size();
         for( size_t i = 0; i < sz_i; ++i ) {

            // Raw pointer to the object:
            void* eltPtr = reinterpret_cast< char* >( ptr ) + i * eltSize;

            // Do different things based on the variable type:
            if( m_typeCache[ auxid ].isEL ) {
               // Cast to an ElementLinkBase object:
               ElementLinkBase* elb =
                  reinterpret_cast< ElementLinkBase* >( eltPtr );
               // Reset the link only if it has its persistent variable(s) set:
               if( elb->persKey() ) {
                  elb->toTransient();
               }
            } else if( m_typeCache[ auxid ].isELVec ) {
               // For a vector of links we heavily rely on knowing how GCC/Clang
               // lays out the memory for vectors.
               std::vector< ElementLinkBase >& v =
                  *( reinterpret_cast< std::vector< ElementLinkBase >* >( eltPtr ) );
               const size_t sz_j = v.size();
               for( size_t j = 0; j < sz_j; ++j ) {
                  // Access the link:
                  ElementLinkBase& elb = v[ j ];
                  // Reset the link only if it has its persistent variable(s)
                  // set:
                  if( elb.persKey() ) {
                     elb.toTransient();
                  }
               }
            } else {
               ATH_MSG_FATAL( "There is a logic error in the code" );
               return StatusCode::FAILURE;
            }
         }
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   ElementLinkResetAlg::AuxIDType::AuxIDType()
      : isSet( false ), isEL( false ), isELVec( false ) {

   }

} // namespace xAODMaker
