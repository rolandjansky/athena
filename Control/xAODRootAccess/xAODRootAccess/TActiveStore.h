// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TActiveStore.h 599784 2014-06-01 17:58:48Z krasznaa $
#ifndef XAODROOTACCESS_TACTIVESTORE_H
#define XAODROOTACCESS_TACTIVESTORE_H

namespace xAOD {

   // Forward declaration(s):
   class TStore;

   /// @short Helper class for finding the active TStore object
   ///
   /// The idea here is very similar to the one in Athena. Some code
   /// needs to access the currently "active" store, without receiving
   /// a pointer/reference to the store directly. This code always
   /// makes sure that the currently active TStore object is returned
   /// to the user. (In case multiple TStore objects are being used
   /// to read multiple input files at the same time.)
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 599784 $
   /// $Date: 2014-06-01 19:58:48 +0200 (Sun, 01 Jun 2014) $
   ///
   class TActiveStore {

      /// Allow the TStore class to register itself at the right time
      friend class TStore;

   public:
      /// Access the currently active TVirtualEvent object
      static TStore* store();

   private:
      /// Pointer to the currently active TVirtualEvent object
      static TStore* s_store;

   }; // class TActiveStore

} // namespace xAOD

#endif // XAODROOTACCESS_TACTIVESTORE_H
