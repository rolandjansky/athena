//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//

// Local include(s):
#include "xAODRootAccess/tools/xAODTEventBranch.h"
#include "xAODRootAccess/tools/Message.h"
#include "xAODRootAccess/TEvent.h"

ClassImp( xAODTEventBranch )

xAODTEventBranch::xAODTEventBranch( ::TTree& parent, xAOD::TEvent& event,
                                    const std::type_info& ti,
                                    const char* brName, const char* brType )
   : ::TBranchObject(),
     m_event( &event ), m_ti( &ti ), m_object( nullptr ) {

   // Now that the object is established in memory, let's initialise it:
   Init( &parent, 0, brName, brType, &m_object, 32000, 0, -1, kTRUE );
}

/// This is the function doing the heavy lifting. It loads the interface
/// object for the current entry requested. Only doing some minimal
/// overhead in the meanwhile.
///
/// @param entry The entry to be read for this object
/// @param getall When set to 1, read all information about the branch
///               (Not used.)
/// @returns The number of bytes read, or a negative number if
///          not successful
///
::Int_t xAODTEventBranch::GetEntry( ::Long64_t entry, ::Int_t /*getall*/ ) {

   // Check if the branch is active:
   if( TestBit( kDoNotProcess ) ) {
      return 0;
   }

   // Check whether we already have the requested entry:
   if( fReadEntry == entry ) {
      return 1;
   }

   // Load the requested entry:
   if( m_event->getEntry( entry ) < 0 ) {
      Error( "GetEntry",
             XAOD_MESSAGE( "Problems with loading entry %lld" ), entry );
      return -1;
   }

   // Update the internal pointer:
   m_object = const_cast< void* >( m_event->getInputObject( GetName(),
                                                            *m_ti ) );
   if( ! m_object ) {
      Error( "GetEntry",
             XAOD_MESSAGE( "Failed to read variable of type \"%s\" with "
                           "key \"%s\"" ),
             GetClassName(), GetName() );
      return -1;
   }

   // We were successful:
   fReadEntry = entry;
   return 1;
}

/// I took this from the ARA code directly. Apparenly TTree would not know
/// how to handle this object if it reported its actual type. So we pretend
/// that this is a TBranchObject object. That way TBrowser shows its contents
/// nicely.
///
/// @returns The class for TBranchObject
///
::TClass* xAODTEventBranch::IsA() const {

   return ::TBranchObject::IsA();
}
