// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

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
   class TActiveStore {

      /// Allow the TStore class to register itself at the right time
      friend class TStore;

   public:
      /// Access the currently active TStore object
      static TStore* store();

   private:
      /// Set the active store pointer
      static void setStore( TStore* ptr );

   }; // class TActiveStore

} // namespace xAOD

#endif // XAODROOTACCESS_TACTIVESTORE_H
