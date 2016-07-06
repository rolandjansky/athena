// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TEventTree.h 645208 2015-02-09 15:48:59Z krasznaa $
#ifndef XAODROOTACCESS_TOOLS_TEVENTTREE_H
#define XAODROOTACCESS_TOOLS_TEVENTTREE_H

// ROOT include(s):
#include <TTree.h>

namespace xAOD {

   // Forward declaration(s):
   class TEvent;
   class TEventBranch;

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
   /// $Revision: 645208 $
   /// $Date: 2015-02-09 16:48:59 +0100 (Mon, 09 Feb 2015) $
   ///
   class TEventTree : public ::TTree {

   public:
      /// Constructor with all the necessary parameters
      TEventTree( TEvent* event, const char* name = "CollectionTree",
                  const char* title = "xAOD transient tree" );

      /// Function overriding the base class's GetEntry(...)
      virtual ::Int_t GetEntry( ::Long64_t entry, ::Int_t getall = 0 );

      /// Function adding a new xAOD::TBranch to the tree
      void AddBranch( xAOD::TEventBranch* br );

   private:
      /// Pointer to the TEvent reading the persistent tree
      TEvent* m_event;

      /// Declare the class to ROOT
      ClassDef( xAOD::TEventTree, 0 )

   }; // class TEventTree

} // namespace xAOD

#endif // XAODROOTACCESS_TOOLS_TEVENTTREE_H
