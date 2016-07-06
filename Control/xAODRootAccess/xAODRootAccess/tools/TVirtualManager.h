// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TVirtualManager.h 595278 2014-05-03 09:31:05Z krasznaa $
#ifndef XAODROOTACCESS_TOOLS_TVIRTUALMANAGER_H
#define XAODROOTACCESS_TOOLS_TVIRTUALMANAGER_H

// ROOT include(s):
#include <Rtypes.h>

namespace xAOD {

   /// @short Interface class for the "manager classes"
   ///
   /// The TEvent class handles the reading of (a collection of) branches
   /// through "manager objects". Since different kinds of manager
   /// classes are available, they need a common ancestor to make their
   /// usage nicer.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 595278 $
   /// $Date: 2014-05-03 11:31:05 +0200 (Sat, 03 May 2014) $
   ///
   class TVirtualManager {

   public:
      /// Virtual destructor, to make vtable happy...
      virtual ~TVirtualManager() {}

      /// Function for updating the object in memory if needed
      virtual ::Int_t getEntry( ::Long64_t entry, ::Int_t getall = 0 ) = 0;

      /// Function getting a pointer to the object being handled
      virtual void* object() const = 0;
      /// Function replacing the object being handled
      virtual void setObject( void* obj ) = 0;

      /// Check if the object was set for the current event
      virtual ::Bool_t isSet( ::Bool_t forceSet = kTRUE ) const = 0;
      /// Reset the object at the end of processing of an event
      virtual void reset() = 0;

   }; // class TVirtualManager

} // namespace xAOD

#endif // XAODROOTACCESS_TOOLS_TVIRTUALMANAGER_H
