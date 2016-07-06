// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TEventBranch.h 645208 2015-02-09 15:48:59Z krasznaa $
#ifndef XAODROOTACCESS_TOOLS_TEVENTBRANCH_H
#define XAODROOTACCESS_TOOLS_TEVENTBRANCH_H

// System include(s):
#include <typeinfo>

// ROOT include(s):
#include <TBranchObject.h>

namespace xAOD {

   // Forward declaration(s):
   class TEvent;

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
   /// $Revision: 645208 $
   /// $Date: 2015-02-09 16:48:59 +0100 (Mon, 09 Feb 2015) $
   ///
   class TEventBranch : public ::TBranchObject {

   public:
      /// Constructor with all the needed parameters
      TEventBranch( ::TTree* parent, TEvent* event, const std::type_info* ti,
                    const char* brName, const char* brType );

      /// Function getting the current entry for this branch
      virtual ::Int_t GetEntry( ::Long64_t entry, ::Int_t getall );

      /// Function returning the type of ::TBranchObject
      virtual ::TClass* IsA() const;

   private:
      /// Pointer to the TEvent object reading the persistent tree
      TEvent* m_event;
      /// The type of the object that needs to be read
      const std::type_info* m_ti;
      /// Pointer to the object read in using TEvent
      void* m_object; //!

      /// Declare the TObject style functions
#define IsA IsARoot
      ClassDef( xAOD::TEventBranch, 0 )
#undef IsA

   }; // class TEventBranch

} // namespace xAOD

#endif // XAODROOTACCESS_TOOLS_TEVENTBRANCH_H
