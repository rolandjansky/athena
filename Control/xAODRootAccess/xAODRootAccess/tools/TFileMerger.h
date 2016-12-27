// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TFileMerger.h 784654 2016-11-16 17:17:32Z krasznaa $
#ifndef XAODROOTACCESS_TOOLS_TFILEMERGER_H
#define XAODROOTACCESS_TOOLS_TFILEMERGER_H

// System include(s):
#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>

// ROOT include(s):
#include <TObject.h>

// Local include(s):
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/tools/TReturnCode.h"
#include "xAODRootAccess/tools/THolder.h"

// Forward declaration(s):
class TFile;
class TDirectory;
class TTree;
class TBranch;

namespace xAOD {

   /// Helper class for merging xAOD files
   ///
   /// This is a slightly dubmed down version of ROOT's TFileMerger class in
   /// some respects, and is a bit smarter in other respects. It can be used
   /// by user code to merge xAOD files in a user friendly way from C++ code.
   /// It is also used by the xAODMerge application to do the heavy lifting
   /// during the merge.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 784654 $
   /// $Date: 2016-11-16 18:17:32 +0100 (Wed, 16 Nov 2016) $
   ///
   class TFileMerger : public ::TObject {

   public:
      /// Default constructor
      TFileMerger();
      /// Destructor
      ~TFileMerger();

      /// Set the name of the output file that should be created
      TReturnCode setOutputFileName( const std::string& name,
                                     const std::string& mode = "RECREATE" );

      /// Add a file to the list to be merged, by name
      TReturnCode addFile( const std::string& name,
                           bool copyLocally = false );

      /// Add a metadata tool to be used during the merging
      TReturnCode addMetaDataTool( const std::string& typeName );

      /// Types of merging that can be done
      ///
      /// Non-xAOD trees are always merged using fast TTree merging. It's only
      /// the method for the xAOD tree merging that can be controlled here.
      ///
      enum EMergeMode {
         kSlowMerge = 0, ///< Merging is done using TEvent
         kFastMerge = 1  ///< Merging is done using fast TTree merge
      };

      /// Number of entries that are assumed to be never exceeded in a file
      static const ::Long64_t kBigNumber = 1234567890;

      /// Execute the file merge itself
      TReturnCode merge( EMergeMode mode = kFastMerge,
                         ::Long64_t entries = kBigNumber );

      /// The access mode used for slow and metadata merging
      TEvent::EAuxMode accessMode() const;
      /// Set the access mode used for slow and metadata merging
      void setAccessMode( TEvent::EAuxMode mode );

      /// The current verbosity level
      int verbosity() const;
      /// Set verbosity level (meant for debugging mainly)
      void setVerbosity( int value );

      /// Get the names of the trees that will be skipped from the merge
      const std::set< std::string >& treesSkipped() const;
      /// Add a tree name that should be skipped during the merging
      void addTreeToSkip( const std::string& name );

   private:
      /// Close all the open files
      TReturnCode closeFiles();
      /// Merge the contents of one directory from the input files
      TReturnCode mergeDirectory( ::TDirectory& input, ::TDirectory& output,
                                  EMergeMode mode, bool topLevelDir );
      /// Merge two top level objects that were found in the inputs
      TReturnCode mergeObject( ::TObject& input, ::TObject& output );
      /// Instantiate the metadata handling tools
      TReturnCode createMetaDataTools();
      /// Get the auxiliary branches missing in one of the trees
      std::vector< ::TBranch* > getMissingBranches( ::TTree* first,
                                                    ::TTree* second ) const;
      /// Get the branches that should be skipped from merging
      std::vector< ::TBranch* > getSkippedBranches( ::TTree* tree ) const;
      /// Duplicate an auxiliary branch from the input into the output
      TReturnCode addAuxBranch( ::TTree* otree, ::TBranch* ibranch ) const;

      /// The list of input files to be merged
      std::vector< ::TFile* > m_input;
      /// Whether the files were copied locally before being merged
      std::vector< bool > m_copiedInput;
      /// The output file to write to
      ::TFile* m_output;

      /// Names of the TTrees to skip from the merge
      std::set< std::string > m_treesToSkip;

      /// Helper objects for merging the xAOD Tree(s) from the input file(s)
      std::map< std::string, std::unique_ptr< TEvent > > m_events;
      /// Helper in-memory files used during fast merging
      std::vector< ::TFile* > m_helperFiles;

      /// Names of the metadata handling tools
      std::set< std::string > m_metaDataToolNames;
      /// Metadata tools instantiated for the file merging
      std::map< std::string, THolder > m_metaDataTools;

      /// Access mode for the created TEvent objects
      TEvent::EAuxMode m_mode;

      /// Events to merge into the output
      ::Long64_t m_entriesToMerge;

      /// Verbosity level
      int m_verbosity;

      /// Declare the class as a non-streamable type
      ClassDef( xAOD::TFileMerger, 0 )

   }; // class TFileMerger

} // namespace xAOD

#endif // XAODROOTACCESS_TOOLS_TFILEMERGER_H
