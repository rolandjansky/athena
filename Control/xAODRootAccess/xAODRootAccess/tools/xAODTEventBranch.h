// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef XAODROOTACCESS_TOOLS_XAODTEVENTBRANCH_H
#define XAODROOTACCESS_TOOLS_XAODTEVENTBRANCH_H

// System include(s):
#include <typeinfo>

// ROOT include(s):
#include <TBranchObject.h>

// Forward declaration(s).
namespace xAOD {
   class TEvent;
}

/// Specialised class for browsing xAOD event interface objects in ROOT
///
/// In order to read xAOD interface objects/containers correctly,
/// one needs to use the xAOD::TEvent class. Since the regular ROOT
/// classes can't be taught how to use an external object for reading
/// information, we need to do it using inheritance.
///
/// The design was pretty much stolen from the AthenaROOTAccess code.
/// Luckily in this case the code can be a lot easier. Here we just
/// need to ask the TEvent object for the correct pointer, and we're done.
///
/// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
///
class xAODTEventBranch : public ::TBranchObject {

public:
   /// Constructor with all the needed parameters
   xAODTEventBranch( ::TTree& parent, xAOD::TEvent& event,
                     const std::type_info& ti, const char* brName,
                     const char* brType );

   // Note that the virtual functions are not marked with "override" because the
   // functions defined by ClassDef are not marked like that either. So using
   // "override" on just these functions would introduce a compile time warning.

   /// Function getting the current entry for this branch
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
   ClassDef( xAODTEventBranch, 0 )
#undef IsA

}; // class xAODTEventBranch

#endif // XAODROOTACCESS_TOOLS_XAODTEVENTBRANCH_H
