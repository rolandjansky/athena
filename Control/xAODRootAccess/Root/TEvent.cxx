/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TEvent.cxx 796983 2017-02-14 05:09:12Z ssnyder $

// System include(s):
#include <cassert>
#include <sstream>
#include <iomanip>
#include <cstring>

// ROOT include(s):
#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TChainElement.h>
#include <TBranch.h>
#include <TError.h>
#include <TMethodCall.h>
#include <TSystem.h>

// Gaudi/Athena include(s):
#include "AthContainersInterfaces/IAuxStoreIO.h"
#include "AthContainersInterfaces/IAuxStoreHolder.h"
#include "AthContainers/AuxTypeRegistry.h"
#define private public
#   include "AthContainers/AuxVectorBase.h"
#undef private
#include "AthContainers/AuxElement.h"
#include "AthContainers/normalizedTypeinfoName.h"
#ifndef XAOD_STANDALONE
#   include "SGTools/CurrentEventStore.h"
#endif // not XAOD_STANDALONE

// Interface include(s):
#include "xAODRootAccessInterfaces/TActiveEvent.h"

// xAOD include(s):
#include "xAODCore/tools/IOStats.h"
#include "xAODCore/tools/ReadStats.h"
#include "xAODCore/tools/PerfStats.h"

// Local include(s):
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TAuxStore.h"
#include "xAODRootAccess/TVirtualIncidentListener.h"
#include "xAODRootAccess/TStore.h"
#include "xAODRootAccess/TActiveStore.h"
#include "xAODRootAccess/tools/TAuxManager.h"
#include "xAODRootAccess/tools/TObjectManager.h"
#include "xAODRootAccess/tools/TAuxBranchManager.h"
#include "xAODRootAccess/tools/TPrimitiveAuxBranchManager.h"
#include "xAODRootAccess/tools/THolder.h"
#include "xAODRootAccess/tools/Utils.h"
#include "xAODRootAccess/tools/Message.h"
#include "xAODRootAccess/tools/TEventFormatRegistry.h"
#include "xAODRootAccess/tools/TDirectoryReset.h"
#include "xAODRootAccess/tools/TIncident.h"
#include "xAODRootAccess/tools/ReturnCheck.h"
#include "xAODRootAccess/tools/TChainStateTracker.h"
#include "xAODRootAccess/tools/TFileAccessTracer.h"

namespace {

   /// Simple helper function for removing the _vX postfixes from type
   /// name strings.
   ///
   /// @param name Type name that should be trimmed
   ///
   void removeVersionNames( std::string& name ) {

      size_t pos = 0;
      while( ( pos = name.find( "_v1" ) ) != name.npos ) {
         name.erase( pos, 3 );
      }
      while( ( pos = name.find( "_v2" ) ) != name.npos ) {
         name.erase( pos, 3 );
      }
      while( ( pos = name.find( "_v3" ) ) != name.npos ) {
         name.erase( pos, 3 );
      }
      while( ( pos = name.find( "_v4" ) ) != name.npos ) {
         name.erase( pos, 3 );
      }
      while( ( pos = name.find( "_v5" ) ) != name.npos ) {
         name.erase( pos, 3 );
      }
      while( ( pos = name.find( "_v6" ) ) != name.npos ) {
         name.erase( pos, 3 );
      }
      while( ( pos = name.find( "_v7" ) ) != name.npos ) {
         name.erase( pos, 3 );
      }
      while( ( pos = name.find( "_v8" ) ) != name.npos ) {
         name.erase( pos, 3 );
      }

      return;
   }

} // private namespace

namespace xAOD {

   //
   // Initialise the static data:
   //
   const ::Int_t TEvent::CACHE_SIZE = 30000000;
   const char* TEvent::EVENT_TREE_NAME    = "CollectionTree";
   const char* TEvent::METADATA_TREE_NAME = "MetaData";

   /// Helper variable
   static const TEvent::EAuxMode DEFAULT_ACCESS_MODE = TEvent::kClassAccess;
   /// Helper variable
   static const TEvent::EAuxMode UNIT_TEST_ACCESS_MODE = TEvent::kAthenaAccess;

   TEvent::TEvent( EAuxMode mode )
      : m_auxMode( mode ),
        m_inTree( 0 ), m_inTreeMissing( kFALSE ),
        m_inChain( 0 ), m_inChainTracker( 0 ),
        m_inTreeNumber( -1 ), m_inMetaTree( 0 ),
        m_entry( 0 ), m_outTree( 0 ),
        m_inputObjects(), m_outputObjects(),
        m_inputMetaObjects(), m_outputMetaObjects(),
        m_inputEventFormat(), m_outputEventFormat( 0 ),
        m_auxItemList(), m_listeners(), m_nameRemapping() {

      // Make sure that the I/O monitoring is active:
      PerfStats::instance();

      // Make this the active event:
      setActive();

      // If the user didn't ask for one particular access type, chose one
      // based on the running environment:
      if( m_auxMode == kUndefinedAccess ) {
         if( gSystem->Getenv( "ROOTCORE_AUTO_UT" ) ||
             gSystem->Getenv( "ROOTCORE_FAST_UT" ) ||
             gSystem->Getenv( "ROOTCORE_SLOW_UT" ) ) {
            ::Info( "xAOD::TEvent::TEvent",
                    "Using access mode \"%i\" for the unit test",
                    static_cast< int >( UNIT_TEST_ACCESS_MODE ) );
            m_auxMode = UNIT_TEST_ACCESS_MODE;
         } else {
            m_auxMode = DEFAULT_ACCESS_MODE;
         }
      }
   }

   TEvent::TEvent( ::TFile* file, EAuxMode mode )
      : m_auxMode( mode ),
        m_inTree( 0 ), m_inTreeMissing( kFALSE ),
        m_inChain( 0 ), m_inChainTracker( 0 ),
        m_inTreeNumber( -1 ), m_inMetaTree( 0 ),
        m_entry( 0 ), m_outTree( 0 ),
        m_inputObjects(), m_outputObjects(),
        m_inputMetaObjects(), m_outputMetaObjects(),
        m_inputEventFormat(), m_outputEventFormat( 0 ),
        m_auxItemList(), m_listeners(), m_nameRemapping() {

      // Make sure that the I/O monitoring is active:
      PerfStats::instance();

      // Make this the active event:
      setActive();

      // If the user didn't ask for one particular access type, chose one
      // based on the running environment:
      if( m_auxMode == kUndefinedAccess ) {
         if( gSystem->Getenv( "ROOTCORE_AUTO_UT" ) ||
             gSystem->Getenv( "ROOTCORE_FAST_UT" ) ||
             gSystem->Getenv( "ROOTCORE_SLOW_UT" ) ) {
            ::Info( "xAOD::TEvent::TEvent",
                    "Using access mode \"%i\" for the unit test",
                    static_cast< int >( UNIT_TEST_ACCESS_MODE ) );
            m_auxMode = UNIT_TEST_ACCESS_MODE;
         } else {
            m_auxMode = DEFAULT_ACCESS_MODE;
         }
      }

      // Let the initialisation function deal with setting up the object:
      readFrom( file ).ignore();
   }

   TEvent::TEvent( ::TTree* tree, EAuxMode mode )
      : m_auxMode( mode ),
        m_inTree( 0 ), m_inTreeMissing( kFALSE ),
        m_inChain( 0 ), m_inChainTracker( 0 ),
        m_inTreeNumber( -1 ), m_inMetaTree( 0 ),
        m_entry( 0 ), m_outTree( 0 ),
        m_inputObjects(), m_outputObjects(),
        m_inputMetaObjects(), m_outputMetaObjects(),
        m_inputEventFormat(), m_outputEventFormat( 0 ),
        m_auxItemList(), m_listeners(), m_nameRemapping() {

      // Make sure that the I/O monitoring is active:
      PerfStats::instance();

      // Make this the active event:
      setActive();

      // If the user didn't ask for one particular access type, chose one
      // based on the running environment:
      if( m_auxMode == kUndefinedAccess ) {
         if( gSystem->Getenv( "ROOTCORE_AUTO_UT" ) ||
             gSystem->Getenv( "ROOTCORE_FAST_UT" ) ||
             gSystem->Getenv( "ROOTCORE_SLOW_UT" ) ) {
            ::Info( "xAOD::TEvent::TEvent",
                    "Using access mode \"%i\" for the unit test",
                    static_cast< int >( UNIT_TEST_ACCESS_MODE ) );
            m_auxMode = UNIT_TEST_ACCESS_MODE;
         } else {
            m_auxMode = DEFAULT_ACCESS_MODE;
         }
      }

      // Let the initialisation function deal with setting up the object:
      readFrom( tree ).ignore();
   }

   TEvent::~TEvent() {

      Object_t::iterator itr = m_inputObjects.begin();
      Object_t::iterator end = m_inputObjects.end();
      for( ; itr != end; ++itr ) {
         delete itr->second;
      }
      itr = m_outputObjects.begin();
      end = m_outputObjects.end();
      for( ; itr != end; ++itr ) {
         delete itr->second;
      }
      itr = m_inputMetaObjects.begin();
      end = m_inputMetaObjects.end();
      for( ; itr != end; ++itr ) {
         delete itr->second;
      }
      itr = m_outputMetaObjects.begin();
      end = m_outputMetaObjects.end();
      for( ; itr != end; ++itr ) {
         delete itr->second;
      }

      if( m_inChainTracker ) {
         delete m_inChainTracker;
      }

      // If this is set up as the active event at the moment, notify
      // the active event object that this object will no longer be
      // available.
      if( TActiveEvent::event() == this ) {
         TActiveEvent::setEvent( nullptr );
      }
#ifndef XAOD_STANDALONE
      if( SG::CurrentEventStore::store() == this ) {
         SG::CurrentEventStore::setStore( nullptr );
      }
#endif // not XAOD_STANDALONE
   }

   /// @returns The auxiliary data access mode currently in use
   ///
   TEvent::EAuxMode TEvent::auxMode() const {

      return m_auxMode;
   }

   /// This function behaves exactly like StoreGateSvc::dump(). It doesn't
   /// actually print anything to the screen, it just returns a user
   /// readable dump of the contents of the current input file/chain.
   ///
   /// It is a pretty dumb implementation for the moment. Should be made
   /// nicer later on.
   ///
   /// @returns The user-readable contents of the current input file/chain
   ///
   std::string TEvent::dump() {

      // The internal stream object:
      std::ostringstream ost;
      ost << "<<<<<<<<<<<<<<<<<<<< xAOD::TEvent Dump >>>>>>>>>>>>>>>>>>>>\n";

      // Loop over the EventFormat object:
      EventFormat::const_iterator ef_itr = m_inputEventFormat.begin();
      EventFormat::const_iterator ef_end = m_inputEventFormat.end();
      for( ; ef_itr != ef_end; ++ef_itr ) {

         // Construct the type name:
         std::string typeName = ef_itr->second.className();
         removeVersionNames( typeName );

         // Get the type:
         ::TClass* cl =
              ::TClass::GetClass( ef_itr->second.className().c_str() );
         const std::type_info* ti = cl->GetTypeInfo();
         if( ( ! cl->IsLoaded() ) || ( ! ti ) ) {
            Warning( "xAOD::TEvent::dump",
                     "Unknown type (%s) found in the event format",
                     ef_itr->second.className().c_str() );
            continue;
         }

         // Skip containers that are not available anyway:
         if( ! contains( ef_itr->second.branchName(), *ti ) ) {
            continue;
         }

         // Do the printout:
         ost << " Hash: 0x" << std::setw( 8 ) << std::setfill( '0' )
             << std::hex << ef_itr->second.hash()
             << " Key: \"" << ef_itr->second.branchName() << "\"\n";

         ost << "   type: " << typeName << "\n";
         const bool isNonConst = transientContains( ef_itr->second.branchName(),
                                                    *ti );
         ost << "   isConst: " << ( isNonConst ? "No" : "Yes" ) << "\n";
         ost << "   Data: "
             << ( isNonConst ? getOutputObject( ef_itr->second.branchName(),
                                                *ti ) :
                  getInputObject( ef_itr->second.branchName(), *ti ) ) << "\n";
      }

      // Finish with the construction:
      ost << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>";
      return ost.str();
   }

   /// This is a convenience function for printing basic I/O information about
   /// the current job. It can be called at the end of a job to get an overview
   /// of what the job did exactly I/O-wise.
   ///
   void TEvent::printIOStats() const {

      // Simply do this via the xAODCore code:
      IOStats::instance().stats().Print( "Summary" );

      return;
   }

   /// This function takes care of connecting the event object to a new input
   /// file. It reads in the metadata of the input file needed for reading
   /// the file.
   ///
   /// @param file Pointer to the file being read
   /// @param useTreeCache Flag for turning on/off the usage of TTreeCache
   /// @param treeName Name of the input tree
   /// @returns <code>kTRUE</code> if successful, <code>kFALSE</code> otherwise
   ///
   TReturnCode TEvent::readFrom( ::TFile* file, Bool_t useTreeCache,
                                 const char* treeName ) {

      // If no file was specified, return gracefully:
      if( ! file ) return TReturnCode::kSuccess;

      // Clear the cached input objects:
      Object_t::iterator itr = m_inputObjects.begin();
      Object_t::iterator end = m_inputObjects.end();
      for( ; itr != end; ++itr ) {
         delete itr->second;
      }
      m_inputObjects.clear();
      m_branches.clear();

      // Clear the cached input meta-objects:
      itr = m_inputMetaObjects.begin();
      end = m_inputMetaObjects.end();
      for( ; itr != end; ++itr ) {
         delete itr->second;
      }
      m_inputMetaObjects.clear();

      // Reset the internal flag:
      m_inTreeMissing = kFALSE;

      // Make sure we return to the current directory:
      TDirectoryReset dr;

      // Set up the file access tracer:
      static TFileAccessTracer tracer;
      tracer.add( *file );

      // Look for the metadata tree:
      m_inMetaTree =
         dynamic_cast< ::TTree* >( file->Get( METADATA_TREE_NAME ) );
      if( ! m_inMetaTree ) {
         ::Error( "xAOD::TEvent::readFrom",
                  XAOD_MESSAGE( "Couldn't find metadata tree on input. Object "
                                "unusable!" ) );
         return TReturnCode::kFailure;
      }

      // A sanity check:
      if( m_inMetaTree->GetEntries() != 1 ) {
         ::Info( "xAOD::TEvent::readFrom",
                 "Was expecting a metadata tree with size 1, instead of %i.",
                 static_cast< int >( m_inMetaTree->GetEntries() ) );
         ::Info( "xAOD::TEvent::readFrom",
                 "File most probably produced by hadd..." );
      }

      // Make sure that the xAOD::EventFormat dictonary is loaded.
      // This may not be the case if streamer information reading is turned
      // off.
      static const std::string eventFormatTypeName =
         Utils::getTypeName( typeid( EventFormat ) );
      ::TClass* cl = ::TClass::GetClass( eventFormatTypeName.c_str() );
      if( ! cl ) {
         ::Warning( "xAOD::TEvent::readFrom",
                    "Couldn't load the EventFormat dictionary" );
      }

      // Check if the EventFormat branch is available:
      static const char* EVENTFORMAT_BRANCH_NAME = "EventFormat";
      if( ! m_inMetaTree->GetBranch( EVENTFORMAT_BRANCH_NAME ) ) {
         // This can happen when the file was produced by an Athena job that
         // didn't have any input events itself. This means that the file
         // doesn't actually have any useful metadata.
         ::Info( "xAOD::TEvent::readFrom", "Input file provides no event or "
                 "metadata" );
         m_inTree = 0;
         m_inTreeMissing = kTRUE;
         return TReturnCode::kSuccess;
      }

      // Read in the event format object:
      EventFormat* format = 0; ::TBranch* br = 0;
      const Int_t status =
         m_inMetaTree->SetBranchAddress( EVENTFORMAT_BRANCH_NAME,
                                         &format, &br );
      if( status < 0 ) {
         ::Error( "xAOD::TEvent::readFrom",
                  XAOD_MESSAGE( "Failed to connect to EventFormat object" ) );
         return TReturnCode::kFailure;
      }

      // Read in the object to our private member:
      br->GetEntry( 0 );
      m_inputEventFormat = *format;

      // This is a strange place. The object has to be deleted, as it is the
      // responsibility of the user code to do so. But if I also explicitly
      // tell the branch to forget about the address of the pointer, then
      // all hell breaks lose...
      delete format;

      // Look for the event tree in the input file:
      m_inTree = dynamic_cast< ::TTree* >( file->Get( treeName ) );
      if( ! m_inTree ) {
         // This is no longer an error condition. As it can happen for DxAODs
         // that don't have any events in them. But they still have metadata
         // that needs to be collected.
         m_inTreeMissing = kTRUE;
      }

      // Turn on the cache if requested:
      if( m_inTree && useTreeCache && ( ! m_inTree->GetCacheSize() ) ) {
         m_inTree->SetCacheSize( CACHE_SIZE );
         m_inTree->SetCacheLearnEntries( 10 );
      }

      // Init the statistics collection:
      RETURN_CHECK( "xAOD::TEvent::readFrom", initStats() );
      // Update the event counter in the statistics object:
      xAOD::ReadStats& stats = IOStats::instance().stats();
      if( m_inTree ) {
         stats.setNEvents( stats.nEvents() + m_inTree->GetEntries() );
      }

      // Notify the listeners that a new file was opened:
      const TIncident beginIncident( IncidentType::BeginInputFile );
      for( TVirtualIncidentListener* listener : m_listeners ) {
         listener->handle( beginIncident );
      }
      // For now implement a very simple scheme in which we claim already
      // at the start that the entire file was processed. Since we have no way
      // of ensuring that the user indeed does this. And we can't delay calling
      // this function, as the user may likely close his/her output file before
      // closing the last opened input file.
      const TIncident endIncident( IncidentType::EndInputFile );
      for( TVirtualIncidentListener* listener : m_listeners ) {
         listener->handle( endIncident );
      }

      // The initialisation was successful:
      return TReturnCode::kSuccess;
   }

   /// This version of the function sets up the object to readin information
   /// from a tree/chain. Using it with a TTree pointer makes not much sense,
   /// but using it with a TChain pointer could be a very valid usage mode.
   ///
   /// @param tree The pointer to a TTree or a TChain
   /// @param useTreeCache Flag for switching TTreeCache usage on/off
   /// @returns <code>kTRUE</code> if successful, <code>kFALSE</code> when not
   ///
   TReturnCode TEvent::readFrom( ::TTree* tree, Bool_t useTreeCache ) {

      // Remember the info:
      m_inTree = 0;
      m_inTreeMissing = kFALSE;
      m_inChain = dynamic_cast< ::TChain* >( tree );
      m_inMetaTree = 0;

      if( m_inChain ) {

         // Set up the caching on the chain level. The individual trees of the
         // input files will get a cache set up automatically after this.
         if( useTreeCache && ( ! m_inChain->GetCacheSize() ) ) {
            m_inChain->SetCacheSize( CACHE_SIZE );
            m_inChain->SetCacheLearnEntries( 10 );
         }

         // Explicitly open the first file of the chain. To correctly auto-load
         // the dictionaries necessary. This doesn't happen automatically with
         // some ROOT versions...
         const TObjArray* files = m_inChain->GetListOfFiles();
         if( ! files ) {
            ::Error( "xAOD::TEvent::readFrom",
                     XAOD_MESSAGE( "Couldn't get the list of files from the "
                                   "input TChain" ) );
            return TReturnCode::kFailure;
         }
         if( ! files->GetEntries() ) {
            ::Error( "xAOD::TEvent::readFrom",
                     XAOD_MESSAGE( "No files are present in the received "
                                   "TChain" ) );
            return TReturnCode::kFailure;
         }
         const ::TChainElement* chEl =
              dynamic_cast< const ::TChainElement* >( files->At( 0 ) );
         if( ! chEl ) {
            ::Error( "xAOD::TEvent::readFrom",
                     XAOD_MESSAGE( "Couldn't cast object to TChainElement" ) );
            return TReturnCode::kFailure;
         }
         ::TFile* dummyFile = ::TFile::Open( chEl->GetTitle() );
         if( ! dummyFile ) {
            ::Error( "xAOD::TEvent::readFrom",
                     XAOD_MESSAGE( "Couldn't open file %s" ),
                                   chEl->GetTitle() );
            return TReturnCode::kFailure;
         }
         delete dummyFile;

         // Set up a tracker for the chain:
         if( ! m_inChainTracker ) {
            m_inChainTracker = new TChainStateTracker();
         }
         m_inChainTracker->reset();
         tree->SetNotify( m_inChainTracker );

         // Stop at this point. The first file will be opened when the user
         // asks for the first event. Otherwise we open the first file of the
         // chain multiple times.
         m_inTreeNumber = -1;
         return TReturnCode::kSuccess;

      } else {

         // If it's a simple TTree, then let's fully initialise the object
         // using its file:
         m_inTreeNumber = -1;
         if( m_inChainTracker ) {
            delete m_inChainTracker;
            m_inChainTracker = 0;
         }
         ::TFile* file = tree->GetCurrentFile();
         return readFrom( file, useTreeCache, tree->GetName() );

      }
   }

   /// This function should be called on a file opened be the user, before
   /// any event processing would occur. It sets up the output event tree.
   ///
   /// @param file The file that the event data should be written to
   /// @param autoFlush The auto-flush setting to use on the output TTree
   /// @param treeName Name of the output event tree
   /// @returns <code>kTRUE</code> if successful, <code>kFALSE</code> otherwise
   ///
   TReturnCode TEvent::writeTo( ::TFile* file, Int_t autoFlush,
                                const char* treeName ) {

      // Just a simple security check:
      if( ! file ) {
         ::Error( "xAOD::TEvent::writeTo",
                  XAOD_MESSAGE( "Null pointer given to the function!" ) );
         return TReturnCode::kFailure;
      }

      // Check that the object is in the "right state":
      if( m_outTree ) {
         ::Error( "xAOD::TEvent::writeTo",
                  XAOD_MESSAGE( "Object already writing to a file. Close that "
                                "file first!" ) );
         return TReturnCode::kFailure;
      }

      // Make sure we return to the current directory:
      TDirectoryReset dr;

      // Create the output TTree:
      file->cd();
      m_outTree = new ::TTree( treeName, "xAOD event tree" );
      m_outTree->SetDirectory( file );
      m_outTree->SetAutoSave( 1000000 );
      m_outTree->SetAutoFlush( autoFlush );

      // Access the EventFormat object associated with this file:
      m_outputEventFormat =
         &( TEventFormatRegistry::instance().getEventFormat( file ) );

      // Return gracefully:
      return TReturnCode::kSuccess;
   }

   /// This function needs to be called when the user is done writing events
   /// to a file, before (s)he would close the file itself.
   ///
   /// @param file The file that the event data is written to
   /// @returns <code>kTRUE</code> if successful, <code>kFALSE</code> otherwise
   ///
   TReturnCode TEvent::finishWritingTo( ::TFile* file ) {

      // A small sanity check:
      if( ! m_outTree ) {
         ::Error( "xAOD::TEvent::finishWritingTo",
                  XAOD_MESSAGE( "The object doesn't seem to be connected to an "
                                "output file!" ) );
         return TReturnCode::kFailure;
      }

      // Make sure we return to the current directory:
      TDirectoryReset dr;

      // Notify the listeners that they should write out their metadata, if they
      // have any.
      const TIncident incident( IncidentType::MetaDataStop );
      Listener_t::iterator l_itr = m_listeners.begin();
      Listener_t::iterator l_end = m_listeners.end();
      for( ; l_itr != l_end; ++l_itr ) {
         ( *l_itr )->handle( incident );
      }

      // Write out the event tree, and delete it:
      m_outTree->AutoSave( "FlushBaskets" );
      m_outTree->SetDirectory( 0 );
      delete m_outTree;
      m_outTree = 0;

      // Now go to the output file:
      file->cd();

      // Check if there's already a metadata tree in the output:
      if( file->Get( METADATA_TREE_NAME ) ) {
         // Let's assume that the metadata is complete in the file already.
         return TReturnCode::kSuccess;
      }

      // Create the metadata tree:
      ::TTree* metatree =
         new ::TTree( METADATA_TREE_NAME, "xAOD metadata tree" );
      metatree->SetAutoSave( 10000 );
      metatree->SetAutoFlush( -30000000 );
      metatree->SetDirectory( file );

      // Create the only branch in it:
      metatree->Branch( "EventFormat",
                        ClassName< xAOD::EventFormat >::name().c_str(),
                        &m_outputEventFormat );

      // Create a copy of the m_outputMetaObjects variable. This is necessary
      // because the putAux(...) function will modify this variable while we
      // loop over it.
      Object_t outputMetaObjects = m_outputMetaObjects;

      // Now loop over all the metadata objects that need to be put into the
      // output file:
      for( auto& object : outputMetaObjects ) {
         // All metadata objects should be held by TObjectManager objects.
         // Anything else is an error.
         TObjectManager* mgr = dynamic_cast< TObjectManager* >( object.second );
         if( ! mgr ) {
            ::Error( "xAOD::TEvent::finishWritingTo",
                     XAOD_MESSAGE( "Internal logic error detected" ) );
            return TReturnCode::kFailure;
         }
         // Select a split level depending on whether this is an interface or an
         // auxiliary object:
         const ::Int_t splitLevel = ( object.first.find( "Aux." ) ==
                                      ( object.first.size() - 4 ) ? 1 : 0 );
         // Create the new branch:
         *( mgr->branchPtr() ) =
            metatree->Branch( object.first.c_str(),
                              mgr->holder()->getClass()->GetName(),
                              mgr->holder()->getPtr(), 32000, splitLevel );
         if( ! mgr->branch() ) {
            ::Error( "xAOD::TEvent::finishWritingTo",
                     XAOD_MESSAGE( "Failed to create metadata branch "
                                   "\"%s/%s\"" ),
                     mgr->holder()->getClass()->GetName(),
                     object.first.c_str() );
            return TReturnCode::kFailure;
         }
         // Set up the saving of all the dynamic auxiliary properties
         // of the object if it has any:
         RETURN_CHECK( "xAOD::TEvent::finishWritingTo",
                       putAux( *metatree, *( object.second ), 32000, 0,
                               kTRUE ) );
      }

      // Write the metadata objects:
      if( metatree->Fill() <= 0 ) {
         ::Error( "xAOD::TEvent::finishWritingTo",
                  XAOD_MESSAGE( "Failed to write event format metadata into "
                                "the output" ) );
         metatree->SetDirectory( 0 );
         delete metatree;
         return TReturnCode::kFailure;
      }

      // Now clean up:
      metatree->Write();
      metatree->SetDirectory( 0 );
      delete metatree;
      m_outputEventFormat = 0;
      Object_t::iterator obj_itr = m_outputObjects.begin();
      Object_t::iterator obj_end = m_outputObjects.end();
      for( ; obj_itr != obj_end; ++obj_itr ) {
         delete obj_itr->second;
      }
      m_outputObjects.clear();
      obj_itr = m_outputMetaObjects.begin();
      obj_end = m_outputMetaObjects.end();
      for( ; obj_itr != obj_end; ++obj_itr ) {
         delete obj_itr->second;
      }
      m_outputMetaObjects.clear();

      // Return gracefully:
      return TReturnCode::kSuccess;
   }

   /// Setting TEvent objects happens automatically when reading a file, but
   /// when writing a file from scratch, the code doesn't necessarily know
   /// which TEvent object a given smart pointer should "belong to". This
   /// function helps the user with setting this up.
   ///
   void TEvent::setActive() const {

      // Do the deed. Since this needs both a static and a const cast at
      // the same time, let's just do the "brutal" cast instead of writing
      // way too much for the same thing...
      TActiveEvent::setEvent( ( TVirtualEvent* ) this );

#ifndef XAOD_STANDALONE
      SG::CurrentEventStore::setStore( const_cast< TEvent* >( this ) );
#endif // not XAOD_STANDALONE

      // Return gracefully:
      return;
   }

   /// This function receives the rules for selecting which dynamic auxiliary
   /// branches should be written for a given container, in the exact same
   /// format in which we need to set it in the Athena output ItemList.
   ///
   /// @param containerKey The name of the auxiliary container in question
   /// @param itemList The variable list according to the ... formatting rules
   ///
   void TEvent::setAuxItemList( const std::string& containerKey,
                                const std::string& itemList ) {

      // Decoded attributes:
      std::set< std::string > attributes;

      // Split up the received string using "." as the separator:
      if( itemList.size() ) {
         std::stringstream ss( itemList );
         std::string attr;
         while( std::getline( ss, attr, '.' ) ) {
            attributes.insert( attr );
         }
      }

      // Remember the setting:
      m_auxItemList[ containerKey ] = attributes;

      return;
   }

   /// This function works pretty much like IIncidentSvc::addListener does in
   /// Athena. It tells the TEvent object that when certain "interesting
   /// incidents" happen, a given object should be notified about it.
   ///
   /// @param listener Pointer to the object that should be notified
   /// @returns The usual TReturnCode types
   ///
   TReturnCode TEvent::addListener( TVirtualIncidentListener* listener ) {

      // Check that we received a valid pointer:
      if( ! listener ) {
         ::Error( "xAOD::TEvent::addListener",
                  XAOD_MESSAGE( "Received a null pointer for the listener" ) );
         return TReturnCode::kFailure;
      }

      // Check if this listener is already in our list:
      bool listenerKnown = false;
      for( TVirtualIncidentListener* l : m_listeners ) {
         if( l == listener ) {
            ::Warning( "xAOD::TEvent::addListener",
                       "Listener %p was added previously already",
                       static_cast< void* >( listener ) );
            listenerKnown = true;
            break;
         }
      }

      // If we don't know it yet, let's add it now:
      if( ! listenerKnown ) {
         m_listeners.push_back( listener );
      }

      // Return gracefully:
      return TReturnCode::kSuccess;
   }

   /// This function allows us to remove a listener when for instance a
   /// metadata tool is deleted during a job.
   ///
   /// @param listener Pointer to the listener that should be removed
   /// @returns The usual TReturnCode types
   ///
   TReturnCode TEvent::removeListener( TVirtualIncidentListener* listener ) {

      // Find the pointer if we can...
      Listener_t::iterator itr = std::find( m_listeners.begin(),
                                            m_listeners.end(), listener );

      // If we didn't find it:
      if( itr == m_listeners.end() ) {
         ::Error( "xAOD::TEvent::removeListener",
                  XAOD_MESSAGE( "Listener %p not known" ),
                  static_cast< void* >( listener ) );
         return TReturnCode::kFailure;
      }

      // Remove it:
      m_listeners.erase( itr );

      // Return gracefully:
      return TReturnCode::kSuccess;
   }

   /// This function can be used to remove all the listeners from the internal
   /// list. Should not be necessary under regular circumstances.
   ///
   void TEvent::clearListeners() {

      m_listeners.clear();
      return;
   }

   /// The names of containers can change during the lifetime of the experiment.
   /// One such change happened after the DC14 exercise, when many containers
   /// got a new name. (Like "ElectronCollection" became simply "Electrons".)
   ///
   /// This function allows us to create aliases with which certain containers
   /// should be accessible. So that the analyser would be able to access
   /// older files, while using the latest container name(s).
   ///
   /// @param onfile The name of the container as it was saved into the input
   ///               file
   /// @param newName The alias with which the object/container should be
   ///                accessible
   /// @returns The usual TReturnCode types
   ///
   TReturnCode TEvent::addNameRemap( const std::string& onfile,
                                     const std::string& newName ) {

      // Check if this name is known on the input or output already. As that's
      // not good.
      if( m_inputEventFormat.exists( newName ) ) {
         ::Error( "xAOD::TEvent::addNameRemap",
                  XAOD_MESSAGE( "Can't use \"%s\" as the target name in the"
                                "\"%s\" -> \"%s\" remapping" ),
                  newName.c_str(), onfile.c_str(), newName.c_str() );
         return TReturnCode::kFailure;
      }

      // Check if this name was remapped to something already:
      auto itr = m_nameRemapping.find( newName );
      if( itr != m_nameRemapping.end() ) {
         ::Warning( "xAOD::TEvent::addNameRemap",
                    "Overriding existing name remapping \"%s\" -> \"%s\"",
                    itr->second.c_str(), itr->first.c_str() );
         ::Warning( "xAOD::TEvent::addNameRemap", "  with: \"%s\" -> \"%s\"",
                    onfile.c_str(), newName.c_str() );
      }

      /// Save the new name association:
      m_nameRemapping[ newName ] = onfile;

      // Return gracefully:
      return TReturnCode::kSuccess;
   }

   /// This function simply clears out any existing name remapping declarations.
   /// In case the remapping rules need to be changed in the code in some
   /// complicated way.
   ///
   void TEvent::clearNameRemap() {

      m_nameRemapping.clear();
      return;
   }

   /// This function can be used for debugging, to check what container/object
   /// name remapping rules are in place for the current TEvent object.
   ///
   void TEvent::printNameRemap() const {

      // Print a header:
      ::Info( "xAOD::TEvent::printNameRemap", "Name remapping rules:" );

      // In case no remapping rules have been set:
      if( ! m_nameRemapping.size() ) {
         ::Info( "xAOD::TEvent::printNameRemap", "   NONE" );
         return;
      }

      // Otherwise:
      for( auto itr = m_nameRemapping.begin(); itr != m_nameRemapping.end();
           ++itr ) {
         ::Info( "xAOD::TEvent::printNameRemap", "   \"%s\" -> \"%s\"",
                 itr->second.c_str(), itr->first.c_str() );
      }

      // Return gracefully:
      return;
   }

   /// This function can be used to create/retrieve a ROOT-specific auxiliary
   /// object container that can be used to write information in the output
   /// file. Any auxiliary information written this way will however only be
   /// readable in ROOT, using the <code>kBranchAccess</code> option.
   ///
   /// @param key The name/prefix of the auxiliary store object/branches
   /// @param type The type of the auxiliary store (object/container)
   /// @param basketSize Size of the baskets created by the store object
   /// @param splitLevel The split level of the branches created by the store
   /// @returns An auxiliary store object that will write to the output
   ///
   SG::IAuxStore* TEvent::recordAux( const std::string& key,
                                     SG::IAuxStoreHolder::AuxStoreType type,
                                     ::Int_t basketSize, ::Int_t splitLevel ) {

      // A sanity check:
      if( ! m_outTree ) {
         ::Error( "xAOD::TEvent::recordAux",
                  XAOD_MESSAGE( "No output tree given to the object" ) );
         return 0;
      }

      // Check for an object with this name in the output list:
      Object_t::iterator itr = m_outputObjects.find( key );
      if( itr == m_outputObjects.end() ) {
         // Create one if if it doesn't exist yet...
         // Translate the store type:
         TAuxStore::EStructMode mode = TAuxStore::kUndefinedStore;
         switch( type ) {
         case SG::IAuxStoreHolder::AST_ObjectStore:
            mode = TAuxStore::kObjectStore;
            break;
         case SG::IAuxStoreHolder::AST_ContainerStore:
            mode = TAuxStore::kContainerStore;
            break;
         default:
            ::Error( "xAOD::TEvent::recordAux",
                     XAOD_MESSAGE( "Unknown store type (%i) requested" ),
                     static_cast< int >( type ) );
            return 0;
         }
         // Create and record the object:
         TAuxStore* store = new TAuxStore( key.c_str(), kTRUE, mode,
                                           basketSize, splitLevel );
         if( record( store, key, basketSize, splitLevel, kTRUE ).isFailure() ) {
            ::Error( "xAOD::TEvent::recordAux",
                     XAOD_MESSAGE( "Couldn't connect TAuxStore object to the "
                                   "output" ) );
            delete store;
            return 0;
         }
         // Update the iterator:
         itr = m_outputObjects.find( key );
      }

      // A security check:
      if( itr == m_outputObjects.end() ) {
         ::Error( "xAOD::TEvent::recordAux",
                  XAOD_MESSAGE( "Internal logic error detected" ) );
         return 0;
      }

      // Check that it is of the right type:
      TAuxManager* mgr = dynamic_cast< TAuxManager* >( itr->second );
      if( ! mgr ) {
         ::Error( "xAOD::TEvent::recordAux",
                  XAOD_MESSAGE( "Object of non-TAuxStore type registered "
                                "with key \"%s\"" ), key.c_str() );
         return 0;
      }

      // Extract the pointer out of it:
      TAuxStore* store = mgr->getStore();

      // Give it to the user:
      return store;
   }

   /// This function can be used to easily copy a given object/container to the
   /// output, without modifying the contents of it. It only needs to be called
   /// on the interface object/container, the copying of the auxiliary data is
   /// done automatically, and is steered by the SetAuxItemList function.
   ///
   /// @param key The key (branch name) of the object/container to copy
   /// @param basketSize Optional size for the basket associated with the
   ///                   output branch
   /// @param splitLevel Optional split level of the output branch
   ///
   TReturnCode TEvent::copy( const std::string& key,
                             ::Int_t basketSize, ::Int_t splitLevel ) {

      // Check if a name re-mapping should be applied or not:
      std::string keyToUse = key;
      std::map< std::string, std::string >::const_iterator remap_itr =
         m_nameRemapping.find( key );
      if( ( remap_itr != m_nameRemapping.end() ) &&
          ( ! m_inputEventFormat.exists( key ) ) &&
          m_inputEventFormat.exists( remap_itr->second ) ) {
         keyToUse = remap_itr->second;
      }

      // Make sure that the branch got connected to:
      RETURN_CHECK( "xAOD::TEvent::copy", connectBranch( keyToUse ) );

      // Make sure that the input object is properly updated:
      Object_t::const_iterator vobjMgr = m_inputObjects.find( keyToUse );
      if( vobjMgr == m_inputObjects.end() ) {
         ::Error( "xAOD::TEvent::copy",
                  XAOD_MESSAGE( "Internal logic error detected" ) );
         return TReturnCode::kFailure;
      }
      const TObjectManager* objMgr =
         dynamic_cast< const TObjectManager* >( vobjMgr->second );
      if( ! objMgr ) {
         ::Error( "xAOD::TEvent::copy",
                  XAOD_MESSAGE( "Internal logic error detected" ) );
         return TReturnCode::kFailure;
      }
      if( ! getInputObject( key,
                       *( objMgr->holder()->getClass()->GetTypeInfo() ) ) ) {
         ::Error( "xAOD::TEvent::copy",
                  XAOD_MESSAGE( "Internal logic error detected" ) );
         return TReturnCode::kFailure;
      }

      // Put the interface object into the output:
      RETURN_CHECK( "xAOD::TEvent::copy",
                    record( objMgr->object(),
                            objMgr->holder()->getClass()->GetName(),
                            key, basketSize, splitLevel, kTRUE ) );

      // Check if we have a filtering rule for the store:
      const std::set< std::string >* filter = 0;
      std::map< std::string, std::set< std::string > >::const_iterator fitr =
         m_auxItemList.find( key + "Aux." );
      if( fitr != m_auxItemList.end() ) {
         filter = &( fitr->second );
      }

      // Put the auxiliary store object into the output:
      Object_t::const_iterator vauxMgr = m_inputObjects.find( keyToUse +
                                                              "Aux." );
      if( vauxMgr == m_inputObjects.end() ) {
         // If there is no auxiliary store for this object/container, we're
         // done already:
         return TReturnCode::kSuccess;
      }
      // Check what type of auxiliary store this is:
      if( m_auxMode == kClassAccess || m_auxMode == kAthenaAccess ) {
         const TObjectManager* auxMgr =
            dynamic_cast< const TObjectManager* >( vauxMgr->second );
         if( ! auxMgr ) {
            ::Error( "xAOD::TEvent::copy",
                     XAOD_MESSAGE( "Internal logic error detected" ) );
            return TReturnCode::kFailure;
         }
         // Set up the filtering:
         if( filter ) {
            void* io1 = auxMgr->holder()->getAs( typeid( SG::IAuxStoreIO ) );
            SG::IAuxStoreIO* io2 = reinterpret_cast< SG::IAuxStoreIO* >( io1 );
            if( ! io2 ) {
               ::Error( "xAOD::TEvent::copy",
                        XAOD_MESSAGE( "Couldn't get SG::IAuxStoreIO pointer to "
                                      "object: %s" ),
                        ( keyToUse + "Aux." ).c_str() );
               return TReturnCode::kFailure;
            }
            io2->selectAux( *filter );
         }
         RETURN_CHECK( "xAOD::TEvent::copy",
                       record( auxMgr->object(),
                               auxMgr->holder()->getClass()->GetName(),
                               key + "Aux.", basketSize, splitLevel, kTRUE ) );
      } else if( m_auxMode == kBranchAccess ) {
         const TAuxManager* auxMgr =
            dynamic_cast< const TAuxManager* >( vauxMgr->second );
         if( ! auxMgr ) {
            ::Error( "xAOD::TEvent::copy",
                     XAOD_MESSAGE( "Internal logic error detected" ) );
            return TReturnCode::kFailure;
         }
         // Set up the filtering:
         if( filter ) {
            auxMgr->getStore()->selectAux( *filter );
         }
         RETURN_CHECK( "xAOD::TEvent::copy",
                       record( auxMgr->getStore(),
                               key + "Aux.", basketSize, splitLevel ) );
      } else {
         ::Fatal( "xAOD::TEvent::copy",
                  XAOD_MESSAGE( "Internal logic error detected" ) );
      }

      // Return gracefully:
      return TReturnCode::kSuccess;
   }

   /// This function is here to make it easy to write code that skims an input
   /// file into an output file without applying any modification to the events.
   ///
   /// @param basketSize Optional size for the basket associated with the
   ///                   output branch
   /// @param splitLevel Optional split level of the output branch
   /// @returns <code>xAOD::TReturnCode::kSuccess</code> if the copy was
   ///          successful, or <code>xAOD::TReturnCode::kFailure</code> if not
   ///
   TReturnCode TEvent::copy( ::Int_t basketSize, ::Int_t splitLevel ) {

      // Make sure that an input TTree is available:
      if( ! m_inTree ) {
         ::Error( "xAOD::TEvent::copy",
                  XAOD_MESSAGE( "No input TTree is open" ) );
         return TReturnCode::kFailure;
      }

      // Loop over the known input containers:
      EventFormat::const_iterator itr = m_inputEventFormat.begin();
      EventFormat::const_iterator end = m_inputEventFormat.end();
      for( ; itr != end; ++itr ) {

         // Convenience reference:
         const EventFormatElement& efe = itr->second;

         // Ignore objects that don't exist on the input:
         if( ! m_inTree->GetBranch( efe.branchName().c_str() ) ) {
            continue;
         }
         // Skip all branches ending in "Aux.":
         if( efe.branchName().find( "Aux." ) ==
             ( efe.branchName().size() - 4 ) ) {
            continue;
         }
         // Also skip dynamic branches:
         if( efe.parentName() != "" ) {
            continue;
         }
         // Check if the class in question is known:
         ::TClass* cl = ::TClass::GetClass( efe.className().c_str() );
         if( ( ! cl ) || ( ! cl->IsLoaded() ) ) {
            continue;
         }

         // Make the copy then...
         RETURN_CHECK( "xAOD::TEvent::copy",
                       this->copy( efe.branchName(), basketSize, splitLevel ) );
      }

      // Return gracefully:
      return TReturnCode::kSuccess;
   }

   /// @returns The number of events in the input file(s)
   ///
   ::Long64_t TEvent::getEntries() const {

      if( m_inChain ) {
         return m_inChain->GetEntries();
      } else if( m_inTree ) {
         return m_inTree->GetEntries();
      } else if( m_inTreeMissing ) {
         // The input file is empty:
         return 0;
      } else {
         ::Error( "xAOD::TEvent::getEntries",
                  XAOD_MESSAGE( "Function called on an uninitialised "
                                "object" ) );
         return 0;
      }
   }

   /// This function is used to move to looking at a new entry from the input
   /// tree. It doesn't do any I/O operation at this point, it just remembers
   /// which entry the objects should be loaded from later on.
   ///
   /// Unless one calls the function with <code>getall==1</code>. In that case
   /// all input is force-read. This is necessary when writing out an event
   /// that was processed in a load-on-request manner.
   ///
   /// @param entry The entry from the input tree to load
   /// @param getall Parameter deciding if partial reading should be used or
   ///               not.
   /// @returns The number of bytes read, or a negative number in case of an
   ///          error
   ///
   ::Int_t TEvent::getEntry( ::Long64_t entry, ::Int_t getall ) {

      // A little sanity check:
      if( ( ! m_inTree ) && ( ! m_inChain ) ) {
         ::Error( "xAOD::TEvent::getEntry",
                  XAOD_MESSAGE( "Function called on an uninitialised "
                                "object" ) );
         return -1;
      }

      // If we have a chain as input:
      if( m_inChain ) {
         // Make sure that the correct tree is loaded:
         m_entry = m_inChain->LoadTree( entry );
         if( m_entry < 0 ) {
            ::Error( "xAOD::TEvent::getEntry",
                     XAOD_MESSAGE( "Failure in loading entry %i from the "
                                   "input chain" ),
                     static_cast< int >( entry ) );
            return -1;
         }
         // Check if a new file was loaded:
         if( ( m_inTreeNumber != m_inChain->GetTreeNumber() ) ||
             m_inChainTracker->internalStateChanged() ) {
            // Reset the tracker:
            m_inChainTracker->reset();
            // Connect to this new file:
            m_inTreeNumber = m_inChain->GetTreeNumber();
            ::TFile* file = m_inChain->GetFile();
            // The useTreeCache parameter is set to false, since the cache
            // is anyway set up through the TChain. It shouldn't be modified
            // on the file level.
            if( ! readFrom( file, kFALSE, m_inChain->GetName() ) ) {
               ::Error( "xAOD::TEvent::getEntry",
                        XAOD_MESSAGE( "Couldn't connect to input file #%i "
                                      "of the input chain" ), m_inTreeNumber );
               return -1;
            }
         }
      }
      // If we have a regular file/tree as input:
      else {
         m_entry = entry;
      }

      // In order to make the tree cache work:
      if( m_inTree && m_inTree->LoadTree( m_entry ) < 0 ) {
         ::Error( "xAOD::TEvent::getEntry",
                  XAOD_MESSAGE( "Failure in loading entry %i from the input "
                                "file" ), static_cast< int >( m_entry ) );
         return -1;
      }

      // Stats counter needs to know it's the next event:
      IOStats::instance().stats().nextEvent();

      // If force-reading is not requested, we're done:
      if( ! getall ) {

         // Notify the listeners that a new event was loaded:
         const TIncident incident( IncidentType::BeginEvent );
         Listener_t::iterator l_itr = m_listeners.begin();
         Listener_t::iterator l_end = m_listeners.end();
         for( ; l_itr != l_end; ++l_itr ) {
            ( *l_itr )->handle( incident );
         }

         // No real reading was done:
         return 0;
      }

      // Loop over all input object managers, and force them to load their
      // content:
      ::Int_t result = 0;
      if( m_auxMode == kAthenaAccess ) {
         // In kAthenaAccess mode we need to use getInputObject(...) to load
         // all the input objects correctly.
         for( auto& inObj : m_inputObjects ) {
            static const std::string dynStorePostfix = "Aux.Dynamic";
            if( inObj.first.find( dynStorePostfix ) ==
                ( inObj.first.size() - dynStorePostfix.size() ) ) {
               // Ignore the dynamic store objects. They get loaded through
               // their parents.
            } else {
               // Load the objects and their auxiliary stores through the
               // getInputObject(...) function, which takes care of correctly
               // setting them up. The type is irrelevant here. We don't
               // really care about the exact type of the objects.
               getInputObject( inObj.first, typeid( int ), kTRUE, kFALSE );
            }
         }
      } else {
         // In a "reasonable" access mode, we do something very simple:
         for( auto& inObj : m_inputObjects ) {
            result += inObj.second->getEntry( m_entry, getall );
         }
      }

      // Notify the listeners that a new event was loaded:
      const TIncident incident( IncidentType::BeginEvent );
      Listener_t::iterator l_itr = m_listeners.begin();
      Listener_t::iterator l_end = m_listeners.end();
      for( ; l_itr != l_end; ++l_itr ) {
         ( *l_itr )->handle( incident );
      }

      // Return the number of bytes read:
      return result;
   }

   /// This function can be used to get the number of input files that the
   /// object currently knows about. This is meant to be used to collect the
   /// metadata from each of the input files, or to access the metadata using
   /// a transient tree. (xAOD::TMetaTree)
   ///
   /// @returns The number of input files when reading from a TChain, 1 when
   ///          reading from a TFile, and 0 if the object is not connected to
   ///          any input
   ///
   ::Long64_t TEvent::getFiles() const {

      if( m_inChain ) {
         return m_inChain->GetListOfFiles()->GetEntries();
      } else if( m_inTree || m_inTreeMissing ) {
         return 1;
      } else {
         return 0;
      }
   }

   /// This function can be used to iterate over the input files specified for
   /// the object. It should be most useful for collecting metadata about a set
   /// of input files, if there's no regular event loop done to process them.
   ///
   /// @param file   The file number to load
   /// @param getall If set to 1, all connected branches are loaded
   /// @returns The number of read bytes on success, or a negative number on
   ///          failure
   ///
   ::Int_t TEvent::getFile( ::Long64_t file, ::Int_t getall ) {

      // Check if the file number is valid:
      if( ( file < 0 ) || ( file >= getFiles() ) ) {
         ::Error( "xAOD::TEvent::getFile",
                  XAOD_MESSAGE( "Function called with invalid file number "
                                "(%i)" ), static_cast< int >( file ) );
         return -1;
      }

      // If we are not reading a TChain, return at this point. As the one and
      // only file is open already...
      if( ! m_inChain ) {
         return 0;
      }

      // Trigger the "scanning" of the input files, so the TChain would know
      // how many entries are in the various files.
      getEntries();

      // Calculate which entry/event we need to load:
      ::Long64_t entry = 0;
      for( ::Long64_t i = 0; i < file; ++i ) {
         entry += m_inChain->GetTreeOffset()[ i ];
      }

      // Load this entry using the regular event opening function:
      return getEntry( entry, getall );
   }

   /// This function needs to be called by the user at the end of processing
   /// each event that is meant to be written out.
   ///
   /// @returns The number of bytes written if successful, a negative number
   ///          if not
   ///
   ::Int_t TEvent::fill() {

      // A little sanity check:
      if( ! m_outTree ) {
         ::Error( "xAOD::TEvent::fill",
                  XAOD_MESSAGE( "Object not connected to an output file!" ) );
         return 0;
      }

      // Make sure that all objects have been read in. The 99 as the value
      // has a special meaning for TAuxStore. With this value it doesn't
      // delete its transient (decoration) variables. Otherwise it does.
      // (As it's supposed to, when moving to a new event.)
      if( m_inChain ) {
         if (getEntry( m_inChain->GetReadEntry(), 99 ) < 0) {
           ::Error( "xAOD::TEvent::fill",
                    XAOD_MESSAGE( "getEntry failed!" ) );
           return 0;
         }
      } else if( m_inTree ) {
         if (getEntry( m_entry, 99 ) < 0) {
           ::Error( "xAOD::TEvent::fill",
                    XAOD_MESSAGE( "getEntry failed!" ) );
           return 0;
         }
      }

      // Prepare the objects for writing:
      std::string unsetObjects;
      Object_t::const_iterator itr = m_outputObjects.begin();
      Object_t::const_iterator end = m_outputObjects.end();
      for( ; itr != end; ++itr ) {
         // Check that a new object was provided in the event:
         if( ! itr->second->isSet() ) {
            // We are now going to fail. But let's collect the names of
            // all the unset objects:
            if( unsetObjects.size() ) {
               unsetObjects.append( ", \"" + itr->first + "\"" );
            } else {
               unsetObjects.append( "\"" + itr->first + "\"" );
            }
            continue;
         }
         // Make sure that any dynamic auxiliary variables that
         // were added to the object after it was put into the event,
         // get added to the output:
         if( ! putAux( *m_outTree, *( itr->second ) ) ) {
            ::Error( "xAOD::TEvent::fill",
                     XAOD_MESSAGE( "Failed to put dynamic auxiliary variables "
                                   "in the output for object \"%s\"" ),
                     itr->first.c_str() );
            return 0;
         }
      }

      // Check if there were any unset objects:
      if( unsetObjects.size() ) {
         ::Error( "xAOD::TEvent::fill",
                  XAOD_MESSAGE( "The following objects were not set in the "
                                "current event: %s" ), unsetObjects.c_str() );
         return 0;
      }

      // Write the entry, and check the return value:
      const ::Int_t ret = m_outTree->Fill();
      if( ret <= 0 ) {
         ::Error( "xAOD::TEvent::fill",
                  XAOD_MESSAGE( "Output tree filling failed with return "
                                "value: %i" ), ret );
      }

      // Reset the object managers:
      Object_t::iterator ncitr = m_outputObjects.begin();
      Object_t::iterator ncend = m_outputObjects.end();
      for( ; ncitr != ncend; ++ncitr ) {
         ncitr->second->reset();
      }

      // Return the value:
      return ret;
   }

   /// This function makes it possible for the user to investigate the
   /// contents of the input file that the object is currently connected to.
   ///
   ///
   /// Notice that this doesn't necessarily mean that all the objects for
   /// which metadata is stored, would necessarily be part of the input
   /// TTree that this object is connected to.
   ///
   /// @returns The event format object if a file is being read, or a null
   ///          pointer if no input file is connected at the moment.
   ///
   const EventFormat* TEvent::inputEventFormat() const {

      // If we *are* reading an input file:
      if( m_inTree || m_inTreeMissing ) {
         return &m_inputEventFormat;
      }

      // If not, let's complain:
      ::Warning( "xAOD::TEvent::inputEventFormat",
                 "No input file is connected at the moment" );
      return 0;
   }

   /// This function makes it possible for the user to investigate the
   /// content that is being written to the output file.
   ///
   /// Notice that this doesn't necessarily mean that all the objects for
   /// which metadata is stored, would necessarily be part of the output
   /// TTree that this object is connected to.
   ///
   /// @returns The event format object if a file is being written, or a null
   ///          pointer if not.
   ///
   const EventFormat* TEvent::outputEventFormat() const {

      // If we *are* reading an input file:
      if( m_outTree ) {
         return m_outputEventFormat;
      }

      // If not, let's complain:
      ::Warning( "xAOD::TEvent::outputEventFormat",
                 "No output file is connected at the moment" );
      return 0;
   }

   /// This helper function is mostly needed by the smart pointers of the
   /// xAOD EDM. Right now it very simply just calculates the hash just
   /// based on the key given to the function. But later on it might be
   /// good to do some tests here, checking if the event format knows about
   /// the specified key or not. This is why it's not made to be a static
   /// function
   ///
   /// @param key String key to turn into a hash
   /// @returns A hash corresponding to the specified string key
   ///
   uint32_t TEvent::getHash( const std::string& key ) const {

      // For empty keys let's finish quickly:
      if( key == "" ) return 0;

      // If the key is used in the input file, let's use the same hash for
      // the output file as well:
      if( m_inputEventFormat.exists( key ) ) {
         return m_inputEventFormat.get( key )->hash();
      }

      // If it's a new key, make a new hash for it from scratch:
      return Utils::hash( key );
   }

   /// This function is used by the smart pointer code to find the identifier
   /// of an object that's already in the event in some way.
   ///
   /// @param obj Pointer to the object that we want to look up
   /// @returns The hashed identifier of the object, or 0 if the object was
   ///          not found in the event
   ///
   uint32_t TEvent::getKey( const void* obj ) const {

      // Make use of the getName function:
      return getHash( getName( obj ) );
   }

   /// This function is used by the smart pointer code to find the identifier
   /// of an object that's already in the event in some way.
   ///
   /// @param obj Pointer to the object that we want to look up
   /// @returns The name of the object, or an empty string if the object was
   ///          not found in the event
   ///
   const std::string& TEvent::getName( const void* obj ) const {

      // First look among the output objects:
      Object_t::const_iterator obj_itr = m_outputObjects.begin();
      Object_t::const_iterator obj_end = m_outputObjects.end();
      for( ; obj_itr != obj_end; ++obj_itr ) {

         // Check if this is our object:
         if( obj_itr->second->object() != obj ) continue;

         // If it is, let's return right away:
         return obj_itr->first;
      }

      // Now look among the input objects:
      obj_itr = m_inputObjects.begin();
      obj_end = m_inputObjects.end();
      for( ; obj_itr != obj_end; ++obj_itr ) {

         // Check if this is our object:
         if( obj_itr->second->object() != obj ) continue;

         // If it is, let's return:
         return obj_itr->first;
      }

      // If it's not there either, check if it's in an active TStore object:
      const TStore* store = TActiveStore::store();
      if( store && store->contains( obj ) ) {
         // Get the name from the store then:
         return store->getName( obj );
      }

      // We didn't find the object in the event...
      ::Warning( "xAOD::TEvent::getName",
                 "Didn't find object with pointer %p in the event",
                 obj );
      static const std::string dummy;
      return dummy;
   }

   /// This function is used primarily when getting the string key of
   /// a smart pointer that we read in from a file, or access it in memory.
   ///
   /// @param hash The hashed key for the container/object
   /// @returns The name of the object, or an empty string if the object was
   ///          not found in the event
   ///
   const std::string& TEvent::getName( uint32_t hash ) const {

      // If the branch is known from the input:
      if( m_inputEventFormat.exists( hash ) ) {
         return m_inputEventFormat.get( hash )->branchName();
      }

      // If the branch is known on the output:
      if( m_outputEventFormat &&
          m_outputEventFormat->exists( hash ) ) {
         return m_outputEventFormat->get( hash )->branchName();
      }

      // If this is an object in the active store:
      const TStore* store = TActiveStore::store();
      if( store && store->contains( hash ) ) {
         return store->getName( hash );
      }

      // If it is unknown:
      static const std::string dummy;
      return dummy;
   }

   /// This function is used by the TVirtualEvent interface to access an
   /// output object with a given hashed key. The function looks up the string
   /// key belonging to the hash, and then calls the other GetOutputObject(...)
   /// function in the class with that parameter.
   ///
   /// @param key The hashed key of the output object
   /// @param ti  The type description of the object requested
   /// @returns A pointer to the requested object, or a null pointer in case
   ///          of failure
   ///
   void* TEvent::getOutputObject( uint32_t key,
                                  const std::type_info& ti ) {

      // Get a string name for this key:
      const std::string& name = getName( key );
      if( ! name.length() ) {
         return 0;
      }

      // Forward the call to the function using an std::string key:
      return getOutputObject( name, ti );
   }

   /// This function is used by the TVirtualEvent interface to access an
   /// input object with a given hashed key. The function looks up the string
   /// key belonging to the hash, and then calls the other GetInputObject(...)
   /// function in the class with that parameter.
   ///
   /// @param key    The hashed key of the input object
   /// @param ti     The type description of the object requested
   /// @param silent Switch for being silent about failures or not
   /// @returns A pointer to the requested object, or a null pointer in case
   ///          of failure
   ///
   const void* TEvent::getInputObject( uint32_t key,
                                       const std::type_info& ti,
                                       bool silent ) {

      // Get a string name for this key:
      const std::string& name = getName( key );
      if( ( ! name.length() ) && ( ! silent ) ) {
         Warning( "xAOD::TEvent::getInputObject",
                  "Key 0x%08x unknown", key );
         return 0;
      }

      // Forward the call to the function using an std::string key:
      return getInputObject( name, ti, silent );
   }

   /// This function is used internally to initialise the reading of an input
   /// file. It prepares the "monitoring information" in memory that gets filled
   /// while the code is running, with information about xAOD I/O.
   ///
   /// @returns <code>TReturnCode::kSuccess</code> if the function is
   ///          successful, or <code>TReturnCode::kFaulure</code> if not
   ///
   TReturnCode TEvent::initStats() {

      // If we're dealing with an empty input file, stop here:
      if( m_inTreeMissing ) {
         return TReturnCode::kSuccess;
      }

      // A little sanity check:
      if( ! m_inTree ) {
         ::Error( "xAOD::TEvent::initStats",
                  XAOD_MESSAGE( "Function called on an uninitialised "
                                "object" ) );
         return TReturnCode::kFailure;
      }

      // Reset the number of input branches information:
      IOStats::instance().stats().setBranchNum( 0 );

      // Loop over the EventFormat information
      EventFormat::const_iterator itr = m_inputEventFormat.begin();
      EventFormat::const_iterator end = m_inputEventFormat.end();
      for( ; itr != end; ++itr ) {

         // Get the name of the branch in question:
         const std::string& branchName = itr->second.branchName();

         // If it's an auxiliary container, scan it using TAuxStore:
         if( branchName.find( "Aux." ) != std::string::npos ) {

            // But first decide whether it describes a container, or just
            // a single object. Since the file may have been written in
            // kBranchAccess mode, it's not necessarily a good idea to check
            // the type of the auxiliary class. So let's check the interface
            // class instead.
            //
            // Get the name of the interface object/container:
            const std::string intName =
               branchName.substr( 0, branchName.size() - 4 );
            if( ! m_inputEventFormat.exists( intName ) ) {
               // When this happens, it may still be that both the interface and
               // the auxiliary container is missing from the file. As we didn't
               // check yet whether the auxiliary container is in place or not.
               // So, before printing a warning, let's check for this.
               // Unfortunately the check is pretty expensive, but this should
               // not be performance critical code after all...
               ::Bool_t auxFound = kFALSE;
               const std::string dynName = Utils::dynBranchPrefix( branchName );
               TObjArray* branches = m_inTree->GetListOfBranches();
               for( Int_t i = 0; i < branches->GetEntries(); ++i ) {
                  const TString name( branches->At( i )->GetName() );
                  if( name.BeginsWith( branchName ) ||
                      name.BeginsWith( dynName ) ) {
                     auxFound = kTRUE;
                     break;
                  }
               }
               if( auxFound ) {
                  ::Warning( "xAOD::TEvent::initStats",
                             "Couldn't find interface object/container "
                             "\"%s\" belonging to branch \"%s\"",
                             intName.c_str(), branchName.c_str() );
               }
               continue;
            }

            // Get the type of the interface:
            const EventFormatElement* el = m_inputEventFormat.get( intName );
            ::TClass* cl = ::TClass::GetClass( el->className().c_str() );
            if( ( ! cl ) || ( ! cl->IsLoaded() ) ) {
               ::Warning( "xAOD::TEvent::initStats",
                          "Couldn't find dictionary for type \"%s\"",
                          el->className().c_str() );
               continue;
            }

            // Get the dictionary for the DataVector base class:
            static const std::type_info& baseTi = typeid( SG::AuxVectorBase );
            static const std::string baseName =
               SG::normalizedTypeinfoName( baseTi );
            static ::TClass* baseCl = ::TClass::GetClass( baseName.c_str() );
            if( ! baseCl ) {
               ::Error( "xAOD::TEvent::initStats",
                        XAOD_MESSAGE( "Couldn't get dictionary for type "
                                      "\"%s\"" ), baseName.c_str() );
               return TReturnCode::kFailure;
            }

            // The type of the auxiliary store is finally deduced from the
            // inheritance of the interface container.
            const TAuxStore::EStructMode mode =
               ( cl->InheritsFrom( baseCl ) ? TAuxStore::kContainerStore :
                 TAuxStore::kObjectStore );

            // Scan the branches using a temporary TAuxStore instance:
            TAuxStore temp( branchName.c_str(), kTRUE, mode );
            RETURN_CHECK( "xAOD::TEvent::initStats",
                          temp.initStats( m_inTree ) );
         }
         // If it's an interface container:
         else {
            // Try to access the branch:
            const ::TBranch* container =
               m_inTree->GetBranch( branchName.c_str() );
            // If it exists, let's remember it:
            if( container ) {
               IOStats::instance().stats().container( branchName );
            }
         }
      }

      // Return gracefully:
      return TReturnCode::kSuccess;
   }

   /// This function does the heavy lifting of retrieving object from the list
   /// of output objects. While it returns a typeless pointer, that pointer can
   /// be cast directly to the type described by the second parameter in the
   /// caller code.
   ///
   /// @param key The key (branch name) of the object to retrieve
   /// @param ti  The type as which the object is to be retrieved
   /// @param metadata Flag deciding whether we're looking for a metadata or
   ///                 event data object
   /// @returns A pointer to the output object if successful, or a null pointer
   ///          if not
   ///
   void* TEvent::getOutputObject( const std::string& key,
                                  const std::type_info& ti,
                                  ::Bool_t metadata ) const {

      // Select which object container to use:
      const Object_t& objects = ( metadata ?
                                  m_outputMetaObjects : m_outputObjects );

      // Check if the object can be found:
      Object_t::const_iterator itr = objects.find( key );
      if( itr == objects.end() ) {
         // Do the following only for event data:
         if( ! metadata ) {
            // It's not in the event. Let's check if we find it in an active
            // TStore object...
            TStore* store = TActiveStore::store();
            if( ( ! store ) || ( ! store->contains( key, ti ) ) ||
                store->isConst( key, ti ) ) {
               // Nope, not there either...
               return 0;
            }
            // Let's return the object from the TStore:
            void* result = store->getObject( key, ti );
            return result;
         } else {
            // For metadata we don't use external resources.
            return 0;
         }
      }

      // If the object is not set in this event yet, we can't continue:
      if( ! itr->second->isSet( false ) ) {
         return 0;
      }

      // If it does exist, check if it's the right kind of object:
      TObjectManager* mgr =
         dynamic_cast< TObjectManager* >( itr->second );
      if( ! mgr ) {
         ::Error( "xAOD::TEvent::getOutputObject",
                  XAOD_MESSAGE( "Object of wrong type found for key \"%s\"" ),
                  key.c_str() );
         return 0;
      }

      // Ask the holder object for the object of this type:
      void* result = mgr->holder()->getAs( ti );
      if( ! result ) {
         ::Warning( "xAOD::TEvent::getOutputObject",
                    "Couldn't retrieve object as \"%s\"",
                    Utils::getTypeName( ti ).c_str() );
         return 0;
      }

      // Return the object:
      return result;
   }

   /// This is the function doing the heavy lifting to retrieve objects
   /// from the input file.
   ///
   /// @param key The key (branch name) of the object to retrieve
   /// @param ti  The type as which the object is to be retrieved
   /// @param silent Set to <code>kTRUE</code> to make the code fail silently
   ///               in case the object can't be retrieved
   /// @param metadata Flag deciding whether we're looking for a metadata or
   ///                 event data object
   /// @returns A pointer to the input object if successful, or a null pointer
   ///          if not
   ///
   const void* TEvent::getInputObject( const std::string& key,
                                       const std::type_info& ti,
                                       ::Bool_t silent,
                                       ::Bool_t metadata ) {

      // Check if a name remapping should be applied or not:
      std::string keyToUse = key;
      std::map< std::string, std::string >::const_iterator remap_itr =
         m_nameRemapping.find( key );
      if( ( remap_itr != m_nameRemapping.end() ) &&
          ( ! m_inputEventFormat.exists( key ) ) &&
          m_inputEventFormat.exists( remap_itr->second ) ) {
         keyToUse = remap_itr->second;
      }

      // The following catches the cases when we ask for a transient
      // ConstDataVector object to be returned as "const DataVector".
      TStore* store = TActiveStore::store();
      if( store && store->contains( keyToUse, ti ) &&
          store->isConst( keyToUse, ti ) ) {
         const void* result = store->getConstObject( keyToUse, ti );
         return result;
      }

      // A sanity check before checking for an object from the input file:
      if( ( ! m_inTree ) && ( ! metadata ) ) {
         return 0;
      }
      if( ( ! m_inMetaTree ) && metadata ) {
         return 0;
      }

      // Make sure that the requested branch is connected to:
      if( metadata ) {
         if( ! connectMetaBranch( keyToUse, silent ).isSuccess() ) {
            return 0;
         }
      } else {
         if( ! connectBranch( keyToUse, silent ).isSuccess() ) {
            return 0;
         }
      }

      // Select which object container to use:
      Object_t& objects = ( metadata ?
                            m_inputMetaObjects : m_inputObjects );

      // Access the object's manager:
      Object_t::iterator itr = objects.find( keyToUse );
      if( itr == objects.end() ) {
         ::Fatal( "xAOD::TEvent::getInputObject",
                  XAOD_MESSAGE( "There is an internal logic error in the "
                                "code..." ) );
         return 0;
      }

      // This has to be an ObjectManager object:
      TObjectManager* mgr =
         dynamic_cast< TObjectManager* >( itr->second );
      if( ! mgr ) {
         if( key == keyToUse ) {
            ::Error( "xAOD::TEvent::getInputObject",
                     XAOD_MESSAGE( "Object of wrong type found for key "
                                   "\"%s\"" ),
                     key.c_str() );
         } else {
            ::Error( "xAOD::TEvent::getInputObject",
                     XAOD_MESSAGE( "Object of wrong type found for key "
                                   "\"%s\"/\"%s\"" ), key.c_str(),
                     keyToUse.c_str() );
         }
         return 0;
      }

      // Make sure that the current entry is loaded for event data objects:
      if( ! metadata ) {
         if( mgr->getEntry( m_entry ) ) {
            // Connect the auxiliary store to objects needing it. This call also
            // takes care of updating the dynamic store of auxiliary containers,
            // when they are getting accessed directly.
            if( ! setAuxStore( *mgr ).isSuccess() ) {
               ::Error( "xAOD::TEvent::getInputObject",
                        XAOD_MESSAGE( "Failed to set the auxiliary store for "
                                      "%s/%s" ),
                        mgr->holder()->getClass()->GetName(),
                        keyToUse.c_str() );
               return 0;
            }
         }
      }

      // Ask the holder object for the object of this type:
      const void* result = mgr->holder()->getAsConst( ti, silent );
      if( ! result ) {
         if( ! silent ) {
            ::Warning( "xAOD::TEvent::getInputObject",
                       "Could not retrieve object with key \"%s\" "
                       "as \"%s\"", keyToUse.c_str(),
                       Utils::getTypeName( ti ).c_str() );
         }
         return 0;
      }

      // We succeeded:
      return result;
   }

   /// This is the function doing the heavy lifting when recording a new
   /// object into the output tree/file. It makes sure that the object is
   /// saved together with all of its dynamic auxiliary data if it has any.
   ///
   /// @param obj A typeless pointer to the object that we want to record
   /// @param typeName The type name of the output object
   /// @param key The key (branch name) of the object to record
   /// @param basketSize The basket size of the output branch
   /// @param splitLevel The split level of the output branch
   /// @param overwrite Flag selecting if it is allowed to overwrite an
   ///                  already existing object (used internally)
   /// @param metadata Flag selecting if we are writing an event or a
   ///                 metadata object
   /// @param isOwner Flag selecting if we should take ownership of the object
   ///                or not
   /// @returns <code>kTRUE</code> if the operation was successful, or
   ///          <code>kFALSE</code> if it was not
   ///
   TReturnCode TEvent::record( void* obj, const std::string& typeName,
                               const std::string& key,
                               ::Int_t basketSize, ::Int_t splitLevel,
                               ::Bool_t overwrite, ::Bool_t metadata,
                               ::Bool_t isOwner ) {

      // Check if we have an output tree:
      if( ! m_outTree ) {
         ::Error( "xAOD::TEvent::record",
                  XAOD_MESSAGE( "No output tree defined. Did you forget to "
                                "call writeTo(...)?" ) );
         return TReturnCode::kFailure;
      }
      assert( m_outputEventFormat != 0 );

      // If this is metadata, just take ownership of it. The object will only
      // be recorded into the output file when calling finishWritingTo(...).
      if( metadata ) {
         // Check whether we already have such an object:
         if( ( ! overwrite ) &&
             ( m_outputMetaObjects.find( key ) !=
               m_outputMetaObjects.end() ) ) {
            ::Error( "xAOD::TEvent::record",
                     XAOD_MESSAGE( "Meta-object %s/%s already recorded" ),
                     typeName.c_str(), key.c_str() );
            return TReturnCode::kFailure;
         }
         // Check if we have a dictionary for this object:
         TClass* cl = TClass::GetClass( typeName.c_str() );
         if( ! cl ) {
            ::Error( "xAOD::TEvent::record",
                     XAOD_MESSAGE( "Didn't find dictionary for type: %s" ),
                     typeName.c_str() );
            return TReturnCode::kFailure;
         }
         // Let's create a holder for the object:
         THolder* hldr = new THolder( obj, cl, isOwner );
         TObjectManager* mgr =
            new TObjectManager( 0, hldr, m_auxMode == kAthenaAccess );
         m_outputMetaObjects[ key ] = mgr;
         // We're done. The rest will be done later on.
         return TReturnCode::kSuccess;
      }

      // Check if we accessed this object on the input. If yes, then this
      // key may not be used for recording.
      if( ( ! overwrite ) &&
          ( m_inputObjects.find( key ) != m_inputObjects.end() ) ) {
         ::Error( "xAOD::TEvent::record",
                  XAOD_MESSAGE( "Object %s/%s already accessed from the input, "
                                "can't be overwritten in memory" ),
                  typeName.c_str(), key.c_str() );
         return TReturnCode::kFailure;
      }

      // Override the default 0 split level with a split level of 1 for
      // auxiliary container objects.
      if( ( splitLevel == 0 ) &&
          ( key.find( "Aux." ) == ( key.size() - 4 ) ) ) {
         splitLevel = 1;
      }

      // Check if we need to add it to the event record:
      Object_t::iterator vitr = m_outputObjects.find( key );
      if( vitr == m_outputObjects.end() ) {

         // Check if we have a dictionary for this object:
         TClass* cl = TClass::GetClass( typeName.c_str() );
         if( ! cl ) {
            ::Error( "xAOD::TEvent::record",
                     XAOD_MESSAGE( "Didn't find dictionary for type: %s" ),
                     typeName.c_str() );
            return TReturnCode::kFailure;
         }

         // Check if this is a new object "type" or not:
         if( ! m_outputEventFormat->exists( key ) ) {
            m_outputEventFormat->add( EventFormatElement( key, cl->GetName(),
                                                          "", getHash( key ) ) );
         }

         // Let's create a holder for the object:
         THolder* hldr = new THolder( obj, cl, isOwner );
         TObjectManager* mgr =
            new TObjectManager( 0, hldr, m_auxMode == kAthenaAccess );
         m_outputObjects[ key ] = mgr;

         // ... and let's add it to the output TTree:
         *( mgr->branchPtr() ) =
            m_outTree->Branch( key.c_str(), cl->GetName(),
                               hldr->getPtr(), basketSize, splitLevel );
         if( ! mgr->branch() ) {
            ::Error( "xAOD::TEvent::record",
                     XAOD_MESSAGE( "Failed to create branch \"%s\" out of "
                                   "type \"%s\"" ),
                     key.c_str(), cl->GetName() );
            // Clean up:
            hldr->setOwner( kFALSE );
            delete mgr;
            return TReturnCode::kFailure;
         }

         // Set up the saving of all the dynamic auxiliary properties
         // of the object if it has any:
         RETURN_CHECK( "xAOD::TEvent::record",
                       putAux( *m_outTree, *mgr, basketSize, splitLevel,
                               kFALSE ) );

         // Return at this point, as we don't want to run the rest of
         // the function's code:
         return TReturnCode::kSuccess;
      }

      // Access the object manager:
      TObjectManager* omgr = dynamic_cast< TObjectManager* >( vitr->second );
      if( ! omgr ) {
         ::Error( "xAOD::TEvent::record",
                  XAOD_MESSAGE( "Manager object of the wrong type "
                                "encountered" ) );
         return TReturnCode::kFailure;
      }

      // Check that the type of the object matches that of the previous
      // object:
      if( typeName != omgr->holder()->getClass()->GetName() ) {
         // This may still be, when the ROOT dictionary name differs from the
         // "simple type name" known to C++. So let's get the ROOT name of the
         // new type:
         TClass* cl = TClass::GetClass( typeName.c_str() );
         if( ( ! cl ) || ::strcmp( cl->GetName(),
                                   omgr->holder()->getClass()->GetName() ) ) {
            ::Error( "xAOD::TEvent::record",
                     XAOD_MESSAGE( "For output key \"%s\" the previous type "
                                   "was \"%s\", the newly requested type is "
                                   "\"%s\"" ),
                     key.c_str(), omgr->holder()->getClass()->GetName(),
                     typeName.c_str() );
            return TReturnCode::kFailure;
         }
      }

      // Replace the managed object:
      omgr->setObject( obj );

      // Replace the auxiliary objects:
      return putAux( *m_outTree, *omgr, basketSize, splitLevel, kFALSE );
   }

   /// This function is used internally when copying an object with its
   /// auxiliary store from the input file, and branch access mode is
   /// activated for the event object.
   ///
   /// The assumption is that the store object already knows what prefix
   /// it should be using. The key parameter only specifies under what
   /// ID the object should be handled in the output object list.
   ///
   /// @param store The store object to connect to the output
   /// @param key The "key" with which to record the object
   /// @param basketSize The basket size of the output branches
   /// @param splitLevel The split level of the output branches
   /// @returns <code>kTRUE</code> if the operation was successful, or
   ///          <code>kFALSE</code> if it was not
   ///
   TReturnCode TEvent::record( TAuxStore* store, const std::string& key,
                               ::Int_t /*basketSize*/, ::Int_t /*splitLevel*/,
                               ::Bool_t ownsStore ) {

      // Check if we have an output tree:
      if( ! m_outTree ) {
         ::Error( "xAOD::TEvent::record",
                  XAOD_MESSAGE( "No output tree defined. Did you forget to "
                                "call writeTo(...)?" ) );
         return TReturnCode::kFailure;
      }

      // Check if we have a filtering rule for this key:
      const std::set< std::string >* filter = 0;
      auto filter_itr = m_auxItemList.find( key );
      if( filter_itr != m_auxItemList.end() ) {
         filter = &( filter_itr->second );
      }

      // Check if we need to add it to the event record:
      Object_t::iterator vitr = m_outputObjects.find( key );
      if( vitr == m_outputObjects.end() ) {

         // Configure the object for variable filtering:
         if( filter ) {
            store->selectAux( *filter );
         }
         // Tell the object where to write its contents:
         RETURN_CHECK( "xAOD::TEvent::record", store->writeTo( m_outTree ) );
         // Record it to the output list:
         TAuxManager* mgr = new TAuxManager( store, ownsStore );
         m_outputObjects[ key ] = mgr;

         // We're done:
         return TReturnCode::kSuccess;
      }

      // Check if the output has the right store:
      if( vitr->second->object() == store ) {
         // We're done already:
         return TReturnCode::kSuccess;
      }

      // If not, update the output manager. This can happen when we copy
      // objects from the input to the output files, and we process
      // multiple input files.
         
      // Check if the output manager is of the right type:
      TAuxManager* mgr = dynamic_cast< TAuxManager* >( vitr->second );
      if( ! mgr ) {
         ::Error( "xAOD::TEvent::record",
                  XAOD_MESSAGE( "Output object with key %s already exists, "
                                "and is not of type TAuxStore" ),
                  key.c_str() );
         return TReturnCode::kFailure;
      }

      // Configure the object for variable filtering:
      if( filter ) {
         store->selectAux( *filter );
      }

      // Connect the auxiliary store to the output tree:
      RETURN_CHECK( "xAOD::TEvent::record", store->writeTo( m_outTree ) );

      // Update the manager:
      mgr->setObject( store );

      // Return gracefully:
      return TReturnCode::kSuccess;
   }

   /// This is one of the more important functions of the class. It connects the
   /// event object to a given branch of the input TTree.
   ///
   /// The type that is read in to memory is not actually determined by the type
   /// written to the tree itself, but from the xAOD::EventFormat object. Which
   /// is there to make it possible to possibly use ROOT read rules in the
   /// future to read a different type than what's on disk.
   ///
   /// Everything else is fairly basic ROOT stuff, the code just has to make
   /// sure that the memory management is set up correctly for the created
   /// object.
   ///
   /// @param key The name of the branch to connect to
   /// @param silent Set to <code>kTRUE</code> to make the code fail silently
   ///               in case the branch can't be connected to
   /// @return <code>kTRUE</code> if the connection was successful, or
   ///         <code>kFALSE</code> if it was not
   ///
   TReturnCode TEvent::connectBranch( const std::string& key,
                                      ::Bool_t silent ) {

      // A little sanity check:
      if( ! m_inTree ) {
         ::Error( "xAOD::TEvent::connectBranch",
                  XAOD_MESSAGE( "Function called on un-initialised object" ) );
         return TReturnCode::kFailure;
      }

      // Increment the access counter on this container:
      IOStats::instance().stats().readContainer( key );

      // Check if the branch is already connected:
      if( m_inputObjects.find( key ) != m_inputObjects.end() ) {
         return TReturnCode::kSuccess;
      }

      // Check if we have metadata about this branch:
      const xAOD::EventFormatElement* ef = 0;
      if( ! m_inputEventFormat.exists( key ) ) {
         if( ! silent ) {
            ::Warning( "xAOD::TEvent::connectBranch",
                       "No metadata available for branch: %s",
                       key.c_str() );
         }
      } else {
         ef = m_inputEventFormat.get( key );
      }

      // Check if the branch exists in our input tree:
      ::TBranch* br = m_inTree->GetBranch( key.c_str() );
      if( ! br ) {
         if( ! silent ) {
            ::Warning( "xAOD::TEvent::connectBranch",
                       "Branch \"%s\" not available on input",
                       key.c_str() );
         }
         return TReturnCode::kRecoverable;
      }

      // Make sure that it's not in "MakeClass mode":
      br->SetMakeClass( 0 );

      // Decide about the type that we need to use for the reading of this
      // branch:
      std::string className = br->GetClassName();
      if( className == "" ) {
         if( ef ) {
            // This is a fairly weird situation, but let's fall back to taking
            // the class name from the metadata object in this case.
            className = ef->className();
         } else {
            ::Error( "xAOD::TEvent::connectBranch",
                     XAOD_MESSAGE( "Couldn't find an appropriate type with a "
                                   "dictionary for branch \"%s\"" ),
                     key.c_str() );
            return TReturnCode::kFailure;
         }
      }
      ::TClass* realClass = ::TClass::GetClass( className.c_str() );
      if( ( ( ! realClass ) || ( ! realClass->IsLoaded() ) ) && ef ) {
         // We may need to do an actual schema evolution here, in which
         // case let's fall back on the class name coming from the metadata
         // object.
         className = ef->className();
         realClass = ::TClass::GetClass( className.c_str() );
      }
      if( ( ! realClass ) || ( ! realClass->IsLoaded() ) ) {
         // Now we're in trouble...
         ::Error( "xAOD::TEvent::connectBranch",
                  XAOD_MESSAGE( "Couldn't find an appropriate type with a "
                                "dictionary for branch \"%s\"" ),
                  key.c_str() );
         return TReturnCode::kFailure;
      }

      // Make sure that the current object is the "active event":
      setActive();

      // The data type is always "other" for us:
      static const ::EDataType dataType = kOther_t;

      // Check if the output already has this object. If it does, let's
      // assume that we have been copying the object to the output. Which
      // means that we need to resume filling the same memory address that
      // the output holder points to.
      void* ptr = 0;
      Object_t::const_iterator out_itr = m_outputObjects.find( key );
      if( out_itr != m_outputObjects.end() ) {
         // It needs to be an object manager...
         const TObjectManager* mgr =
            dynamic_cast< const TObjectManager* >( out_itr->second );
         if( ! mgr ) {
            ::Error( "xAOD::TEvent::connectBranch",
                     XAOD_MESSAGE( "Couldn't access output manager for: %s" ),
                     key.c_str() );
            return TReturnCode::kFailure;
         }
         // Get the pointer out of it:
         ptr = mgr->holder()->get();
      }

      // If there is no output object, then let's create one ourselves.
      // This is the only way in which we can have the memory management of
      // THolder do the right thing with this object.
      if( ! ptr ) {
         ptr = realClass->New();
      }

      // Create the new manager object that will hold this EDM object:
      THolder* hldr = new THolder( ptr, realClass );
      TObjectManager* mgr =
         new TObjectManager( 0, hldr, ( m_auxMode == kAthenaAccess ) );
      m_inputObjects[ key ] = mgr;

      // One final check. If it's not an auxiliary store, then it must have
      // a split level of 0. Otherwise read rules may not work on it. Causing
      // *very* serious silent corruption in the data read, if we don't use
      // the "Athena read mode".
      if( ( m_auxMode != kAthenaAccess ) && ( br->GetSplitLevel() != 0 ) &&
          ( ! isAuxStore( *mgr ) ) ) {
         ::Error( "xAOD::TEvent::connectBranch",
                  XAOD_MESSAGE( "Split level for branch \"%s\" is %i. "
                                "This can only be read in kAthenaAccess mode." ),
                  key.c_str(), br->GetSplitLevel() );
         // Clean up:
         *( hldr->getPtr() ) = 0;
         delete mgr;
         m_inputObjects.erase( key );
         return TReturnCode::kFailure;
      }

      // Now try to connect to the branch:
      const ::Int_t status = m_inTree->SetBranchAddress( key.c_str(),
                                                         hldr->getPtr(),
                                                         mgr->branchPtr(),
                                                         realClass, dataType,
                                                         kTRUE );
      if( status < 0 ) {
         ::Error( "xAOD::TEvent::connectBranch",
                  XAOD_MESSAGE( "Couldn't connect variable of type \"%s\" to "
                                "input branch \"%s\". Return code: %i" ),
                  className.c_str(), key.c_str(), status );
         // Clean up:
         *( hldr->getPtr() ) = 0;
         delete mgr;
         m_inputObjects.erase( key );
         return TReturnCode::kFailure;
      }

      // If it's an auxiliary store object, set it up correctly:
      if( isAuxStore( *mgr ) ) {
         RETURN_CHECK( "xAOD::TEvent::connectBranch",
                       setUpDynamicStore( *mgr, m_inTree ) );
      }

      // Return here if the object can't have an auxiliary store:
      if( ! hasAuxStore( *mgr ) ) return TReturnCode::kSuccess;

      // If there may be an auxiliary object connected to this one,
      // connect that as well:
      return connectAux( key + "Aux.", isStandalone( *mgr ) );
   }

   /// This is the function doing the heavy lifting with creating metadata
   /// objects in memory out of the payload of the input file.
   ///
   /// @param key The key (branch name) of the metadata object to retrieve
   /// @param silent Set to <code>kTRUE</code> to make the code fail silently
   ///               in case the branch can't be connected to
   /// @returns The usual TReturnCode types
   ///
   TReturnCode TEvent::connectMetaBranch( const std::string& key,
                                          ::Bool_t silent ) {

      // A little sanity check:
      if( ! m_inMetaTree ) {
         ::Error( "xAOD::TEvent::connectMetaBranch",
                  XAOD_MESSAGE( "Function called on un-initialised object" ) );
         return TReturnCode::kFailure;
      }

      // Check if the branch is already connected:
      if( m_inputMetaObjects.find( key ) != m_inputMetaObjects.end() ) {
         return TReturnCode::kSuccess;
      }

      // Check if the branch exists in our metadata tree:
      ::TBranch* br = m_inMetaTree->GetBranch( key.c_str() );
      if( ! br ) {
         if( ! silent ) {
            ::Warning( "xAOD::TEvent::connectMetaBranch",
                       "Branch \"%s\" not available on input",
                       key.c_str() );
         }
         return TReturnCode::kRecoverable;
      }

      // Check that we have an entry in the branch:
      if( ! br->GetEntries() ) {
         if( ! silent ) {
            ::Warning( "xAOD::TEvent::connectMetaBranch",
                       "Branch \"%s\" doesn't hold any data",
                       key.c_str() );
         }
         return TReturnCode::kRecoverable;
      }

      // Make sure that it's not in "MakeClass mode":
      br->SetMakeClass( 0 );

      // Extract the type of the branch:
      ::TClass* cl = 0;
      ::EDataType dt = kOther_t;
      if( br->GetExpectedType( cl, dt ) || ( ! cl ) ) {
         ::Error( "xAOD::TEvent::connectMetaBranch",
                  XAOD_MESSAGE( "Couldn't get the type for metadata "
                                "branch %s" ), key.c_str() );
         return TReturnCode::kFailure;
      }

      // Create the object, and all of the managers around it:
      void* ptr = cl->New();
      THolder* hldr = new THolder( ptr, cl );
      TObjectManager* mgr =
         new TObjectManager( 0, hldr, m_auxMode == kAthenaAccess );
      m_inputMetaObjects[ key ] = mgr;

      // Now try to connect to the branch:
      const ::Int_t status = m_inMetaTree->SetBranchAddress( key.c_str(),
                                                             hldr->getPtr(),
                                                             mgr->branchPtr(),
                                                             cl, dt,
                                                             kTRUE );
      if( status < 0 ) {
         ::Error( "xAOD::TEvent::connectMetaBranch",
                  XAOD_MESSAGE( "Couldn't connect variable of type \"%s\" to "
                                "input branch \"%s\". Return code: %i" ),
                  cl->GetName(), key.c_str(), status );
         // Clean up:
         *( hldr->getPtr() ) = 0;
         delete mgr;
         m_inputMetaObjects.erase( key );
         return TReturnCode::kFailure;
      }

      // Read in the object:
      if( mgr->getEntry( 0 ) < 0 ) {
         ::Error( "xAOD::TEvent::connectMetaBranch",
                  XAOD_MESSAGE( "Couldn't read in metadata object with key "
                                "\"%s\"" ), key.c_str() );
         return TReturnCode::kFailure;
      }

      // If it's an auxiliary store object, set it up correctly:
      if( isAuxStore( *mgr ) ) {
         RETURN_CHECK( "xAOD::TEvent::connectBranch",
                       setUpDynamicStore( *mgr, m_inMetaTree ) );
      }

      // Return here if the object can't have an auxiliary store:
      if( ! hasAuxStore( *mgr ) ) return TReturnCode::kSuccess;

      // If there may be an auxiliary object connected to this one,
      // connect that as well:
      RETURN_CHECK( "xAOD::TEvent::connectMetaBranch",
                    connectMetaAux( key + "Aux.", isStandalone( *mgr ) ) );

      // And now connect the first object to its auxiliary store:
      RETURN_CHECK( "xAOD::TEvent::connectMetaBranch",
                    setAuxStore( *mgr, kTRUE ) );

      // We succeeded:
      return TReturnCode::kSuccess;
   }

   /// This function is used internally to connect an auxiliary object to
   /// the input. Based on the configuration of the object it will either
   /// use TAuxStore, or the EDM object that was used to write the auxiliary
   /// information in Athena.
   ///
   /// @param prefix The prefix (main branch name) of the auxiliary data
   /// @param standalone Type of the auxiliary store that should be created
   /// @return <code>kTRUE</code> if the connection was successful, or
   ///         <code>kFALSE</code> if it was not
   ///
   TReturnCode TEvent::connectAux( const std::string& prefix,
                                   ::Bool_t standalone ) {

      // A simple test...
      if( ! m_inTree ) {
         ::Error( "xAOD::TEvent::connectAux",
                  XAOD_MESSAGE( "No input tree is available" ) );
         return TReturnCode::kFailure;
      }

      // Check if we know anything about this auxiliary object:
      if( ( ! m_inTree->GetBranch( prefix.c_str() ) ) &&
          ( m_auxMode == kClassAccess || m_auxMode == kAthenaAccess ) ) {
         // If not, then let's just return right away. Not having
         // an auxiliary object with this name is not an error per se.
         return TReturnCode::kSuccess;
      }

      // Check if the branch is already connected:
      if( m_inputObjects.find( prefix ) != m_inputObjects.end() ) {
         return TReturnCode::kSuccess;
      }

      // Do different things based on the "auxiliary mode" we are in:
      if( m_auxMode == kClassAccess || m_auxMode == kAthenaAccess ) {

         // In "class access mode" let's first connect the concrete auxiliary
         // object to the input:
         RETURN_CHECK( "xAOD::TEvent::connectAux", connectBranch( prefix ) );

         // Access the object's manager:
         Object_t::const_iterator mgr_itr = m_inputObjects.find( prefix );
         if( mgr_itr == m_inputObjects.end() ) {
            ::Fatal( "xAOD::TEvent::connectAux",
                     XAOD_MESSAGE( "There's a logic error in the code" ) );
         }
         const TObjectManager* omgr =
            dynamic_cast< const TObjectManager* >( mgr_itr->second );
         if( ! omgr ) {
            ::Fatal( "xAOD::TEvent::connectAux",
                     XAOD_MESSAGE( "There's a logic error in the code" ) );
         }

         // Check if we can switch out the internal store of this object:
         static const TClass* holderClass =
            TClass::GetClass( typeid( SG::IAuxStoreHolder ) );
         if( ! omgr->holder()->getClass()->InheritsFrom( holderClass ) ) {
            // Nope... So let's just end the journey here.
            return TReturnCode::kSuccess;
         }

         // Try to get the object as an IAuxStoreHolder:
         SG::IAuxStoreHolder* storeHolder =
            reinterpret_cast< SG::IAuxStoreHolder* >(
               omgr->holder()->getAs( typeid( SG::IAuxStoreHolder ) ) );
         if( ! storeHolder ) {
            ::Fatal( "xAOD::TEvent::connectAux",
                     XAOD_MESSAGE( "There's a logic error in the code" ) );
         }

         // A sanity check to see whether the store's type is in sync with the
         // object's type that it will be connected to:
         if( ( standalone && ( storeHolder->getStoreType() !=
                               SG::IAuxStoreHolder::AST_ObjectStore ) ) ||
             ( ( ! standalone ) &&
               ( storeHolder->getStoreType() !=
                 SG::IAuxStoreHolder::AST_ContainerStore ) ) ) {
            ::Error( "xAOD::TEvent::connectAux",
                     XAOD_MESSAGE( "Requested store types inconsistent "
                                   "for: %s" ), prefix.c_str() );
            ::Error( "xAOD::TEvent::connectAux",
                     XAOD_MESSAGE( "standalone = %s, getStoreType() = %i" ),
                     ( standalone ? "kTRUE" : "kFALSE" ),
                     static_cast< int >( storeHolder->getStoreType() ) );
            return TReturnCode::kFailure;
         }

         // Return gracefully:
         return TReturnCode::kSuccess;

      } else if( m_auxMode == kBranchAccess ) {

         // In "branch access mode" let's create a TAuxStore object, and let
         // that take care of the auxiliary store access:
         TAuxStore* store =
            new TAuxStore( prefix.c_str(), kTRUE,
                           ( standalone ? TAuxStore::kObjectStore :
                             TAuxStore::kContainerStore ) );
         // We're using this object to read from the input, it needs to be
         // locked:
         store->lock();
         TAuxManager* mgr = new TAuxManager( store );
         m_inputObjects[ prefix ] = mgr;

         // Now connect the object to the input tree:
         RETURN_CHECK( "xAOD::TEvent::connectAux",
                       store->readFrom( m_inTree ) );

         // Return gracefully:
         return TReturnCode::kSuccess;
      }

      // There was some problem:
      ::Error( "xAOD::TEvent::connectAux",
               XAOD_MESSAGE( "Unknown auxiliary access mode set (%i)" ),
               static_cast< int >( m_auxMode ) );
      return TReturnCode::kFailure;
   }

   /// This function is used internally to connect an auxiliary metadata object
   /// to the input. Based on the configuration of the object it will either
   /// use TAuxStore, or the EDM object that was used to write the auxiliary
   /// information in Athena.
   ///
   /// @param prefix The prefix (main branch name) of the auxiliary data
   /// @param standalone Type of the auxiliary store that should be created
   /// @return The usual TReturnCode types
   ///
   TReturnCode TEvent::connectMetaAux( const std::string& prefix,
                                       ::Bool_t standalone ) {

      // Check if the branch is already connected:
      if( m_inputMetaObjects.find( prefix ) != m_inputMetaObjects.end() ) {
         return TReturnCode::kSuccess;
      }

      // A sanity check:
      if( ! m_inMetaTree ) {
         ::Fatal( "xAOD::TEvent::connectMetaAux",
                  XAOD_MESSAGE( "Internal logic error detected" ) );
         return TReturnCode::kFailure;
      }

      // Do different things based on the "auxiliary mode" we are in:
      if( m_auxMode == kClassAccess || m_auxMode == kAthenaAccess ) {

         // In "class access mode" let's first connect the concrete auxiliary
         // object to the input:
         RETURN_CHECK( "xAOD::TEvent::connectMetaAux",
                       connectMetaBranch( prefix ) );

         // Access the object's manager:
         Object_t::const_iterator mgr_itr = m_inputMetaObjects.find( prefix );
         if( mgr_itr == m_inputMetaObjects.end() ) {
            ::Fatal( "xAOD::TEvent::connectMetaAux",
                     XAOD_MESSAGE( "There's a logic error in the code" ) );
         }
         const TObjectManager* omgr =
            dynamic_cast< const TObjectManager* >( mgr_itr->second );
         if( ! omgr ) {
            ::Fatal( "xAOD::TEvent::connectMetaAux",
                     XAOD_MESSAGE( "There's a logic error in the code" ) );
         }

         // Check if we can switch out the internal store of this object:
         static const TClass* holderClass =
            TClass::GetClass( typeid( SG::IAuxStoreHolder ) );
         if( ! omgr->holder()->getClass()->InheritsFrom( holderClass ) ) {
            // Nope... So let's just end the journey here.
            return TReturnCode::kSuccess;
         }

         // Try to get the object as an IAuxStoreHolder:
         SG::IAuxStoreHolder* storeHolder =
            reinterpret_cast< SG::IAuxStoreHolder* >(
               omgr->holder()->getAs( typeid( SG::IAuxStoreHolder ) ) );
         if( ! storeHolder ) {
            ::Fatal( "xAOD::TEvent::connectMetaAux",
                     XAOD_MESSAGE( "There's a logic error in the code" ) );
         }

         // A sanity check to see whether the store's type is in sync with the
         // object's type that it will be connected to:
         if( ( standalone && ( storeHolder->getStoreType() !=
                               SG::IAuxStoreHolder::AST_ObjectStore ) ) ||
             ( ( ! standalone ) &&
               ( storeHolder->getStoreType() !=
                 SG::IAuxStoreHolder::AST_ContainerStore ) ) ) {
            ::Error( "xAOD::TEvent::connectMetaAux",
                     XAOD_MESSAGE( "Requested store types inconsistent" ) );
            ::Error( "xAOD::TEvent::connectMetaAux",
                     XAOD_MESSAGE( "standalone = %s, getStoreType() = %i" ),
                     ( standalone ? "kTRUE" : "kFALSE" ),
                     static_cast< int >( storeHolder->getStoreType() ) );
            return TReturnCode::kFailure;
         }

         // Return gracefully:
         return TReturnCode::kSuccess;

      } else if( m_auxMode == kBranchAccess ) {

         // In "branch access mode" let's create a TAuxStore object, and let
         // that take care of the auxiliary store access:
         TAuxStore* store =
            new TAuxStore( prefix.c_str(), kTRUE,
                           ( standalone ? TAuxStore::kObjectStore :
                             TAuxStore::kContainerStore ) );
         // We use this object to read data from the input, it needs to be
         // locked:
         store->lock();
         TAuxManager* mgr = new TAuxManager( store );
         m_inputMetaObjects[ prefix ] = mgr;

         // Now connect the object to the input tree:
         RETURN_CHECK( "xAOD::TEvent::connectMetaAux",
                       store->readFrom( m_inMetaTree ) );

         // Tell the auxiliary store which entry to use:
         store->getEntry( 0 );

         // Return gracefully:
         return TReturnCode::kSuccess;
      }

      // There was some problem:
      ::Error( "xAOD::TEvent::connectMetaAux",
               XAOD_MESSAGE( "Unknown auxiliary access mode set (%i)" ),
               static_cast< int >( m_auxMode ) );
      return TReturnCode::kFailure;
   }

   /// This function is used by connectBranch(...) and connectMetaBranch(...)
   /// to set up auxiliary store type objects correctly for accessing dynamic
   /// variables from the input file.
   ///
   /// @param mgr  The object manager of the auxiliary store object
   /// @param tree The tree to read dynamic variables from
   /// @returns The usual <code>TReturnCode</code> types
   ///
   TReturnCode TEvent::setUpDynamicStore( TObjectManager& mgr, ::TTree* tree ) {

      // Check if we can call setName(...) on the object:
      ::TMethodCall setNameCall;
      // Don't use this code in Athena access mode. And just accept that access
      // monitoring is disabled in this case...
      if( m_auxMode != kAthenaAccess ) {
         setNameCall.InitWithPrototype( mgr.holder()->getClass(),
                                        "setName", "const char*" );
         if( setNameCall.IsValid() ) {
            // Yes, there is such a function. Let's call it with the branch
            // name:
            const ::TString params =
               ::TString::Format( "\"%s\"", mgr.branch()->GetName() );
            const char* charParams = params.Data();
            setNameCall.Execute( mgr.holder()->get(), charParams );
         } else {
            // This is weird. What sort of auxiliary container is this? :-/
            ::Warning( "xAOD::TEvent::setUpDynamicStore",
                       "Couldn't find setName(...) function for container %s "
                       " (type: %s)",
                       mgr.branch()->GetName(),
                       mgr.holder()->getClass()->GetName() );
         }
      }

      // Check if we can switch out the internal store of this object:
      static const TClass* holderClass =
         TClass::GetClass( typeid( SG::IAuxStoreHolder ) );
      if( ! mgr.holder()->getClass()->InheritsFrom( holderClass ) ) {
         // Nope... So let's just end the journey here.
         return TReturnCode::kSuccess;
      }

      // Try to get the object as an IAuxStoreHolder:
      SG::IAuxStoreHolder* storeHolder =
         reinterpret_cast< SG::IAuxStoreHolder* >(
                         mgr.holder()->getAs( typeid( SG::IAuxStoreHolder ) ) );
      if( ! storeHolder ) {
         ::Fatal( "xAOD::TEvent::setUpDynamicStore",
                  XAOD_MESSAGE( "There's a logic error in the code" ) );
      }

      // Create a TAuxStore instance that will read the dynamic variables
      // of this container. Notice that the TAuxManager doesn't own the
      // TAuxStore object. It will be owned by the SG::IAuxStoreHolder
      // object.
      TAuxStore* store =
         new TAuxStore( mgr.branch()->GetName(), kFALSE,
                        ( storeHolder->getStoreType() ==
                          SG::IAuxStoreHolder::AST_ObjectStore ?
                          TAuxStore::kObjectStore :
                          TAuxStore::kContainerStore ) );
      // This object is used to read data from the input, it needs to be
      // locked:
      store->lock();
      TAuxManager* amgr = new TAuxManager( store, kFALSE );
      m_inputObjects[ std::string( mgr.branch()->GetName() ) +
                      "Dynamic" ] = amgr;
      RETURN_CHECK( "xAOD::TEvent::setUpDynamicStore",
                    store->readFrom( tree ) );
      // Tell the auxiliary store which entry to use. This is essential for
      // metadata objects, and non-important for event data objects, which will
      // get a possibly different entry loaded in setAuxStore(...).
      store->getEntry( 0 );

      // Give this object to the store holder:
      storeHolder->setStore( store );

      // Return gracefully:
      return TReturnCode::kSuccess;
   }

   /// Every time a DataVector is read in from the input for a new TTree
   /// entry, one needs to re-connect it with its auxiliary store. This
   /// function takes care of this.
   ///
   /// @param mgr The manager object of the DV container
   /// @param metadata Flag specifying whether we're dealing with a metadata
   ///                 or event data object
   /// @return <code>kTRUE</code> if the setup was successful, or
   ///         <code>kFALSE</code> if it was not
   ///
   TReturnCode TEvent::setAuxStore( TObjectManager& mgr,
                                    ::Bool_t metadata ) {

      // Check if we need to do anything:
      if( ( ! hasAuxStore( mgr ) ) && ( ! isAuxStore( mgr ) ) ) {
         return TReturnCode::kSuccess;
      }

      // Get the branch name of the object in question:
      const std::string key = mgr.branch()->GetName();

      // Select which object container to use:
      Object_t& objects = ( metadata ?
                            m_inputMetaObjects : m_inputObjects );

      // If it's an auxiliary store, then all we need to do is to make sure that
      // its dynamic store is updated as well.
      if( ( ! metadata ) && isAuxStore( mgr ) ) {
         Object_t::const_iterator dynAuxMgr =
            objects.find( key + "Dynamic" );
         if( dynAuxMgr != objects.end() ) {
            dynAuxMgr->second->getEntry( m_entry );
         }
         return TReturnCode::kSuccess;
      }

      // Look up the auxiliary object's manager:
      Object_t::const_iterator auxMgr = objects.find( key + "Aux." );
      if( auxMgr == objects.end() ) {
         // Apparently there's no auxiliary object for this DV, so let's
         // give up:
         return TReturnCode::kSuccess;
      }

      if( ! metadata ) {
         // Make sure the auxiliary object is up to date:
         ::Int_t readBytes = auxMgr->second->getEntry( m_entry );

         // Check if there is a separate auxiliary object for the dynamic
         // variables:
         Object_t::const_iterator dynAuxMgr =
            objects.find( key + "Aux.Dynamic" );
         if( dynAuxMgr != objects.end() ) {
            if( readBytes ) {
               // Do different things based on the access mode:
               if( m_auxMode != kAthenaAccess ) {
                  // In "normal" access modes just tell the dynamic store object
                  // to switch to a new event.
                  dynAuxMgr->second->getEntry( m_entry );
               } else {
                  // In "Athena mode" this object has already been deleted when
                  // the main auxiliary store object was switched to the new
                  // event. So let's re-create it:
                  xAOD::TObjectManager& auxMgrRef =
                     dynamic_cast< xAOD::TObjectManager& >( *( auxMgr->second ) );
                  RETURN_CHECK( "xAOD::TEvent::setAuxStore",
                                setUpDynamicStore( auxMgrRef,
                                                   ( metadata ?
                                                     m_inMetaTree :
                                                     m_inTree ) ) );
                  // Now tell the newly created dynamic store object which event
                  // it should be looking at:
                  Object_t::const_iterator dynAuxMgr =
                     objects.find( key + "Aux.Dynamic" );
                  if( dynAuxMgr == objects.end() ) {
                     ::Error( "xAOD::TEvent::setAuxStore",
                              XAOD_MESSAGE( "Internal logic error detected" ) );
                     return TReturnCode::kFailure;
                  }
                  dynAuxMgr->second->getEntry( m_entry );
               }
            }
         }
      }

      // Access the auxiliary base class of the object/vector:
      SG::AuxVectorBase* vec = 0;
      SG::AuxElement* aux = 0;
      switch (mgr.holder()->typeKind()) {
      case THolder::DATAVECTOR: {
         void* vvec = mgr.holder()->getAs( typeid( SG::AuxVectorBase ) );
         vec = reinterpret_cast< SG::AuxVectorBase* >( vvec );
         break;
      }
      case THolder::AUXELEMENT: {
         void* vaux = mgr.holder()->getAs( typeid( SG::AuxElement ) );
         aux = reinterpret_cast< SG::AuxElement* >( vaux );
         break;
      }
      default:
        break;
      }

      // Check whether index tracking is enabled for the type. If not, let's not
      // get any further:
      if( vec && ( ! vec->trackIndices() ) ) {
      /*
         ::Warning( "xAOD::TEvent::setAuxStore", "Can't associate auxiliary "
                    "store to container %s, as it's not tracking its indices",
                    key.c_str() );
         return TReturnCode::kSuccess;
         */
         vec->m_trackIndices = true;
      }

      // Check if we were successful:
      if( ( ! vec ) && ( ! aux ) ) {
         ::Fatal( "xAOD::TEvent::setAuxStore",
                  XAOD_MESSAGE( "Couldn't access class \"%s\" as "
                                "SG::AuxVectorBase or SG::AuxElement" ),
                  mgr.holder()->getClass()->GetName() );
      }

      // Get the auxiliary store object:
      const SG::IConstAuxStore* store = 0;
      if( m_auxMode == kBranchAccess ) {
         // Get the concrete auxiliary manager:
         TAuxManager* amgr = dynamic_cast< TAuxManager* >( auxMgr->second );
         if( ! amgr ) {
            ::Fatal( "xAOD::TEvent::setAuxStore",
                     XAOD_MESSAGE( "Auxiliary manager for \"%s\" is not of the "
                                   "right type" ), auxMgr->first.c_str() );
         }
         store = amgr->getConstStore();
         // If the store still doesn't know its type, help it now:
         if( amgr->getStore()->structMode() == TAuxStore::kUndefinedStore ) {
            const TAuxStore::EStructMode mode = ( vec ?
                                                  TAuxStore::kContainerStore :
                                                  TAuxStore::kObjectStore );
            RETURN_CHECK( "xAOD::TEvent::setAuxStore",
                          amgr->getStore()->setStructMode( mode ) );
         }
      } else if( m_auxMode == kClassAccess || m_auxMode == kAthenaAccess ) {
         // Get the concrete auxiliary manager:
         TObjectManager* omgr =
            dynamic_cast< TObjectManager* >( auxMgr->second );
         if( ! omgr ) {
            ::Fatal( "xAOD::TEvent::setAuxStore",
                     XAOD_MESSAGE( "Auxiliary manager for \"%s\" is not of the "
                                   "right type" ), auxMgr->first.c_str() );
         }
         void* p = omgr->holder()->getAs( typeid( SG::IConstAuxStore ) );
         store = reinterpret_cast< const SG::IConstAuxStore* >( p );
      }
      if( ! store ) {
         ::Fatal( "xAOD::TEvent::setAuxStore",
                  XAOD_MESSAGE( "Logic error detected in the code" ) );
      }

      // Connect the two:
      if( vec ) {
         vec->setStore( store );
      } else if( aux ) {
         aux->setStore( store );
      } else {
         ::Fatal( "xAOD::TEvent::setAuxStore",
                  XAOD_MESSAGE( "Logic error detected in the code" ) );
      }

      // We succeeded:
      return TReturnCode::kSuccess;
   }

   /// This function is used internally to set up the writing of the auxiliary
   /// store variables that were dynamically created on an object. (And not
   /// statically defined to be part of that object.)
   ///
   /// @param outTree The TTree to put the auxiliary branches into
   /// @param mgr The object manager of the output object
   /// @param basketSize Size of the buffer associated with the branches
   /// @param splitLevel The split level to use for the created branches
   /// @param metadata Flag specifying whether the info written is metadata or
   ///                 not
   /// @returns <code>kTRUE</code> if the setup was successful, or
   ///         <code>kFALSE</code> if it was not
   ///
   TReturnCode TEvent::putAux( ::TTree& outTree, TVirtualManager& vmgr,
                               ::Int_t basketSize, ::Int_t splitLevel,
                               ::Bool_t metadata ) {

      // A little sanity check:
      assert( m_outputEventFormat != 0 );

      // Do the conversion:
      TObjectManager* mgr = dynamic_cast< TObjectManager* >( &vmgr );
      if( ! mgr ) {
         // It's not an error any more when we don't get a TObjectManager.
         return TReturnCode::kSuccess;
      }

      // Check if we need to do anything here:
      if( ! mgr->holder()->getClass()->InheritsFrom( "SG::IAuxStoreIO" ) ) {
         return TReturnCode::kSuccess;
      }

      // Get a pointer to the auxiliary store I/O interface:
      SG::IAuxStoreIO* aux =
         reinterpret_cast< SG::IAuxStoreIO* >(
            mgr->holder()->getAs( typeid( SG::IAuxStoreIO ) ) );
      if( ! aux ) {
         ::Fatal( "xAOD::TEvent::putAux",
                  XAOD_MESSAGE( "There is a logic error in the code!" ) );
      }

      // Check if we have rules defined for which auxiliary properties
      // to write out:
      if( ! metadata ) {
         std::map< std::string, std::set< std::string > >::const_iterator
            item_itr = m_auxItemList.find( mgr->branch()->GetName() );
         if( item_itr != m_auxItemList.end() ) {
            aux->selectAux( item_itr->second );
         }
      }

      // Get the dynamic auxiliary variables held by this object, which
      // were selected to be written:
      const SG::auxid_set_t& auxids = aux->getSelectedAuxIDs();

      // If there are no dynamic auxiliary variables in the object, return
      // right away:
      if( ! auxids.size() ) {
         return TReturnCode::kSuccess;
      }

      // Decide what should be the prefix of all the dynamic branches:
      const std::string dynNamePrefix =
         Utils::dynBranchPrefix( mgr->branch()->GetName() );

      // Select which container to add the variables to:
      Object_t& objects = ( metadata ? m_outputMetaObjects : m_outputObjects );

      // Extract all the dynamic variables from the object:
      // This iteration will determine the ordering of branches within
      // the tree, so sort auxids by name.
      const SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
      typedef std::pair<std::string, SG::auxid_t> AuxVarSort_t;
      std::vector<AuxVarSort_t> varsort;
      varsort.reserve (auxids.size());
      for (SG::auxid_t id : auxids)
        varsort.emplace_back (r.getName(id), id);
      std::sort (varsort.begin(), varsort.end());
      for (const auto& p : varsort) {
         SG::auxid_t id = p.second;

         // Construct a name for the branch that we will write:
         const std::string brName = dynNamePrefix + p.first;

         // Try to find the branch:
         Object_t::iterator bmgr = objects.find( brName );

         // Check if we already know about this variable:
         if( bmgr == objects.end() ) {

            // Construct the full type name of the variable:
            const std::type_info* brType = aux->getIOType( id );
            const std::string brTypeName =
               Utils::getTypeName( *brType );
            std::string brProperTypeName = "<unknown>";

            // The branch that will hopefully be created:
            ::TBranch* br = 0;

            // Check if it's a primitive type or not:
            if( strlen( brType->name() ) == 1 ) {

               // Making the "proper" type name is simple in this case:
               brProperTypeName = brTypeName;

               // Get the character describing this type for ROOT:
               const char rootType = Utils::rootType( brType->name()[ 0 ] );
               if( rootType == '\0' ) {
                  ::Error( "xAOD::TEvent::putAux",
                           XAOD_MESSAGE( "Type not known for variable \"%s\" "
                                         "of type \"%s\"" ),
                           brName.c_str(), brTypeName.c_str() );
                  return TReturnCode::kFailure;
               }

               // Create the full description of the variable for ROOT:
               std::ostringstream leaflist;
               leaflist << brName << "/" << rootType;

               // Let's create a holder for this property:
               THolder* hldr = new THolder( ( void* ) aux->getIOData( id ),
                                            0, kFALSE );
               TPrimitiveAuxBranchManager* auxmgr =
                  new TPrimitiveAuxBranchManager( id, 0, hldr );
               objects[ brName ] = auxmgr;

               // ... and let's add it to the output TTree:
               *( auxmgr->branchPtr() ) =
                  outTree.Branch( brName.c_str(), hldr->get(),
                                  leaflist.str().c_str(),
                                  basketSize );
               if( ! auxmgr->branch() ) {
                  ::Error( "xAOD::TEvent::putAux",
                           XAOD_MESSAGE( "Failed to create branch \"%s\" out "
                                         "of type \"%s\"" ),
                           brName.c_str(), brProperTypeName.c_str() );
                  // Clean up:
                  *( auxmgr->holder()->getPtr() ) = 0;
                  delete auxmgr;
                  objects.erase( brName );
                  return TReturnCode::kFailure;
               }
               br = auxmgr->branch();

            } else {

               // Check if we have a dictionary for this type:
               TClass* cl = TClass::GetClass( *brType, kTRUE, kTRUE );
               if( ! cl ) {
                  // The dictionary needs to be loaded now. This could be an
                  // issue. But let's hope for the best...
                  cl = TClass::GetClass( brTypeName.c_str() );
                  // If still not found...
                  if( ! cl ) {
                     ::Error( "xAOD::TEvent::putAux",
                              XAOD_MESSAGE( "Dictionary not available for "
                                            "variable \"%s\" of type \"%s\"" ),
                              brName.c_str(), brTypeName.c_str() );
                     return TReturnCode::kFailure;
                  }
               }

               // The proper type name comes from the dictionary in this case:
               brProperTypeName = cl->GetName();

               // Let's create a holder for this property:
               THolder* hldr = new THolder( ( void* ) aux->getIOData( id ),
                                            cl, kFALSE );
               TAuxBranchManager* auxmgr =
                  new TAuxBranchManager( id, 0, hldr );
               objects[ brName ] = auxmgr;

               if (!cl->CanSplit() && strncmp (cl->GetName(), "SG::PackedContainer<", 20) == 0)
                 splitLevel = 0;

               // ... and let's add it to the output TTree:
               *( auxmgr->branchPtr() ) =
                  outTree.Branch( brName.c_str(),
                                  cl->GetName(),
                                  hldr->getPtr(), basketSize, splitLevel );
               if( ! auxmgr->branch() ) {
                  ::Error( "xAOD::TEvent::putAux",
                           XAOD_MESSAGE( "Failed to create branch \"%s\" out "
                                         "of type \"%s\"" ),
                           brName.c_str(), brProperTypeName.c_str() );
                  // Clean up:
                  *( auxmgr->holder()->getPtr() ) = 0;
                  delete auxmgr;
                  objects.erase( brName );
                  return TReturnCode::kFailure;
               }
               br = auxmgr->branch();

            }

            // If this is not the first event, fill up the already filled
            // events with (empty) content:
            if( outTree.GetEntries() ) {
               void* ptr = br->GetAddress();
               br->SetAddress( 0 );
               for( ::Long64_t i = 0; i < outTree.GetEntries(); ++i ) {
                  br->Fill();
               }
               br->SetAddress( ptr );
            }

            // If all went fine, let's add this branch to the event format
            // metadata:
            if( ! m_outputEventFormat->exists( brName ) ) {
               m_outputEventFormat->add(
                      EventFormatElement( brName,
                                          brProperTypeName,
                                          mgr->branch()->GetName(),
                                          getHash( brName ) ) );
            }

            // We don't need to do the rest:
            continue;
         }

         // Access the object manager:
         bmgr = objects.find( brName );
         if( bmgr == objects.end() ) {
            ::Fatal( "xAOD::TEvent::putAux",
                     XAOD_MESSAGE( "There is an internal logic error in the "
                                   "code..." ) );
         }

         // Replace the managed object:
         bmgr->second->setObject( ( void* ) aux->getIOData( id ) );
      }

      // Return gracefully:
      return TReturnCode::kSuccess;
   }

   /// Since the code needs to check in a few places whether a given object
   /// has an auxiliary store, and as this check may change slightly over time,
   /// it seemed to be a good idea to outsource this check into this function.
   ///
   /// @param mgr The manager of the object that should be checked
   /// @returns <code>kTRUE</code> if the object can have an auxiliary store,
   ///          <code>kFALSE</code> otherwise
   ///
   ::Bool_t TEvent::hasAuxStore( const TObjectManager& mgr ) {

      // The classes whose children can have an auxiliary store attached
      // to them:
      static const TClass* dvClass =
         ::TClass::GetClass( typeid( SG::AuxVectorBase ) );
      static const TClass* aeClass =
         ::TClass::GetClass( typeid( SG::AuxElement ) );

      // Do the check:
      return ( mgr.holder()->getClass()->InheritsFrom( dvClass ) ||
               mgr.holder()->getClass()->InheritsFrom( aeClass ) );
   }

   /// This function is used to decide if a given object is an auxiliary store
   /// type or not.
   ///
   /// @param mgr The manager of the object that should be checked
   /// @returns <code>kTRUE</code> if the object is an auxiliary store,
   ///          <code>kFALSE</code> if it is not
   ///
   ::Bool_t TEvent::isAuxStore( const TObjectManager& mgr ) {

      // The classes whose children are considered auxiliary stores:
      static const TClass* storeClass =
         ::TClass::GetClass( typeid( SG::IConstAuxStore ) );
      static const TClass* storeHolderClass =
         ::TClass::GetClass( typeid( SG::IAuxStoreHolder ) );

      // Do the check:
      return ( mgr.holder()->getClass()->InheritsFrom( storeClass ) ||
               mgr.holder()->getClass()->InheritsFrom( storeHolderClass ) );
   }

   /// This function is used to figure out while setting up the reading of the
   /// TTree in kBranchAccess mode, what to tell the underlying TAuxStore
   /// object, what kind of store it needs to be. It's done in pretty much
   /// the same way in which hasAuxStore(...) decides if the object in question
   /// can have an auxiliary store.
   ///
   /// @param mgr The manager of the object that should be checked
   /// @returns <code>kTRUE</code> if the object is a standalone one,
   ///          <code>kFALSE</code> if it is a container
   ///
   ::Bool_t TEvent::isStandalone( const TObjectManager& mgr ) {

      // The classes whose children can have an auxiliary store attached
      // to them:
      static const TClass* dvClass =
         TClass::GetClass( typeid( SG::AuxVectorBase ) );
      static const TClass* aeClass =
         TClass::GetClass( typeid( SG::AuxElement ) );

      // Do the check:
      if( mgr.holder()->getClass()->InheritsFrom( aeClass ) ) {
         return kTRUE;
      } else if( mgr.holder()->getClass()->InheritsFrom( dvClass ) ) {
         return kFALSE;
      }

      // Some logic error happened:
      ::Error( "xAOD::TEvent::isStandalone",
               XAOD_MESSAGE( "Received type (%s) that can't have an "
                             "auxiliary store" ),
               mgr.holder()->getClass()->GetName() );
      return kFALSE;
   }

   /// Created a non-template version of the call internally to be able to use
   /// it from xAOD::MakeTransientTree.
   ///
   /// @param key The key to check
   /// @param ti  The type of the object to check
   /// @param metadata Flag deciding whether we are looking for a metadata
   ///                 object or not
   /// @returns <code>kTRUE</code> if such an object is held by the
   ///          event object at the moment, <code>kFALSE</code> otherwise
   ///
   ::Bool_t TEvent::contains( const std::string& key,
                              const std::type_info& ti,
                              ::Bool_t metadata ) {

      return ( getOutputObject( key, ti, metadata ) ||
               getInputObject( key, ti, kTRUE, metadata ) );
   }

   /// Internal function checking if an object of a given type, with a given
   /// key is held in memory by the event object. It's this function doing
   /// the heavy lifting for the public template function.
   ///
   /// @param key The key/branch name of the object
   /// @param ti The type description of the object
   /// @param metadata Flag deciding whether we are looking for a metadata
   ///                 object or not
   /// @returns <code>kTRUE</code> if such a modifiable object is held by the
   ///          event object at the moment, <code>kFALSE</code> otherwise
   ///
   ::Bool_t TEvent::transientContains( const std::string& key,
                                       const std::type_info& ti,
                                       ::Bool_t metadata ) const {

      return getOutputObject( key, ti, metadata );
   }

} // namespace xAOD
