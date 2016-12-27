/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TFileMerger.cxx 784654 2016-11-16 17:17:32Z krasznaa $

// System include(s):
#include <cstring>
#include <set>
#include <sstream>

// ROOT include(s):
#include <TFile.h>
#include <TFileMergeInfo.h>
#include <TMemFile.h>
#include <TDirectory.h>
#include <TUUID.h>
#include <TSystem.h>
#include <TString.h>
#include <TList.h>
#include <TMethodCall.h>
#include <TClass.h>
#include <TKey.h>
#include <TTree.h>
#include <TROOT.h>
#include <TFunction.h>
#include <TTreeCloner.h>

// Local include(s):
#include "xAODRootAccess/tools/TFileMerger.h"
#include "xAODRootAccess/tools/Message.h"
#include "xAODRootAccess/tools/ReturnCheck.h"
#include "xAODRootAccess/tools/Utils.h"

/// Implement the auto-generated functions of the class
ClassImp( xAOD::TFileMerger )

namespace xAOD {

   TFileMerger::TFileMerger()
   : TObject(), m_input(), m_copiedInput(), m_output( 0 ),
     m_treesToSkip( { "##Shapes", "##Links", "##Params",
                      "##Sections",
                      "CollectionTreeInDet::Track_tlp2",
                      "POOLContainerForm", "POOLContainer",
                      "POOLCollectionTree", "MetaData",
                      "MetaDataHdrForm", "MetaDataHdr" } ),
     m_events(), m_helperFiles(),
     m_metaDataToolNames(), m_metaDataTools(),
     m_mode( TEvent::kClassAccess ), m_entriesToMerge( kBigNumber ),
     m_verbosity( 0 ) {

   }

   TFileMerger::~TFileMerger() {

      // Close all the files that may still be open:
      closeFiles().ignore();
   }

   TReturnCode TFileMerger::setOutputFileName( const std::string& name,
                                               const std::string& mode ) {

      // Try to open the file:
      m_output = ::TFile::Open( name.c_str(), mode.c_str() );
      if( ! m_output ) {
         Error( "setOutputFileName",
                XAOD_MESSAGE( "Couldn't open output file \"%s\" in mode "
                              "\"%s\"" ),
                name.c_str(), mode.c_str() );
         return TReturnCode::kFailure;
      }

      // Tell the user what happened:
      Info( "setOutputFileName", "Opened \"%s\" for writing",
            name.c_str() );

      // Return ROOT to its main directory in memory:
      gROOT->cd();

      // Return gracefully:
      return TReturnCode::kSuccess;
   }

   TReturnCode TFileMerger::addFile( const std::string& name,
                                     bool copyLocally ) {

      // Decide whether to use the file from its current location, or to make
      // a copy of it first:
      std::string localName = name;
      if( copyLocally ) {
         // Helper object to create a unique name for the local file:
         TUUID uuid;
         // Construct the local name of he file:
         localName =
            TString::Format( "%s/XAODMERGE-%s.root",
                             gSystem->TempDirectory(), uuid.AsString() ).Data();
         // Copy the file:
         if( ! TFile::Cp( name.c_str(), localName.c_str(), kTRUE ) ) {
            Error( "addFile",
                   XAOD_MESSAGE( "Couldn't create local copy of \"%s\" under "
                                 "\"%s\"" ),
                   name.c_str(), localName.c_str() );
            return TReturnCode::kFailure;
         }
         // Tell the user what happened:
         Info( "addFile", "Made a local copy of: %s", name.c_str() );
      }

      // Now try to open the file:
      ::TFile* ifile = ::TFile::Open( localName.c_str(), "READ" );
      if( ! ifile ) {
         Error( "addFile", XAOD_MESSAGE( "Couldn't open file \"%s\"" ),
                localName.c_str() );
         return TReturnCode::kFailure;
      }

      // Add it to our list:
      m_input.push_back( ifile );
      m_copiedInput.push_back( copyLocally );
      Info( "addFile", "Opened \"%s\" for reading", localName.c_str() );

      // Return ROOT to its main directory in memory:
      gROOT->cd();

      // Return gracefully:
      return TReturnCode::kSuccess;
   }

   /// In order to merge various kinds of metadata from the input files
   /// correctly into the output file, it is possible to specify the type names
   /// of dual-use metadata handling tools. The object then creates an instance
   /// of the specified tool, which should take care of merging its metadata
   /// type in the background.
   ///
   /// @param typeName The type name of the tool to create
   /// @returns <code>TReturnCode::kSuccess</code> if the tool was successfully
   ///          created, <code>TReturnCode::kFailure</code> if the tool could
   ///          not be created, and <code>TReturnCode::kRecoverable</code> if
   ///          a tool of this type is already held by the object
   ///
   TReturnCode TFileMerger::addMetaDataTool( const std::string& typeName ) {

      // Check if we already have a tool of this type:
      if( m_metaDataToolNames.find( typeName ) != m_metaDataToolNames.end() ) {
         Warning( "addMetaDataTool",
                  "Tool of type \"%s\" is already specified",
                  typeName.c_str() );
         return TReturnCode::kRecoverable;
      }

      // Try to create an instance of the tool:
      ::TClass* cl = ::TClass::GetClass( typeName.c_str() );
      if( ! cl ) {
         Error( "addMetaDataTool",
                XAOD_MESSAGE( "Tool of type \"%s\" not found" ),
                typeName.c_str() );
         return TReturnCode::kFailure;
      }

      // Remember the type:
      m_metaDataToolNames.insert( typeName );

      // Tell the user what happened:
      if( m_verbosity > 1 ) {
         Info( "addMetaDataTool", "Added tool of type \"%s\"",
               typeName.c_str() );
      }

      // Return gracefully:
      return TReturnCode::kSuccess;
   }

   /// This is the main function of the class. It executes the merging of the
   /// input xAOD files in the specified mode, and then closes all the files.
   ///
   /// @param mode The merging mode to be used
   /// @param entries Entries to be merged into the output file. Only effective
   ///                in slow merging mode.
   /// @return <code>TReturnCode::kSuccess</code> if the merging was successful,
   ///         or <code>TReturnCode::kFailure</code> if it wasn't
   ///
   TReturnCode TFileMerger::merge( EMergeMode mode, ::Long64_t entries ) {

      // Check that we have an output file:
      if( ! m_output ) {
         Error( "merge", XAOD_MESSAGE( "No output file specified" ) );
         return TReturnCode::kFailure;
      }

      // Check that we have input files:
      if( ! m_input.size() ) {
         Warning( "merge", "No input files were specified" );
         return TReturnCode::kRecoverable;
      }

      // Check that we received consistent parameters:
      if( ( mode == kFastMerge ) && ( entries != kBigNumber ) ) {
         Warning( "merge", "Number of entries can't be specified in fast "
                  "merging mode" );
         m_entriesToMerge = kBigNumber;
      } else {
         m_entriesToMerge = entries;
      }

      // Merge all the input files into the output using the recursive merging
      // function:
      for( ::TFile* file : m_input ) {
         RETURN_CHECK( "xAOD::TFileMerger::merge",
                       mergeDirectory( *file, *m_output, mode, true ) );
      }

      // Make sure that everything in the output is written out:
      m_output->SaveSelf();

      // Close all the files:
      RETURN_CHECK( "xAOD::TFileMerger::merge", closeFiles() );

      // Return gracefully:
      return TReturnCode::kSuccess;
   }

   TEvent::EAuxMode TFileMerger::accessMode() const {

      return m_mode;
   }

   void TFileMerger::setAccessMode( TEvent::EAuxMode mode ) {

      m_mode = mode;
      return;
   }

   int TFileMerger::verbosity() const {

      return m_verbosity;
   }

   void TFileMerger::setVerbosity( int value ) {

      m_verbosity = value;
      return;
   }

   const std::set< std::string >& TFileMerger::treesSkipped() const {

      return m_treesToSkip;
   }

   void TFileMerger::addTreeToSkip( const std::string& name ) {

      m_treesToSkip.insert( name );
      return;
   }

   TReturnCode TFileMerger::closeFiles() {

      if( m_verbosity >= 1 ) {
         Info( "closeFiles", "Closing all open files" );
      }

      // A little sanity check:
      if( m_input.size() != m_copiedInput.size() ) {
         Fatal( "closeFiles",
                XAOD_MESSAGE( "Internal logic error detected" ) );
      }

      // Finish writing with the TEvent objects that are still open:
      auto ev_itr = m_events.begin();
      auto ev_end = m_events.end();
      for( ; ev_itr != ev_end; ++ev_itr ) {
         if( ev_itr->second->m_outTree && m_output ) {
            RETURN_CHECK( "xAOD::TFileMerger::closeFiles",
                          ev_itr->second->finishWritingTo( m_output ) );
         }
      }

      // Close and delete the input files:
      for( size_t i = 0; i < m_input.size(); ++i ) {
         m_input[ i ]->Close();
         if( m_verbosity >= 2 ) {
            Info( "closeFiles", "Closed: %s", m_input[ i ]->GetName() );
         }
         if( m_copiedInput[ i ] ) {
            TString p( m_input[ i ]->GetPath() );
            p = p( 0, p.Index( ':', 0 ) );
            if( gSystem->Unlink( p ) ) {
               Error( "closeFiles",
                      XAOD_MESSAGE( "Couldn't remove local file: %s" ),
                      p.Data() );
               return TReturnCode::kFailure;
            }
            if( m_verbosity >= 2 ) {
               Info( "closeFiles", "Deleted: %s", m_input[ i ]->GetName() );
            }
         }
         delete m_input[ i ];
      }
      m_input.clear();
      m_copiedInput.clear();

      // Close the output file:
      if( m_output ) {
         m_output->Close();
         if( m_verbosity >= 2 ) {
            Info( "closeFiles", "Closed: %s", m_output->GetName() );
         }
         delete m_output;
         m_output = 0;
      }

      // Delete all the TEvent objects that were created during the merging:
      m_events.clear();

      // Delete all the helper in-memory files created during the merging:
      for( ::TFile* file : m_helperFiles ) {
         delete file;
      }
      m_helperFiles.clear();

      // Return gracefully:
      return TReturnCode::kSuccess;
   }

   /// This function does most of the heavy-lifting in the code. It recursively
   /// merges all the objects from an input directory into the output file's
   /// corresponding directory.
   ///
   /// @param input Directory in one of the input files
   /// @param output The corresponding directory in the output file
   /// @param mode The merging mode for the xAOD trees
   /// @param topLevelDir Flag whether this is the top directory in the file(s)
   /// @return <code>TReturnCode::kSuccess</code> if the merging succeeded, or
   ///         <code>TReturnCode::kFailure</code> if it didn't
   ///
   TReturnCode TFileMerger::mergeDirectory( ::TDirectory& input,
                                            ::TDirectory& output,
                                            EMergeMode mode,
                                            bool topLevelDir ) {

      if( m_verbosity >= 1 ) {
         Info( "mergeDirectory", "Merging directories:" );
         Info( "mergeDirectory", "  input : %s", input.GetName() );
         Info( "mergeDirectory", "  output: %s", output.GetName() );
      }

      // Get a list of all objects in this directory:
      TList* keyList = input.GetListOfKeys();
      if( ! keyList ) {
         Error( "mergeDirectory",
                XAOD_MESSAGE( "Couldn't get list of keys from input directory "
                              "\"%s\"" ), input.GetName() );
         return TReturnCode::kFailure;
      }

      //
      // Loop over all keys in the directory, and select the ones describing
      // objects that we can deal with. Since one single TTree can appear
      // multiple times in this list (with different "cycles"), keep track of
      // which trees have already been merged into the output.
      //
      std::set< std::string > processedTrees;
      for( Int_t i = 0; i < keyList->GetSize(); ++i ) {
         
         // Convert to a TKey:
         TKey* key = dynamic_cast< TKey* >( keyList->At( i ) );
         if( ! key ) {
            Error( "mergeDirectory",
                   XAOD_MESSAGE( "Couldn't case object to TKey. No idea "
                                 "why..." ) );
            return TReturnCode::kFailure;
         }
         if( m_verbosity >= 3 ) {
            Info( "mergeDirectory", "Evaluating key \"%s;%hi\" with type "
                  "\"%s\"",
                  key->GetName(), key->GetCycle(), key->GetClassName() );
         }

         // Get the object:
         TObject* obj = input.Get( TString::Format( "%s;%hi", key->GetName(),
                                                    key->GetCycle() ) );
         if( ! obj ) {
            Error( "mergeDirectory",
                   XAOD_MESSAGE( "Couldn't access object with name "
                                 "\"%s;%hi\"" ),
                   key->GetName(), key->GetCycle() );
            return TReturnCode::kFailure;
         }

         //
         // Decide how to handle this object:
         //
         if( obj->IsA()->InheritsFrom( "TDirectory" ) ) {

            if( m_verbosity >= 3 ) {
               Info( "mergeDirectory", "Appears to be a TDirectory" );
            }

            // Access the input object as a directory:
            TDirectory* indir = dynamic_cast< TDirectory* >( obj );
            if( ! indir ) {
               Error( "mergeDirectory",
                      XAOD_MESSAGE( "Couldn't cast to object to TDirectory" ) );
               return TReturnCode::kFailure;
            }

            // Check if such a directory already exists in the output:
            TDirectory* outdir =
               dynamic_cast< TDirectory* >( output.Get( key->GetName() ) );
            // If it doesn't let's create it:
            if( ! outdir ) {
               if( ! ( outdir = output.mkdir( key->GetName(),
                                              "dummy title" ) ) ) {
                  Error( "mergeDirectory",
                         XAOD_MESSAGE( "Failed creating subdirectory with "
                                       "name: %s" ),
                         key->GetName() );
                  return TReturnCode::kFailure;
               }
            }

            // Now call this same function recursively. Note that xAOD trees
            // can't be in sub-directories, so the sub-directories are always
            // merged in a fast way.
            RETURN_CHECK( "xAOD::TFileMerger::mergeDirectory",
                          mergeDirectory( *indir, *outdir, kFastMerge,
                                          false ) );

         } else if( obj->IsA()->InheritsFrom( "TTree" ) ) {

            if( m_verbosity >= 3 ) {
               Info( "mergeDirectory", "Appears to be a TTree" );
            }

            // Check if this tree is to be skipped from merging. At least by
            // this code. Because the "MetaData" tree is getting merged. Just
            // not here...
            if( m_treesToSkip.find( key->GetName() ) !=
                m_treesToSkip.end() ) {
               if( m_verbosity >= 4 ) {
                  Info( "mergeDirectory",
                        "Skipping it because of m_treesToSkip" );
               }
               continue;
            }

            // Check if this tree was already processed:
            if( processedTrees.find( key->GetName() ) !=
                processedTrees.end() ) {
               if( m_verbosity >= 4 ) {
                  Info( "mergeDirectory",
                        "Skipping it, because it was processed already" );
               }
               continue;
            }

            if( m_verbosity >= 3 ) {
               Info( "mergeDirectory",
                     "Tree not processed yet. Doing so now." );
            }

            // Make sure that we have the object with the highest cycle number:
            ::TObject* treeObj = input.Get( key->GetName() );
            if( ! treeObj ) {
               Fatal( "mergeDirectory",
                      XAOD_MESSAGE( "Internal logic error found" ) );
            }
            ::TTree* itree = dynamic_cast< TTree* >( treeObj );
            if( ! itree ) {
               Error( "mergeDirectory",
                      XAOD_MESSAGE( "Couldn't access \"%s\" with a TTree "
                                    "pointer" ),
                      treeObj->GetName() );
               return TReturnCode::kFailure;
            }

            // Skip trees that have friends. Those don't play nicely with
            // merging/cloning...
            if( itree->GetListOfFriends() &&
                itree->GetListOfFriends()->GetSize() ) {
               if( m_verbosity >= 2 ) {
                  Info( "mergeDirectory", "TTree \"%s\" has friends; "
                        "skipping from merge", itree->GetName() );
               }
               continue;
            }

            // Trees in the top level directory are all assumed to be xAOD
            // trees. We need to make a TEvent object aware of this output tree
            // regardless of the merge mode. Because it's the TEvent that will
            // take care of merging the metadata from the files.
            if( topLevelDir ) {
               // Instantiate the TEvent object:
               std::unique_ptr< TEvent >& event = m_events[ key->GetName() ];
               if( ( ! event.get() ) || ( event->auxMode() != m_mode ) ) {
                  event.reset( new TEvent( m_mode ) );
                  event->setActive();
               }
               // And now create all the metadata tools:
               RETURN_CHECK( "xAOD::TFileMerger::mergeDirectory",
                             createMetaDataTools() );
               // And now connect it to the input file:
               RETURN_CHECK( "xAOD::TFileMerger::mergeDirectory",
                             event->readFrom( itree ) );
               // Let the user know what's happening:
               Info( "mergeDirectory", "Copying xAOD tree \"%s\"",
                     key->GetName() );
            } else {
               // Let the user know what's happening:
               Info( "mergeDirectory", "Copying non-xAOD tree \"%s\"",
                     key->GetName() );
            }

            // Do quite different things in fast and slow merging modes:
            if( mode == kFastMerge ) {

               // Connect the TEvent object to the output file in order to
               // handle metadata merging correctly:
               if( topLevelDir ) {
                  std::unique_ptr< TEvent >& event = m_events[ key->GetName() ];
                  if( ! event.get() ) {
                     event.reset( new TEvent() );
                  }
                  if( ! event->m_outTree ) {
                     // Create an in-memory file, that's really only needed
                     // for technical reasons. Tricking the TEvent object into
                     // thinking that it's actually writing event data. While
                     // it really isn't.
                     ::TFile* ofile =
                        new ::TMemFile( TString::Format( "%sHelper.root",
                                                         key->GetName() ),
                                        "CREATE" );
                     m_helperFiles.push_back( ofile );
                     RETURN_CHECK( "xAOD::TFileMerger::mergeDirectory",
                                   event->writeTo( ofile, 200,
                                                   key->GetName() ) );
                  }
                  // Merge the input file's EventFormat object into the one
                  // created for the output file:
                  const EventFormat* ief = event->inputEventFormat();
                  EventFormat* oef = event->m_outputEventFormat;
                  if( ( ! ief ) || ( ! oef ) ) {
                     Error( "mergeDirectory",
                            XAOD_MESSAGE( "Internal logic error detected" ) );
                     Error( "mergeDirectory",
                            XAOD_MESSAGE( "ief = %p, oef = %p" ),
                            static_cast< const void* >( ief ),
                            static_cast< void* >( oef ) );
                     return TReturnCode::kFailure;
                  }
                  auto itr = ief->begin();
                  auto end = ief->end();
                  for( ; itr != end; ++itr ) {
                     if( ! oef->exists( itr->first ) ) {
                        oef->add( itr->second );
                     }
                  }
               }

               //
               // See if such a TTree exists in the output already:
               //
               ::TTree* otree =
                  dynamic_cast< ::TTree* >( output.Get( key->GetName() ) );
               if( otree ) {

                  if( m_verbosity >= 3 ) {
                     Info( "mergeDirectory",
                           "Merging the tree into an existing one" );
                  }

                  // Check if there are any branches in either tree that don't
                  // exist in the other one:
                  const std::vector< ::TBranch* > missingMerged =
                     getMissingBranches( otree, itree );
                  const std::vector< ::TBranch* > missingIncoming =
                     getMissingBranches( itree, otree );
                  if( m_verbosity >= 4 ) {
                     Info( "mergeDirectory", "missingMerged.size() = %i",
                           static_cast< int >( missingMerged.size() ) );
                     Info( "mergeDirectory", "missingIncoming.size() = %i",
                           static_cast< int >( missingIncoming.size() ) );
                  }

                  // Add branches with default values for the variables that
                  // appeared in the input tree now, and were not in the output
                  // tree yet.
                  for( ::TBranch* br : missingIncoming ) {
                     if( m_verbosity >= 4 ) {
                        Info( "mergeDirectory", "Adding auxiliary branch: %s",
                              br->GetName() );
                     }
                     RETURN_CHECK( "xAOD::TFileMerger::mergeDirectory",
                                   addAuxBranch( otree, br ) );
                  }

                  // Exclude branches from the incoming tree that have no
                  // dictionary in our environment:
                  const std::vector< ::TBranch* > skippedBranches =
                     getSkippedBranches( itree );
                  for( ::TBranch* br : skippedBranches ) {
                     itree->SetBranchStatus( br->GetName(), 0 );
                     if( m_verbosity >= 4 ) {
                        Info( "mergeDirectory",
                              "Deactivated branch \"%s\" from merging",
                              br->GetName() );
                     }
                  }

                  // Get the indices of the aux branches in the merged tree that
                  // need to be excluded from the fast merging:
                  TObjArray* obranches = otree->GetListOfBranches();
                  std::map< ::Int_t, ::TBranch* > auxIndices;
                  for( ::TBranch* br : missingMerged ) {
                     const Int_t index = obranches->IndexOf( br );
                     if( index < 0 ) {
                        Error( "mergeDirectory",
                               XAOD_MESSAGE( "Internal logic error "
                                             "detected" ) );
                        return TReturnCode::kFailure;
                     }
                     auxIndices[ index ] = br;
                     obranches->RemoveAt( index );
                     if( m_verbosity >= 4 ) {
                        Info( "mergeDirectory", "Removed branch \"%s\" from "
                              "fast merge list", br->GetName() );
                     }
                  }
                  if( auxIndices.size() ) {
                     obranches->Compress();
                  }

                  // Set up a TTreeCloner object for fast-merging the branches
                  // that appear in both trees:
                  ::TTreeCloner
                     cloner( itree, otree, "fast SortBasketsByBranch",
                             ( ::TTreeCloner::kNoWarnings |
                               ::TTreeCloner::kIgnoreMissingTopLevel ) );
                  // Check if the cloner is valid:
                  if( ! cloner.IsValid()) {
                     // Let's check why
                     static const char* okerror = "One of the export branch";
                     if( ::strncmp( cloner.GetWarning(), okerror,
                                    ::strlen( okerror ) ) == 0 ) {
                        // That's fine we will handle it
                     } else {
                        Error( "mergeDirectory",
                               XAOD_MESSAGE( "Invalid TTreeCloner (%s)" ),
                               cloner.GetWarning() );
                        return TReturnCode::kFailure;
                     }
                  }

                  // Run the fast merging:
                  otree->SetEntries( otree->GetEntries() +
                                     itree->GetEntries() );
                  if( ! cloner.Exec() ) {
                     Error( "mergeDirectory",
                            XAOD_MESSAGE( "Failed to execute TTreeCloner" ) );
                     return TReturnCode::kFailure;
                  }

                  // Put back the branches that have no counterpart in the input
                  // tree:
                  for( auto it : auxIndices ) {
                     const Int_t last = obranches->GetLast();
                     if( last >= 0 ) {
                        obranches->AddAtAndExpand( obranches->At( last ),
                                                   last + 1 );
                        for( Int_t ind = last - 1; ind >= it.first; --ind ) {
                           obranches->AddAt( obranches->At( ind ), ind + 1 );
                        }
                        obranches->AddAt( it.second, it.first );
                     } else {
                        obranches->Add( it.second );
                     }
                  }
                  // And fill them with default values:
                  const ::Long64_t ientries = itree->GetEntries();
                  for( ::TBranch* br : missingMerged ) {
                     br->SetAddress( 0 );
                     for( ::Long64_t i = 0; i < ientries; ++i ) {
                        if( br->Fill() < 0 ) {
                           Error( "mergeDirectory",
                                  XAOD_MESSAGE( "Failed to fill branch \"%s\" "
                                                "with default content" ),
                                  br->GetName() );
                           return TReturnCode::kFailure;
                        }
                     }
                  }

                  // Make sure that the tree is flushed:
                  otree->AutoSave( "FlushBaskets" );

               } else {

                  if( m_verbosity >= 3 ) {
                     Info( "mergeDirectory", "Cloning the tree as is" );
                  }

                  //
                  // If it doesn't exist, then use the TTree::CloneTree function
                  // to create a copy of the TTree in the input file. Then save
                  // this copy into the output file.
                  //
                  // But first, let's turn off all the branches that don't have
                  // a dictionary in our environment. As those will have to be
                  // skipped.
                  //
                  const std::vector< ::TBranch* > skippedBranches =
                     getSkippedBranches( itree );
                  for( ::TBranch* br : skippedBranches ) {
                     itree->SetBranchStatus( br->GetName(), 0 );
                     if( m_verbosity >= 4 ) {
                        Info( "mergeDirectory",
                              "Deactivated branch \"%s\" from cloning",
                              br->GetName() );
                     }
                  }

                  // Now do the clone:
                  output.cd();
                  if( ! ( otree =
                          itree->CloneTree( -1,
                                            "fast SortBasketsByBranch" ) ) ) {
                     Error( "mergeDirectory",
                            XAOD_MESSAGE( "TTree \"%s\" couldn't be cloned "
                                          "into the output" ),
                            itree->GetName() );
                     return TReturnCode::kFailure;
                  }
                  otree->SetDirectory( &output );
                  otree->AutoSave();
               }

               // Later on the metadata merging will come here.

            } else if( mode == kSlowMerge ) {

               // Access the TEvent object made for this tree. Such a thing
               // needs to access, because we can only slow-merge top level
               // trees. Which are always assumed to be xAOD trees.
               std::unique_ptr< TEvent >& event = m_events[ key->GetName() ];
               if( ! event.get() ) {
                  event.reset( new TEvent() );
               }

               // Check whether it's already writing to the output:
               if( ! event->m_outTree ) {
                  // Make a TFile pointer out of the TDirectory:
                  ::TFile* ofile = dynamic_cast< ::TFile* >( &output );
                  if( ! ofile ) {
                     Error( "mergeDirectory",
                            XAOD_MESSAGE( "Internal logic error found" ) );
                     return TReturnCode::kFailure;
                  }
                  RETURN_CHECK( "xAOD::TFileMerger::mergeDirectory",
                                event->writeTo( ofile, 200, key->GetName() ) );
               }

               // Let the user know what's happening:
               if( m_verbosity >= 3 ) {
                  Info( "mergeDirectory", "Slow copying xAOD tree \"%s\"",
                        key->GetName() );
               }

               // Loop over the events using the TEvent object:
               const ::Long64_t entries = event->getEntries();
               for( ::Long64_t entry = 0; entry < entries; ++entry ) {

                  // Check whether we're done already:
                  if( event->m_outTree->GetEntries() >= m_entriesToMerge ) {
                     break;
                  }

                  // Load the entry:
                  if( event->getEntry( entry ) < 0 ) {
                     Error( "mergeDirectory",
                            XAOD_MESSAGE( "Couldn't get entry %i from tree "
                                          "%s" ),
                           static_cast< int >( entry ), key->GetName() );
                     return TReturnCode::kFailure;
                  }

                  // Give the user feedback about the processing status:
                  if( ! ( entry % 500 ) ) {
                     Info( "mergeDirectory", "Copied %i / %i entries",
                           static_cast< int >( entry ),
                           ( m_entriesToMerge == kBigNumber ?
                             static_cast< int >( entries ) :
                             static_cast< int >( m_entriesToMerge ) ) );
                  }

                  // Copy the full event to the output:
                  RETURN_CHECK( "xAOD::TFileMerger::mergeDirectory",
                                event->copy() );

                  // Write the event:
                  const ::Int_t bytes = event->fill();
                  if( bytes < 0 ) {
                     Error( "mergeDirectory",
                            XAOD_MESSAGE( "There was an error in writing entry "
                                          "%i from tree %s" ),
                            static_cast< int >( entry ), key->GetName() );
                     return TReturnCode::kFailure;
                  } else if( bytes == 0 ) {
                     Warning( "mergeDirectory",
                              "No payload was written for entry %i",
                              static_cast< int >( entry ) );
                  }
               }

            } else {
               Fatal( "mergeDirectory",
                      XAOD_MESSAGE( "Unknown mode (%i) received" ),
                      static_cast< int >( mode ) );
            }

            // Remember that this TTree has already been processed:
            processedTrees.insert( key->GetName() );

         } else if( obj->IsA()->InheritsFrom( "TObject" ) ) {

            if( m_verbosity >= 3 ) {
               Info( "mergeDirectory", "Appears to be a TObject of type %s",
                     obj->IsA()->GetName() );
            }

            // Skip objects that we don't have a dictionary for:
            ::TClass* cl = ::TClass::GetClass( key->GetClassName() );
            if( ( ! cl ) || ( ! cl->IsLoaded() ) ) {
               continue;
            }

            if( m_verbosity >= 3 ) {
               Info( "mergeDirectory", "We seem to have a dictionary for it" );
            }

            // Check if the object is already in the output:
            TObject* oobj = output.Get( key->GetName() );

            // If the output object already exists, and we can merge this input
            // object into it, then that's all that we need to do:
            if( oobj ) {
               const TReturnCode ret = mergeObject( *obj, *oobj );
               if( ret.isSuccess() ) {
                  // Make sure that the output object is updated in the output
                  // file:
                  oobj->Write( 0, TObject::kOverwrite );
                  continue;
               } else if( ret.isFailure() ) {
                  // This is not good:
                  Error( "mergeDirectory",
                         XAOD_MESSAGE( "Error detected while merging object "
                                       "\"%s\"" ), obj->GetName() );
                  return TReturnCode::kFailure;
               }
            }

            if( m_verbosity >= 3 ) {
               Info( "mergeDirectory", "Simply writing it to the output" );
            }

            // If the object doesn't exist yet, or there's no merge operation
            // defined for it, just write this object to the output as is:
            output.cd();
            obj->Write();
         }
      }

      // Return gracefully:
      return TReturnCode::kSuccess;
   }

   /// This function is used internally to merge two generic ROOT objects in
   /// the file together. It's not the most trivial, since TObject doesn't have
   /// a merge function itself.
   ///
   /// The function fails with <code>TReturnCode::kRecoverable</code> when
   /// encountering types that don't have a Merge(...) function defined. In this
   /// case we need to save both objects, with different versions, into the
   /// output.
   ///
   /// @param input The input object
   /// @param output The output object
   /// @return <code>TReturnCode::Success</code> if the merging succeeded,
   ///         <code>TReturnCode::kFailure</code> if there was a technical
   ///         issue, and <code>TReturnCode::kRecoverable</code> if the objects
   ///         don't define a merging function.
   ///
   TReturnCode TFileMerger::mergeObject( ::TObject& input, ::TObject& output ) {

      if( m_verbosity >= 3 ) {
         Info( "mergeObject", "Called mergeObject( %s, %s )",
               input.GetName(), output.GetName() );
      }

      // A little sanity check:
      if( strcmp( input.IsA()->GetName(), output.IsA()->GetName() ) ) {
         Error( "mergeObject",
                XAOD_MESSAGE( "Received objects of different types" ) );
         Error( "mergeObject",
                XAOD_MESSAGE( "input = %s" ), input.IsA()->GetName() );
         Error( "mergeObject",
                XAOD_MESSAGE( "output = %s" ), output.IsA()->GetName() );
         return TReturnCode::kFailure;
      }

      // Check if there is a merge function defined for these objects:
      ::TMethodCall mergeMethod;
      mergeMethod.InitWithPrototype( output.IsA(), "Merge", "TCollection*" );
      if( ! mergeMethod.IsValid() ) {
         // No, there isn't:
         if( m_verbosity >= 3 ) {
            Info( "mergeObject", "Type doesn't have a Merge function" );
         }
         return TReturnCode::kRecoverable;
      }

      // Put the input object into a list, since that's what we need to give
      // to the merge function.
      ::TList list;
      list.Add( &input );

      // Execute the merging:
      mergeMethod.SetParam( ( Long_t ) &list );
      mergeMethod.Execute( &output );

      if( m_verbosity >= 3 ) {
         Info( "mergeObject", "Merging executed successfully" );
      }

      // Return gracefully:
      return TReturnCode::kSuccess;
   }

   TReturnCode TFileMerger::createMetaDataTools() {

      // Create all the specified tool types:
      for( const std::string& typeName : m_metaDataToolNames ) {

         // If the tool was already created, don't do it again:
         if( m_metaDataTools.find( typeName ) != m_metaDataTools.end() ) {
            continue;
         }

         // Try to create an instance of the tool:
         ::TClass* cl = ::TClass::GetClass( typeName.c_str() );
         if( ! cl ) {
            Error( "createMetaDataTools",
                   XAOD_MESSAGE( "Tool of type \"%s\" not found" ),
                   typeName.c_str() );
            return TReturnCode::kFailure;
         }
         void* ptr = cl->New();
         if( ! ptr ) {
            Error( "createMetaDataTools",
                   XAOD_MESSAGE( "Failed to create an instance of tool "
                                 "\"%s\"" ), typeName.c_str() );
            return TReturnCode::kFailure;
         }

         // Try to call the tool's initialize() function:
         ::TMethodCall initMethod;
         initMethod.InitWithPrototype( cl, "initialize", "" );
         if( initMethod.IsValid() ) {
            // Check the return type of the initialize() function. If it's a
            // class that has an ignore() function, then let's call that.
            const std::string returnTypeName =
               initMethod.GetMethod()->GetReturnTypeNormalizedName();
            ::TClass* returnCl = ::TClass::GetClass( returnTypeName.c_str(),
                                                     kTRUE, kTRUE );
            if( returnCl &&
                returnCl->GetListOfMethods()->FindObject( "ignore" ) ) {
               char* statusCode = 0;
               initMethod.Execute( ptr, "", &statusCode );
               if( ! statusCode ) {
                  Warning( "createMetaDataTools", "No StatusCode was returned" );
               }
               ::TMethodCall ignoreMethod;
               ignoreMethod.InitWithPrototype( returnCl, "ignore", "" );
               if( ignoreMethod.IsValid() && statusCode ) {
                  ignoreMethod.Execute( static_cast< void* >( statusCode ) );
               } else {
                  Error( "createMetaDataTools",
                         XAOD_MESSAGE( "Failed to access the ignore() function "
                                       "of the initialize() function's return "
                                       "type" ) );
               }
            } else {
               // If not, then let's just ignore the return value of the
               // function:

            initMethod.Execute( ptr );
            }
         } else {
            Warning( "createMetaDataTools",
                     "No initialize() function found for tool \"%s\"",
                     typeName.c_str() );
         }

         // Store the pointer of this object:
         m_metaDataTools[ typeName ] = THolder( ptr, cl );
         if( m_verbosity > 0 ) {
            Info( "createMetaDataTools", "Created tool of type \"%s\"",
                  typeName.c_str() );
         }
      }

      // Return gracefully:
      return TReturnCode::kSuccess;
   }

   /// This function finds branches that appear in the first tree, but don't
   /// appear in the second one.
   ///
   /// @param first The "first" tree
   /// @param second The "second" tree
   /// @returns The branches that appear in the "first" tree, but don't appear
   ///          in the "second" one
   ///
   std::vector< ::TBranch* >
   TFileMerger::getMissingBranches( ::TTree* first,
                                    ::TTree* second ) const {

      // The result object:
      std::vector< ::TBranch* > result;

      // List of branches from the two trees:
      ::TObjArray* brFirst  = first->GetListOfBranches();
      ::TObjArray* brSecond = second->GetListOfBranches();

      // Loop over the branches of the first tree:
      for( ::Int_t i = 0; i < brFirst->GetEntries(); ++i ) {
         // Check if such a branch exists in the second tree:
         if( brSecond->FindObject( brFirst->At( i )->GetName() ) ) {
            continue;
         }
         // Access the branch:
         ::TBranch* br = dynamic_cast< ::TBranch* >( brFirst->At( i ) );
         if( ! br ) {
            Fatal( "getMissingBranches",
                   XAOD_MESSAGE( "Couldn't cast branch to TBranch?!?" ) );
         }
         // Skip it if we don't have a dictionary for it:
         ::TClass* cl = 0;
         ::EDataType dt = kOther_t;
         if( br->GetExpectedType( cl, dt ) ) {
            Error( "getMissingBranches",
                   XAOD_MESSAGE( "Type could not be extracted for branch "
                                 "\"%s\"" ), br->GetName() );
            continue;
         }
         if( cl && ( ! cl->IsLoaded() ) ) {
            continue;
         }
         // Okay, this is apparently a branch that we'll be able to handle
         // correctly.
         result.push_back( br );
      }

      // Return the collected branches:
      return result;
   }

   std::vector< ::TBranch* >
   TFileMerger::getSkippedBranches( ::TTree* tree ) const {

      // The result object:
      std::vector< ::TBranch* > result;

      // The list of all branches from the tree:
      ::TObjArray* branches = tree->GetListOfBranches();

      // Select the ones that we don't have a dictionary for. These will not
      // be cloned.
      for( ::Int_t i = 0; i < branches->GetEntries(); ++i ) {
         // The branch:
         ::TBranch* br = dynamic_cast< ::TBranch* >( branches->At( i ) );
         if( ! br ) {
            Fatal( "getSkippedBranches",
                   XAOD_MESSAGE( "Couldn't cast branch to TBranch?!?" ) );
         }
         // Check the type of it:
         ::TClass* cl = 0;
         ::EDataType dt = kOther_t;
         if( br->GetExpectedType( cl, dt ) ) {
            Error( "getSkippedBranches",
                   XAOD_MESSAGE( "Type could not be extracted for branch "
                                 "\"%s\"" ), br->GetName() );
            continue;
         }
         // Skip the branches that come from a class that doesn't have a
         // compiled dictionary:
         if( cl && ( ! cl->IsLoaded() ) ) {
            result.push_back( br );
         }
      }

      return result;
   }

   /// This function is used during fast merging to create branches that have
   /// been encountered first in file N (N >= 2).
   ///
   /// @param otree The output tree to create the branch in
   /// @param ibranch Pointer to the branch in the input tree
   /// @returns The usual <code>TReturnCode</code> types
   ///
   TReturnCode TFileMerger::addAuxBranch( ::TTree* otree,
                                          ::TBranch* ibranch ) const {

      // Get the type of the branch:
      ::TClass* cl = 0;
      ::EDataType dt = kOther_t;
      if( ibranch->GetExpectedType( cl, dt ) ) {
         Error( "addAuxBranch",
                XAOD_MESSAGE( "Type could not be extracted for branch \"%s\"" ),
                ibranch->GetName() );
         return TReturnCode::kFailure;
      }

      // Pointer to the output branch:
      ::TBranch* obranch = 0;

      // Decide what sort of branch it is:
      if( cl ) {
         // It's an object type:
         void* ptr = 0;
         obranch = otree->Branch( ibranch->GetName(), cl->GetName(),
                                  &ptr, ibranch->GetBasketSize(),
                                  ibranch->GetSplitLevel() );
         if( ! obranch ) {
            Error( "addAuxBranch",
                   XAOD_MESSAGE( "Couldn't create auxiliary branch \"%s\" with "
                                 "type: %s" ),
                   ibranch->GetName(), cl->GetName() );
            return TReturnCode::kFailure;
         }
      } else if( dt != kOther_t ) {
         // It's a primitive type:
         const char rootType =
            Utils::rootType( Utils::getTypeInfo( dt ).name()[ 0 ] );
         if( rootType == '\0' ) {
            Error( "addAuxBranch",
                    XAOD_MESSAGE( "Type can't be extracted for EDataType = %i"),
                    static_cast< int >( dt ) );
            return TReturnCode::kFailure;
         }
         std::ostringstream leaflist;
         leaflist << ibranch->GetName() << "/" << rootType;
         void* ptr = 0;
         obranch = otree->Branch( ibranch->GetName(), ptr,
                                  leaflist.str().c_str(),
                                  ibranch->GetBasketSize() );
         if( ! obranch ) {
            Error( "addAuxBranch",
                   XAOD_MESSAGE( "Couldn't create auxiliary branch \"%s\" with "
                                 "data type: %i" ),
                   ibranch->GetName(), static_cast< int >( dt ) );
            return TReturnCode::kFailure;
         }
      } else {
         Error( "addAuxBranch",
                XAOD_MESSAGE( "Couldn't figure out the type of branch \"%s\"" ),
                ibranch->GetName() );
         return TReturnCode::kFailure;
      }

      // Fill up the branch with dummy entries:
      obranch->SetAddress( 0 );
      for( ::Long64_t i = 0; i < otree->GetEntries(); ++i ) {
         if( obranch->Fill() < 0 ) {
            Error( "addAuxBranch",
                  XAOD_MESSAGE( "Failed to fill branch \"%s\" with dummy "
                               "data" ), obranch->GetName() );
            return TReturnCode::kFailure;
         }
      }

      // Return gracefully:
      return TReturnCode::kSuccess;
   }

} // namespace xAOD
