//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//

// Local include(s):
#include "xAODRootAccess/TTreeMgr.h"
#include "xAODRootAccess/tools/ReturnCheck.h"
#include "xAODRootAccess/tools/xAODTEventBranch.h"
#include "xAODRootAccess/tools/xAODTMetaBranch.h"

// ROOT include(s):
#include <TTree.h>
#include <TClass.h>
#include <TDirectory.h>
#include <TFile.h>
#include <TROOT.h>
#include <TRegexp.h>

// System include(s).
#include <memory>

namespace xAOD {

   TTreeMgr::TTreeMgr( TEvent::EAuxMode mode )
      : m_eventTree(), m_metaTree(),
        m_event( mode ), m_eventTreeName( "UNKNOWN" ),
        m_enableEventObj(), m_suppressEventObj(),
        m_enableMetaObj(), m_suppressMetaObj() {

   }

   TReturnCode TTreeMgr::readFrom( ::TFile* file, ::Bool_t useTreeCache,
                                   const char* treeName ) {

      // Delete the current transient tree(s):
      m_eventTree.reset();
      m_metaTree.reset();

      // Give the file to the TEvent object:
      RETURN_CHECK( "xAOD::TTreeMgr::readFrom",
                    m_event.readFrom( file, useTreeCache, treeName ) );
      // Load the first event from the file.
      if( m_event.getEntry( 0 ) < 0 ) {
         ::Error( "xAOD::TTreeMgr::readFrom",
                  "Couldn't load the first event from file \"%s\"",
                  file->GetName() );
         return TReturnCode::kFailure;
      }

      // Remember the event tree name:
      m_eventTreeName = treeName;

      // Return gracefully:
      return TReturnCode::kSuccess;
   }

   TReturnCode
   TTreeMgr::enableEventObj( const std::vector< std::string >& names ) {

      // If the event tree already exists, this call will have no effect:
      if( m_eventTree ) {
         ::Warning( "xAOD::TTreeMgr::enableEventObj",
                    "Event tree already created, can't filter its contents "
                    "anymore" );
         return TReturnCode::kRecoverable;
      }

      // Remember the selection:
      m_enableEventObj = names;

      // Return gracefully:
      return TReturnCode::kSuccess;
   }

   TReturnCode
   TTreeMgr::suppressEventObj( const std::vector< std::string >& names ) {

      // If the event tree already exists, this call will have no effect:
      if( m_eventTree ) {
         ::Warning( "xAOD::TTreeMgr::suppressEventObj",
                    "Event tree already created, can't filter its contents "
                    "anymore" );
         return TReturnCode::kRecoverable;
      }

      // Remember the selection:
      m_suppressEventObj = names;

      // Return gracefully:
      return TReturnCode::kSuccess;
   }

   TReturnCode
   TTreeMgr::enableMetaObj( const std::vector< std::string >& names ) {

      // If the metadata tree already exists, this call will have no effect:
      if( m_metaTree ) {
         ::Warning( "xAOD::TTreeMgr::enableMetaObj",
                    "Metadata tree already created, can't filter its contents "
                    "anymore" );
         return TReturnCode::kRecoverable;
      }

      // Remember the selection:
      m_enableMetaObj = names;

      // Return gracefully:
      return TReturnCode::kSuccess;
   }

   TReturnCode
   TTreeMgr::suppressMetaObj( const std::vector< std::string >& names ) {

      // If the metadata tree already exists, this call will have no effect:
      if( m_metaTree ) {
         ::Warning( "xAOD::TTreeMgr::suppressMetaObj",
                    "Metadata tree already created, can't filter its contents "
                    "anymore" );
         return TReturnCode::kRecoverable;
      }

      // Remember the selection:
      m_suppressMetaObj = names;

      // Return gracefully:
      return TReturnCode::kSuccess;
   }

   ::TTree* TTreeMgr::eventTree() {

      // If the tree is available, return it right away:
      if( m_eventTree ) {
         return m_eventTree.get();
      }

      // Get the input file's format:
      const EventFormat* ef = m_event.inputEventFormat();
      if( ! ef ) {
         ::Error( "xAOD::TTreeMgr::eventTree",
                  XAOD_MESSAGE( "Couldn't get the input file's format "
                                "metadata. Did you use readFrom(...)?" ) );
         return 0;
      }

      {
         // Remember which directory we are in now:
         ::TDirectory* dir = gDirectory;

         // Create the transient tree:
         gROOT->cd();
         m_eventTree.reset( new xAODTEventTree( m_event,
                                                m_eventTreeName.c_str() ) );

         // Go back to the original directory:
         dir->cd();
      }

      // Create a branch for each xAOD interface object:
      for( auto ef_itr : *ef ) {

         // Convenience reference:
         const xAOD::EventFormatElement& efe = ef_itr.second;

         // Just skip all branches ending in "Aux.":
         if( efe.branchName().rfind( "Aux." ) ==
             ( efe.branchName().size() - 4 ) ) {
            continue;
         }
         // Also skip dynamic branches:
         if( efe.parentName() != "" ) {
            continue;
         }

         // Apply the user specified filtering:
         if( m_enableEventObj.size() ) {
            bool accept = false;
            for( const std::string& name : m_enableEventObj ) {
               TRegexp re( name.c_str() );
               Ssiz_t dummy = 0;
               if( re.Index( efe.branchName().c_str(), &dummy ) >= 0 ) {
                  accept = true;
                  break;
               }
            }
            if( ! accept ) {
               continue;
            }
         }
         {
            bool accept = true;
            for( const std::string& name : m_suppressEventObj ) {
               TRegexp re( name.c_str() );
               Ssiz_t dummy = 0;
               if( re.Index( efe.branchName().c_str(), &dummy ) >= 0 ) {
                  accept = false;
                  break;
               }
            }
            if( ! accept ) {
               continue;
            }
         }

         // Don't bother if the branch is not available:
         if( ! m_event.m_inTree->GetBranch( efe.branchName().c_str() ) ) {
            continue;
         }

         // Check if the class in question is known:
         ::TClass* cl = ::TClass::GetClass( efe.className().c_str(), kTRUE,
                                            kTRUE );
         if( ! cl ) continue;

         // Don't add auxiliary store objects:
         if( cl->InheritsFrom( "SG::IConstAuxStore" ) ) {
            continue;
         }

         // Check if we have a type-info for this class:
         const std::type_info* ti = cl->GetTypeInfo();
         if( ! ti ) {
            ::Warning( "xAOD::TTreeMgr::eventTree",
                       "Couldn't find std::type_info object for type %s "
                       "(key=%s)", cl->GetName(), efe.branchName().c_str() );
            continue;
         }

         // Check if the object is actually available on the input:
         if( ! m_event.contains( efe.branchName(), *ti ) ) {
            continue;
         }

         // If everything is right, let's make the branch:
         auto br =
            std::make_unique< xAODTEventBranch >( *m_eventTree, m_event, *ti,
                                                  efe.branchName().c_str(),
                                                  efe.className().c_str() );
         m_eventTree->AddBranch( std::move( br ) );
      }

      // Return the newly created object:
      return m_eventTree.get();
   }

   ::TTree* TTreeMgr::metaTree() {

      // If the tree is available, return it right away:
      if( m_metaTree ) {
         return m_metaTree.get();
      }

      // Make sure that the persistent TTree is available:
      if( ! m_event.m_inMetaTree ) {
         ::Error( "xAOD::TTreeMgr::metaTree",
                  XAOD_MESSAGE( "Couldn't get the input file's metatate "
                                "tree. Did you use readFrom(...)?" ) );
         return 0;
      }

      {
         // Remember which directory we are in now:
         ::TDirectory* dir = gDirectory;

         // Create the transient tree:
         gROOT->cd();
         m_metaTree.reset( new xAODTMetaTree( m_event ) );

         // Go back to the original directory:
         dir->cd();
      }

      // Loop over the branches of the persistent metadata tree:
      TObjArray* branches = m_event.m_inMetaTree->GetListOfBranches();
      for( ::Int_t i = 0; i < branches->GetEntries(); ++i ) {

         // Access the branch object:
         ::TBranch* br = dynamic_cast< ::TBranch* >( branches->At( i ) );
         if( ! br ) {
            ::Error( "xAOD::TTreeMgr::metaTree",
                     XAOD_MESSAGE( "Couldn't access branch %i as a TBranch" ),
                     i );
            continue;
         }

         // Skip the EventFormat branch. That must not be disturbed by the
         // generic metadata handling.
         if( ! strcmp( br->GetName(), "EventFormat" ) ) continue;

         // Apply the user specified filtering:
         if( m_enableMetaObj.size() ) {
            bool accept = false;
            for( const std::string& name : m_enableMetaObj ) {
               TRegexp re( name.c_str() );
               Ssiz_t dummy = 0;
               if( re.Index( br->GetName(), &dummy ) >= 0 ) {
                  accept = true;
                  break;
               }
            }
            if( ! accept ) {
               continue;
            }
         }
         {
            bool accept = true;
            for( const std::string& name : m_suppressMetaObj ) {
               TRegexp re( name.c_str() );
               Ssiz_t dummy = 0;
               if( re.Index( br->GetName(), &dummy ) >= 0 ) {
                  accept = false;
                  break;
               }
            }
            if( ! accept ) {
               continue;
            }
         }

         // Check whether we have a dictionary for this type:
         ::TClass* cl = ::TClass::GetClass( br->GetClassName(), kTRUE, kTRUE );
         if( ! cl ) continue;

         // Check if we have a type-info for this class:
         const std::type_info* ti = cl->GetTypeInfo();
         if( ! ti ) continue;

         // Don't add auxiliary store objects:
         if( cl->InheritsFrom( "SG::IConstAuxStore" ) ) {
            continue;
         }

         // If everything is right, let's make the branch:
         auto mbr =
            std::make_unique< xAODTMetaBranch >( *m_metaTree, m_event, *ti,
                                                 br->GetName(),
                                                 br->GetClassName() );
         m_metaTree->AddBranch( std::move( mbr ) );
      }

      // Return the now created object:
      return m_metaTree.get();
   }

   TEvent& TTreeMgr::event() {

      // Return a reference to the object:
      return m_event;
   }

} // namespace xAOD
