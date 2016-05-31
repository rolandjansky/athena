/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ElementLinkResetAlg.cxx 751107 2016-05-31 11:23:23Z krasznaa $

// EDM include(s):
#include "AthContainersInterfaces/IConstAuxStore.h"
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
      std::vector< const SG::IConstAuxStore* > stores;
      if( m_keys.size() ) {
         for( const std::string& key : m_keys ) {
            const SG::IConstAuxStore* store = 0;
            ATH_CHECK( evtStore()->retrieve( store, key ) );
            stores.push_back( store );
         }
      } else {
         SG::ConstIterator< SG::IConstAuxStore > begin, end;
         ATH_CHECK( evtStore()->retrieve( begin, end ) );
         for( auto itr = begin; itr != end; ++itr ) {
            const SG::IConstAuxStore* store = 0;
            ATH_CHECK( evtStore()->retrieve( store, itr.key() ) );
            stores.push_back( store );
         }
      }
      ATH_MSG_DEBUG( "Number of IConstAuxStore objects retrieved: "
                     << stores.size() );

      // Reset the ElementLinks in all of them:
      for( const SG::IConstAuxStore* store : stores ) {
         ATH_MSG_VERBOSE( "Reseting element links in store: " << store );
         ATH_CHECK( reset( *store ) );
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode ElementLinkResetAlg::reset( const SG::IConstAuxStore& store ) {

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

         // Get the variable's element size:
         const size_t eltSize = reg.getEltSize( auxid );

         // Loop over its elements:
         const size_t sz_i = store.size();
         for( size_t i = 0; i < sz_i; ++i ) {

            // Raw pointer to the object:
            void* eltPtr = reinterpret_cast< char* >( ptr ) + i * eltSize;

            // Do different things based on the variable type:
            if( m_typeCache[ auxid ].isEL ) {
               // For a single ElementLink, the logic is relatively simple:
               reinterpret_cast< ElementLinkBase* >( eltPtr )->toTransient();
            } else if( m_typeCache[ auxid ].isELVec ) {
               // For a vector of links we heavily rely on knowing how GCC/Clang
               // lays out the memory for vectors.
               std::vector< ElementLinkBase >& v =
                  *( reinterpret_cast< std::vector< ElementLinkBase >* >( eltPtr ) );
               const size_t sz_j = v.size();
               for( size_t j = 0; j < sz_j; ++j ) {
                  v[ j ].toTransient();
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
