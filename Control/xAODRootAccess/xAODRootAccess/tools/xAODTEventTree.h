// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef XAODROOTACCESS_TOOLS_XAODTEVENTTREE_H
#define XAODROOTACCESS_TOOLS_XAODTEVENTTREE_H

// Local include(s).
#include "xAODRootAccess/tools/xAODTEventBranch.h"

// ROOT include(s):
#include <TTree.h>

// System include(s).
#include <memory>

// Forward declaration(s).
namespace xAOD {
   class TEvent;
}

/// Transient TTree for interactive xAOD event data access
///
/// This is pretty much the same sort of thing that ARA's TTreeTrans was
/// designed for. In order to implement everything that we need from the
/// transient tree, we need to override some functions within the class.
/// But to the outside user we still want to present the usual TTree
/// interface.
///
/// This class is only meant for interactive and PyROOT access to an xAOD
/// file. It should not be used in compiled analysis code, or for writing
/// information into a file. (The first one will be slow, the second
/// one just impossible.)
///
/// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
///
class xAODTEventTree : public ::TTree {

public:
   /// Constructor with all the necessary parameters
   xAODTEventTree( xAOD::TEvent& event, const char* name = "CollectionTree",
                   const char* title = "xAOD transient tree" );

   /// Function overriding the base class's GetEntry(...)
   virtual ::Int_t GetEntry( ::Long64_t entry, ::Int_t getall = 0 );

   /// Function adding a new xAODTEventBranch to the tree
   void AddBranch( std::unique_ptr< xAODTEventBranch > br );

private:
   /// Pointer to the TEvent reading the persistent tree
   xAOD::TEvent* m_event; //!

   /// Declare the class to ROOT
   ClassDef( xAODTEventTree, 0 )

}; // class xAODTEventTree

#endif // XAODROOTACCESS_TOOLS_XAODTEVENTTREE_H
