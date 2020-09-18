//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//

// System include(s):
#include <cstring>
#include <iostream>
#include <memory>

// ROOT include(s):
#include <TFile.h>
#include <TChain.h>
#include <TROOT.h>
#include <TError.h>
#include <TClass.h>
#include <TDirectory.h>

// Local include(s):
#include "xAODRootAccess/MakeTransientTree.h"
#include "xAODRootAccess/tools/xAODTEventTree.h"
#include "xAODRootAccess/tools/xAODTEventBranch.h"
#include "xAODRootAccess/tools/xAODTMetaTree.h"
#include "xAODRootAccess/tools/xAODTMetaBranch.h"
#include "xAODRootAccess/tools/Message.h"
#include "xAODRootAccess/TEvent.h"

namespace {

   /// Helper class for making sure the xAOD::TEvent object is always up to date
   ///
   /// Since the TEvent object needs to access the object inside the TFile
   /// managed by an input TChain, it can quite easily happen that the TChain
   /// for some reason closes the current input file without TEvent knowing
   /// about this.
   ///
   /// Luckily all TTree-s can notify users about updates in their status using
   /// the TObject::Notify() function. Since TEvent doesn't inherit from
   /// TObject, we need to use this small adaptor class here. It sends the
   /// notification signal from the TChain to the TEvent object.
   ///
   /// Since it's not a public class, and we don't need a dictionary for it,
   /// it's fine that we don't use ClassDef/ClassImp for it.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class TEventNotifier : public ::TObject {

   public:
      /// Default constructor
      TEventNotifier()
         : m_tree( 0 ), m_event( 0 ) {}
      /// Constructor with all necessary inputs
      TEventNotifier( ::TTree* tree, xAOD::TEvent* event )
         : m_tree( tree ), m_event( event ) {}

      /// Function giving new objects to this one
      void Setup( ::TTree* tree, xAOD::TEvent* event ) {
         m_tree = tree;
         m_event = event;
         return;
      }

      /// Function called by the tree/chain when a new tree is loaded
      virtual ::Bool_t Notify() {
         if( ( ! m_tree ) || ( ! m_event ) ) {
            ::Error( "TEventNotifier::Notify",
                     XAOD_MESSAGE( "Function called on uninitialised "
                                   "object" ) );
            return kFALSE;
         }
         if( ! m_event->readFrom( m_tree ) ) {
            ::Error( "TEventNotifier::Notify",
                     XAOD_MESSAGE( "Couldn't notify TEvent object of TTree "
                                   "update" ) );
            return kFALSE;
         }
         return kTRUE;
      }

   private:
      /// TTree/TChain that we are reading from
      ::TTree* m_tree;
      /// TEvent object that reads from this tree/chain
      xAOD::TEvent* m_event;

   }; // TEventNotifier

   /// Helper class managing the memory of transient trees
   ///
   /// The TTree type objects allocated by this code need to be managed
   /// here explicitly, and not left to the user. Since this code is mostly
   /// used from PyROOT, where deleting the objects by hand is anything but
   /// simple.
   ///
   /// For a long time the code would only allow a single transient tree to
   /// be open at any given time. But more and more requests came in for
   /// allowing multiple trees to be compared at the same time. So now this
   /// class is used to hold on to the pointers of the created trees.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class TTransObjectHolder {

   private:
      /// Base class for the holder objects
      class TVirtualTransObject {
      public:
         /// Virtual destructor, to make vtable happy...
         virtual ~TVirtualTransObject() {}
      }; // class TVirtualTransObject

      /// Structure holding any type of object
      template< typename T >
      class TTransObject : public TVirtualTransObject {
      public:
         /// Constructor
         TTransObject( std::unique_ptr< T >&& obj )
            : m_obj( std::move( obj ) ) {}
         /// Destructor
         ~TTransObject() {
            // If ROOT is already deleted, then at least some of the objects
            // are probably also deleted by now.
            if( ! ROOT::Internal::gROOTLocal ) {
               m_obj.release();
            }
            // If we're in global cleanup, then they may also have been deleted.
            if (TTransObjectHolder::s_inCleanup)
              m_obj.release();
         }
      private:
         std::unique_ptr< T > m_obj;
      }; // struct TTransObject

   public:
      /// Default constructor
      TTransObjectHolder() : m_objects() {}

      /// Add a new object to be managed:
      template< typename T >
      void add( std::unique_ptr< T >&& obj ) {
         m_objects.push_back( std::unique_ptr< TVirtualTransObject >(
            new TTransObject< T >( std::move( obj ) ) ) );
         return;
      }

      /// Clear out all the managed objects from memory
      void clear() {
         m_objects.clear();
         return;
      }

      // Used to flag that we're cleaning up global objects at the end
      // of the job.
      struct Canary
      {
        ~Canary() { TTransObjectHolder::s_inCleanup = true; }
      };
   private:
      /// The managed transient objects
      std::vector< std::unique_ptr< TVirtualTransObject > > m_objects;

      /// Are we cleaning up global objects at the end of the job?
      static bool s_inCleanup;
   }; // class TTransObjectHolder

   bool TTransObjectHolder::s_inCleanup = false;

} // private namespace

namespace xAOD {

   /// Object managing all the transient objects created on the heap
   static ::TTransObjectHolder s_objectHolder;
   /// To flag when we're in global cleanup.  Must come after s_objectHolder.
   static ::TTransObjectHolder::Canary s_canary;

   //
   // Forward declare the private functions:
   //
   ::TTree* MakeTransientTree( TEvent& event, const char* treeName );
   ::TTree* MakeTransientMetaTree( TEvent& event, ::TTree* persMetaTree );

   TTransTrees MakeTransientTrees( ::TFile* ifile,
                                   const char* treeName,
                                   TEvent::EAuxMode mode ) {

      // Create a TEvent object that will take care of retrieving objects
      // from the persistent trees:
      std::unique_ptr< TEvent > event( new TEvent( mode ) );
      if( ! event->readFrom( ifile, kTRUE, treeName ) ) {
         ::Error( "xAOD::MakeTransientTrees",
                 XAOD_MESSAGE( "Couldn't set up the reading of the input "
                               "file" ) );
         return TTransTrees( 0, 0 );
      }

      // Access the metadata tree of the file:
      ::TTree* metaTree = dynamic_cast< ::TTree* >( ifile->Get( "MetaData" ) );
      if( ! metaTree ) {
         ::Error( "xAOD::MakeTransientTrees",
                  XAOD_MESSAGE( "Couldn't access metadata tree \"MetaData\" "
                                "in the input file" ) );
         return TTransTrees( 0, 0 );
      }

      // Create the transient tree using this TEvent object:
      ::TTree* ev = MakeTransientTree( *event, treeName );
      ::TTree* meta = MakeTransientMetaTree( *event, metaTree );

      // If no problem happened so far, let's put in the TEvent object into
      // the object holder:
      s_objectHolder.add( std::move( event ) );

      // Return the tree pointers:
      return TTransTrees( ev, meta );
   }

   void ClearTransientTrees() {

      s_objectHolder.clear();
      return;
   }

   ::TTree* MakeTransientTree( TEvent& event, const char* treeName ) {

      // Get the input file's format:
      const EventFormat* ef = event.inputEventFormat();
      if( ! ef ) {
         ::Error( "xAOD::MakeTransientTree",
                  XAOD_MESSAGE( "Couldn't get the input file's format "
                                "metadata" ) );
         return 0;
      }

      // Remember which directory we are in now:
      ::TDirectory* dir = gDirectory;

      // Create the transient tree:
      gROOT->cd();
      auto eventTree = std::make_unique< xAODTEventTree >( event, treeName );

      // Go back to the original directory:
      dir->cd();

      // Make sure that the first file/event is loaded at this point.
      if( event.getEntry( 0 ) < 0 ) {
         ::Error( "xAOD::MakeTransientTree",
                  XAOD_MESSAGE( "Couldn't load first event" ) );
         return 0;
      }

      // Create a branch for each xAOD interface object:
      EventFormat::const_iterator ef_itr = ef->begin();
      EventFormat::const_iterator ef_end = ef->end();
      for( ; ef_itr != ef_end; ++ef_itr ) {

         // Convenience reference:
         const xAOD::EventFormatElement& efe = ef_itr->second;

         // Just skip all branches ending in "Aux.":
         if( efe.branchName().rfind( "Aux." ) ==
             ( efe.branchName().size() - 4 ) ) {
            continue;
         }
         // Also skip dynamic branches:
         if( efe.parentName() != "" ) {
            continue;
         }

         // Don't bother if the branch is not available:
         if( ! event.m_inTree->GetBranch( efe.branchName().c_str() ) ) {
            continue;
         }

         // Check if the class in question is known:
         ::TClass* cl = ::TClass::GetClass( efe.className().c_str() );
         if( ! cl ) continue;

         // Don't add auxiliary store objects:
         if( cl->InheritsFrom( "SG::IConstAuxStore" ) ) {
            continue;
         }

         // Check if we have a type-info for this class:
         const std::type_info* ti = cl->GetTypeInfo();
         if( ! ti ) {
            ::Warning( "xAOD::MakeTransientTree",
                       "Couldn't find std::type_info object for type %s "
                       "(key=%s)", cl->GetName(), efe.branchName().c_str() );
            continue;
         }

         // Check if the object is actually available on the input:
         if( ! event.contains( efe.branchName(), *ti ) ) {
            continue;
         }

         // If everything is right, let's make the branch:
         auto br =
            std::make_unique< xAODTEventBranch >( *eventTree, event, *ti,
                                                  efe.branchName().c_str(),
                                                  efe.className().c_str() );
         eventTree->AddBranch( std::move( br ) );

      }

      // Grab a simple pointer to the tree:
      ::TTree* result = eventTree.get();
      // Now put it into the object store:
      s_objectHolder.add( std::move( eventTree ) );

      // Return the object that was just created:
      ::Info( "xAOD::MakeTransientTree",
              "Created transient tree \"%s\" in ROOT's common memory",
              treeName );
      return result;
   }

   ::TTree* MakeTransientTree( ::TFile* ifile, const char* treeName,
                               TEvent::EAuxMode mode ) {

      // Create a TEvent object that will take care of retrieving objects
      // from the persistent tree:
      std::unique_ptr< TEvent > event( new TEvent( mode ) );
      if( ! event->readFrom( ifile, kTRUE, treeName ) ) {
         ::Error( "xAOD::MakeTransientTree",
                  XAOD_MESSAGE( "Couldn't set up the reading of the input "
                                "file" ) );
         return 0;
      }

      // Create the transient tree using this TEvent object:
      ::TTree* result = MakeTransientTree( *event, treeName );

      // Put the event object into the object store:
      s_objectHolder.add( std::move( event ) );

      // Return the created tree:
      return result;
   }

   ::TTree* MakeTransientTree( ::TChain* itree, TEvent::EAuxMode mode ) {

      // Create the objects used by the function:
      std::unique_ptr< TEvent > event( new TEvent( mode ) );
      std::unique_ptr< ::TEventNotifier > notifier( new ::TEventNotifier() );

      // Set up the TEvent object that will take care of retrieving objects
      // from the persistent tree:
      if( ! event->readFrom( itree ) ) {
         ::Error( "xAOD::MakeTransientTree",
                  XAOD_MESSAGE( "Couldn't set up the reading of the input "
                                "tree/chain" ) );
         return 0;
      }

      // Load the first event, to trigger opening the first input file:
      if( event->getEntry( 0 ) < 0 ) {
         ::Error( "xAOD::MakeTransientTree",
                  XAOD_MESSAGE( "Couldn't load event 0 from the chain" ) );
         return 0;
      }

      // Set up an object taking care of notifying the TEvent object about
      // changes in the TChain's status:
      notifier->Setup( itree, event.get() );
      itree->SetNotify( notifier.get() );

      // Create the transient tree using this TEvent object:
      ::TTree* result = MakeTransientTree( *event, itree->GetName() );

      // Put all objects into the object store:
      s_objectHolder.add( std::move( event ) );
      s_objectHolder.add( std::move( notifier ) );

      // Return the created tree:
      return result;
   }

   ::TTree* MakeTransientMetaTree( TEvent& event, ::TTree* persMetaTree ) {

      // Remember which directory we are in now:
      ::TDirectory* dir = gDirectory;

      // Create the transient metadata tree:
      gROOT->cd();
      auto metaTree = std::make_unique< xAODTMetaTree >( event );

      // Go back to the original directory:
      dir->cd();

      // Loop over the branches of the persistent metadata tree:
      TObjArray* branches = persMetaTree->GetListOfBranches();
      for( ::Int_t i = 0; i < branches->GetEntries(); ++i ) {

         // Access the branch object:
         ::TBranch* br = dynamic_cast< ::TBranch* >( branches->At( i ) );
         if( ! br ) {
            ::Error( "xAOD::MakeTransientMetaTree",
                     XAOD_MESSAGE( "Couldn't access branch %i as a TBranch" ),
                     i );
            continue;
         }

         // Skip the EventFormat branch. That must not be disturbed by the
         // generic metadata handling.
         if( ! strcmp( br->GetName(), "EventFormat" ) ) continue;

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
         auto mbr = std::make_unique< xAODTMetaBranch >( *metaTree, event, *ti,
                                                         br->GetName(),
                                                         br->GetClassName() );
         metaTree->AddBranch( std::move( mbr ) );
      }

      // Grab a simple pointer to the tree:
      ::TTree* result = metaTree.get();
      // Now put it into the object store:
      s_objectHolder.add( std::move( metaTree ) );

      // Return the object that was just created:
      ::Info( "xAOD::MakeTransientMetaTree",
              "Created transient metadata tree \"MetaData\" in ROOT's common "
              "memory");
      return result;
   }

   ::TTree* MakeTransientMetaTree( ::TFile* ifile,
                                   const char* eventTreeName,
                                   TEvent::EAuxMode mode ) {

      // Create a TEvent object that will take care of retrieving objects
      // from the persistent tree:
      std::unique_ptr< TEvent > event( new TEvent( mode ) );
      if( ! event->readFrom( ifile, kTRUE, eventTreeName ) ) {
         ::Error( "xAOD::MakeTransientMetaTree",
                  XAOD_MESSAGE( "Couldn't set up the reading of the input "
                                "file" ) );
         return 0;
      }

      // Access the metadata tree directly:
      ::TTree* metaTree = dynamic_cast< ::TTree* >( ifile->Get( "MetaData" ) );
      if( ! metaTree ) {
         ::Error( "xAOD::MakeTransientMetaTree",
                  XAOD_MESSAGE( "Couldn't access metadata tree \"MetaData\" "
                                "in the input file" ) );
         return 0;
      }

      // Create the transient tree using this TEvent object:
      ::TTree* result = MakeTransientMetaTree( *event, metaTree );

      // Put the event object into the object store:
      s_objectHolder.add( std::move( event ) );

      // Return the created tree:
      return result;
   }

   ::TTree* MakeTransientMetaTree( ::TChain* ichain,
                                   const char* eventTreeName,
                                   TEvent::EAuxMode mode ) {

      // Construct a helper TChain object to give to TEvent:
      std::unique_ptr< ::TChain > helperChain( new ::TChain( eventTreeName ) );
      ::TObjArray* files = ichain->GetListOfFiles();
      for( ::Int_t i = 0; i < files->GetEntries(); ++i ) {
         helperChain->Add( files->At( i )->GetTitle() );
      }

      // Create a TEvent object that will take care of retrieving objects
      // from the persistent tree.
      std::unique_ptr< TEvent > event( new TEvent( mode ) );
      if( ! event->readFrom( helperChain.get(), kTRUE ) ) {
         ::Error( "xAOD::MakeTransientMetaTree",
                  XAOD_MESSAGE( "Couldn't set up the reading of the input "
                                "TChain" ) );
         return 0;
      }

      // Create the transient tree using this TEvent object:
      ::TTree* result = MakeTransientMetaTree( *event, ichain );

      // Put the created objects into the object store:
      s_objectHolder.add( std::move( helperChain ) );
      s_objectHolder.add( std::move( event ) );

      // Return the created tree:
      return result;
   }

} // namespace xAOD
