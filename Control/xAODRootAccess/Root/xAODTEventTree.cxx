//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//

// Local include(s):
#include "xAODRootAccess/tools/xAODTEventTree.h"
#include "xAODRootAccess/tools/Message.h"
#include "xAODRootAccess/TEvent.h"

ClassImp( xAODTEventTree )

xAODTEventTree::xAODTEventTree( xAOD::TEvent& event, const char* name,
                                const char* title )
   : ::TTree( name, title ), m_event( &event ) {

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
::Int_t xAODTEventTree::GetEntry( ::Long64_t entry, ::Int_t getall ) {

   // Forward the call to the TEvent object:
   if( m_event->getEntry( entry, getall ) < 0 ) {
      Error( "GetEntry",
             XAOD_MESSAGE( "Problems with loading entry %lld" ), entry );
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
void xAODTEventTree::AddBranch( std::unique_ptr< xAODTEventBranch > br ) {

   // Add the branch:
   fBranches.Add( br.release() );
   return;
}
