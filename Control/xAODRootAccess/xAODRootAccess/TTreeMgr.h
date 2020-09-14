// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef XAODROOTACCESS_TTREEMGR_H
#define XAODROOTACCESS_TTREEMGR_H

// System include(s):
#include <memory>
#include <string>
#include <vector>

// Local include(s):
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/tools/TReturnCode.h"
#include "xAODRootAccess/tools/xAODTEventTree.h"
#include "xAODRootAccess/tools/xAODTMetaTree.h"

// Forward declaration(s):
class TTree;
class TFile;

namespace xAOD {

   /// Class creating (a) transient tree(s) from xAOD files
   ///
   /// This class is a replacement for the many MakeTransientTree(...)
   /// functions used earlier. It can be used to create a transient tree
   /// describing the event tree, and one describing the metadata tree
   /// of the input file(s).
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class TTreeMgr {

   public:
      /// Constructor, with an optional access mode selector
      TTreeMgr( TEvent::EAuxMode mode = TEvent::kUndefinedAccess );

      /// @name Function(s) setting up the input(s) for the tree(s)
      /// @{

      /// Read from the file given to the function
      TReturnCode readFrom( ::TFile* file, ::Bool_t useTreeCache = kTRUE,
                            const char* treeName = "CollectionTree" );

      /// @}

      /// @name Function(s) used for filtering the contents of the trees
      /// @{

      /// Object/container names that should be used in the event tree
      TReturnCode enableEventObj( const std::vector< std::string >& names );
      /// Object/container names that should be vetoed from the event tree
      TReturnCode suppressEventObj( const std::vector< std::string >& names );

      /// Object/container names that should be used in the metadata tree
      TReturnCode enableMetaObj( const std::vector< std::string >& names );
      /// Object/container names that should be suppressed in the metadata tree
      TReturnCode suppressMetaObj( const std::vector< std::string >& names );

      /// @}

      /// @name Transient tree accessors
      /// @{

      /// Get a pointer to the transient event tree
      ::TTree* eventTree();

      /// Get a pointer to the transient metadata tree
      ::TTree* metaTree();

      /// @}

      /// Get the TEvent object being used by the transient tree(s)
      TEvent& event();

   private:
      /// The transient event tree
      std::unique_ptr< xAODTEventTree > m_eventTree;
      /// The transient metadata tree
      std::unique_ptr< xAODTMetaTree > m_metaTree;

      /// The internal TEvent object
      TEvent m_event;

      /// The name of the event tree to create
      std::string m_eventTreeName;

      /// Names to select for the event tree
      std::vector< std::string > m_enableEventObj;
      /// Names to suppress from the event tree
      std::vector< std::string > m_suppressEventObj;

      /// Names to select for the metatada tree
      std::vector< std::string > m_enableMetaObj;
      /// Names to suppress from the metadata tree
      std::vector< std::string > m_suppressMetaObj;

   }; // class TTreeMgr

} // namespace xAOD

#endif // XAODROOTACCESS_TTREEMGR_H
