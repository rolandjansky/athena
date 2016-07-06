/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TEventTree.cxx 645208 2015-02-09 15:48:59Z krasznaa $

// Local include(s):
#include "xAODRootAccess/tools/TEventTree.h"
#include "xAODRootAccess/tools/TEventBranch.h"
#include "xAODRootAccess/tools/Message.h"
#include "xAODRootAccess/TEvent.h"

ClassImp( xAOD::TEventTree )

namespace xAOD {

   TEventTree::TEventTree( xAOD::TEvent* event, const char* name,
                           const char* title )
      : ::TTree( name, title ), m_event( event ) {

      // A security check:
      if( ! m_event ) {
         Error( "TEventTree",
                XAOD_MESSAGE( "No xAOD::TEvent pointer received." ) );
         Error( "TEventTree",
                XAOD_MESSAGE( "Object will be non-functional." ) );
         return;
      }

      // Set up the base class's variables:
      fEntries = m_event->getEntries();
   }

   /// This function is not actually used when browsing the TTree, or when
   /// using TTree::Draw(...). So in interactive usage there's little point to
   /// it. But it is very important for PyROOT usage.
   ///
   /// @param entry The entry from the TTree that should be read
   /// @param getall When set to 1, read all the (active) branches
   /// @returns The number of bytes read, or a negative number in case of
   ///          problems.
   ///
   ::Int_t TEventTree::GetEntry( ::Long64_t entry, ::Int_t getall ) {

      // Forward the call to the TEvent object:
      if( m_event->getEntry( entry, getall ) < 0 ) {
         Error( "GetEntry",
                XAOD_MESSAGE( "Problems with loading entry %i" ),
                static_cast< int >( entry ) );
         return -1;
      }

      // Let the base class know what happened:
      return ::TTree::GetEntry( entry, getall );
   }

   /// This is a simple convenience function for setting up the object
   /// with all of its branches.
   ///
   /// @param br The xAOD::TEventBranch object that should be added to the tree
   ///
   void TEventTree::AddBranch( TEventBranch* br ) {

      // A little sanity check:
      if( ! br ) {
         Error( "AddBranch",
                XAOD_MESSAGE( "Null pointer received!" ) );
         return;
      }

      // Add the branch:
      fBranches.Add( br );
      return;
   }

} // namespace xAOD
