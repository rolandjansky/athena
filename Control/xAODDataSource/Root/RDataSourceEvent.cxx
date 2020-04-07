//
// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "RDataSourceEvent.h"

// xAOD include(s).
#include "xAODRootAccess/tools/Message.h"
#include "xAODRootAccess/tools/Utils.h"
#include "xAODRootAccess/tools/TObjectManager.h"
#include "xAODRootAccess/tools/THolder.h"

// ROOT include(s).
#include <TClass.h>
#include <TTree.h>
#include <TError.h>

// System include(s).
#include <functional>

namespace xAOD {

   RDataSourceEvent::RDataSourceEvent()
      : TEvent( TEvent::kClassAccess ) {

   }

   std::vector< std::pair< std::string, std::string > >
   RDataSourceEvent::columnAndTypeNames() {

      // This makes sure that the input is connected to.
      if( ( ! m_inTree ) && ( getEntry( 0 ) < 0 ) ) {
         ::Error( "xAOD::RDataSourceEvent::columnAndTypeNames",
                  XAOD_MESSAGE( "No input file opened yet" ) );
         throw std::runtime_error( "No input file opened yet" );
      }

      // A sanity check.
      if( ! inputEventFormat() ) {
         ::Error( "xAOD::RDataSourceEvent::columnAndTypeNames",
                 XAOD_MESSAGE( "No xAOD::EventFormat object available" ) );
         throw std::runtime_error( "No xAOD::EventFormat object available" );
      }

      // The result object.
      std::vector< std::pair< std::string, std::string > > result;

      // Loop over the known objects.
      EventFormat::const_iterator ef_itr = inputEventFormat()->begin();
      EventFormat::const_iterator ef_end = inputEventFormat()->end();
      for( ; ef_itr != ef_end; ++ef_itr ) {

         // A convenient reference.
         const xAOD::EventFormatElement& efe = ef_itr->second;

         // Skip auxiliary containers.
         if( efe.branchName().rfind( "Aux." ) ==
             ( efe.branchName().size() - 4 ) ) {
            continue;
         }
         // Skip dynamic branches.
         if( efe.parentName() != "" ) {
            continue;
         }
         // Skip unavailable branches.
         if( ! m_inTree->GetBranch( efe.branchName().c_str() ) ) {
            continue;
         }
         // Skip unknown types.
         ::TClass* cl = ::TClass::GetClass( efe.className().c_str() );
         if( ! cl ) {
            continue;
         }
         // Skip auxiliary containers that are mislabeled.
         if( cl->InheritsFrom( "SG::IConstAuxStore" ) ) {
            continue;
         }

         // Make sure that we have a functional dictionary for the type.
         const std::type_info* ti = cl->GetTypeInfo();
         if( ! ti ) {
            ::Warning( "xAOD::RDataSourceEvent::columnAndTypeNames",
                       "No std::type_info available for type %s (key=%s)",
                       cl->GetName(), efe.branchName().c_str() );
            continue;
         }

         // And do one final check that we can really read this type.
         if( ! contains( efe.branchName(), *ti ) ) {
            continue;
         }

         // Remember this objects.
         result.emplace_back( efe.branchName(), efe.className() );
      }

      // Return the collected results.
      return result;
   }

   void* RDataSourceEvent::columnReader( std::string_view columnName,
                                         const std::type_info& typeInfo ) {

      // Make sure that this is the active event in the current thread.
      setActive();

      // Create/access the pointer for this object.
      return &( m_objects[ std::make_pair( std::string( columnName ),
                                           &typeInfo ) ] );
   }

   void RDataSourceEvent::updateObjectsForEntry( Long64_t entry ) {

      // Make sure that this is the active event in the current thread.
      setActive();

      // Switch to the new entry.
      if( getEntry( entry ) < 0 ) {
         ::Error( "xAOD::RDataSourceEvent::updateObjectsForEntry",
                  XAOD_MESSAGE( "Failed to get entry %lld" ), entry );
         throw std::runtime_error( "Failed to get entry" );
      }

      // Loop over all "registered" objects.
      for( auto& obj : m_objects ) {
         // (Re-)Access this object.
         obj.second = getInputObject( obj.first.first, *( obj.first.second ) );
      }

      // Return gracefully.
      return;
   }

   std::size_t
   RDataSourceEvent::key_hash::operator()( const Key_t& key ) const {

      return std::hash< std::string >()( key.first + key.second->name() );
   }

} // namespace xAOD
