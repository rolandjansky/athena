// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef XAODROOTACCESS_TOOLS_XAODTMETABRANCH_H
#define XAODROOTACCESS_TOOLS_XAODTMETABRANCH_H

// System include(s):
#include <typeinfo>

// ROOT include(s):
#include <TBranchObject.h>

// Forward declaration(s).
namespace xAOD {
   class TEvent;
}

/// Specialised class for browsing xAOD metadata interface objects in ROOT
///
/// This is a class extremely similar to xAOD::TEventBranch. But instead of
/// giving access to event level information, it provides a convenient view
/// of the ROOT-readable metadata payload of xAOD files.
///
/// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
///
class xAODTMetaBranch : public ::TBranchObject {

public:
   /// Constructor with all the needed parameters
   xAODTMetaBranch( ::TTree& parent, xAOD::TEvent& event,
                    const std::type_info& ti, const char* brName,
                    const char* brType );

   // Note that the virtual functions are not marked with "override" because the
   // functions defined by ClassDef are not marked like that either. So using
   // "override" on just these functions would introduce a compile time warning.

   /// Function getting the current entry/file for this branch
   virtual ::Int_t GetEntry( ::Long64_t entry, ::Int_t getall );

   /// Function returning the type of ::TBranchObject
   virtual ::TClass* IsA() const;

private:
   /// Pointer to the TEvent object reading the persistent tree
   xAOD::TEvent* m_event; //!
   /// The type of the object that needs to be read
   const std::type_info* m_ti; //!
   /// Pointer to the object read in using TEvent
   void* m_object; //!

   /// Declare the TObject style functions
#define IsA IsARoot
   ClassDef( xAODTMetaBranch, 0 )
#undef IsA

}; // class xAODTMetaBranch

#endif // XAODROOTACCESS_TOOLS_XAODTMETABRANCH_H
