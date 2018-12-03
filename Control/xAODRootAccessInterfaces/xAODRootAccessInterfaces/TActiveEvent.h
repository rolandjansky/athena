// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODROOTACCESSINTERFACES_TACTIVEEVENT_H
#define XAODROOTACCESSINTERFACES_TACTIVEEVENT_H

namespace xAOD {

   // Forward declaration(s):
   class TVirtualEvent;
   class TEvent;

   /// @short Helper class for finding the active TVirtualEvent object
   ///
   /// The idea here is very similar to the one in Athena. Some code
   /// needs to access the currently "active" event, without receiving
   /// a pointer/reference to the event directly. This code always
   /// makes sure that the currently active TVirtualEvent object is returned
   /// to the user. (In case multiple TEvent objects are being used
   /// to read multiple input files at the same time.)
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class TActiveEvent {

      /// Allow the TEvent class to register itself at the right time
      friend class TEvent;

   public:
      /// Access the currently active TVirtualEvent object
      static TVirtualEvent* event();

   private:
      /// Set the active event pointer
      static void setEvent( TVirtualEvent* ptr );

   }; // class TActiveEvent

} // namespace xAOD

#endif // XAODROOTACCESSINTERFACES_TACTIVEEVENT_H
