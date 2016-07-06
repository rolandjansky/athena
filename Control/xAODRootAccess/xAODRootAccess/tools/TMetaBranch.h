// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TMetaBranch.h 645208 2015-02-09 15:48:59Z krasznaa $
#ifndef XAODROOTACCESS_TOOLS_TMETABRANCH_H
#define XAODROOTACCESS_TOOLS_TMETABRANCH_H

// System include(s):
#include <typeinfo>

// ROOT include(s):
#include <TBranchObject.h>

namespace xAOD {

   // Forward declaration(s):
   class TEvent;

   /// Specialised class for browsing xAOD metadata interface objects in ROOT
   ///
   /// This is a class extremely similar to xAOD::TEventBranch. But instead of
   /// giving access to event level information, it provides a convenient view
   /// of the ROOT-readable metadata payload of xAOD files.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 645208 $
   /// $Date: 2015-02-09 16:48:59 +0100 (Mon, 09 Feb 2015) $
   ///
   class TMetaBranch : public ::TBranchObject {

   public:
      /// Constructor with all the needed parameters
      TMetaBranch( ::TTree* parent, TEvent* event, const std::type_info* ti,
                  const char* brName, const char* brType );

      /// Function getting the current entry/file for this branch
      virtual ::Int_t GetEntry( ::Long64_t entry, ::Int_t getall );

      /// Function returning the type of ::TBranchObject
      virtual ::TClass* IsA() const;

   private:
      /// Pointer to the TEvent object reading the persistent tree
      TEvent* m_event; //!
      /// The type of the object that needs to be read
      const std::type_info* m_ti; //!
      /// Pointer to the object read in using TEvent
      void* m_object; //!

      /// Declare the TObject style functions
#define IsA IsARoot
      ClassDef( xAOD::TMetaBranch, 0 )
#undef IsA

   }; // class TMetaBranch

} // namespace xAOD

#endif // XAODROOTACCESS_TOOLS_TMETABRANCH_H
