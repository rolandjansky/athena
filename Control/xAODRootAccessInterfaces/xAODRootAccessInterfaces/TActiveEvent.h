// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TActiveEvent.h 594923 2014-04-30 13:28:05Z krasznaa $
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
   /// $Revision: 594923 $
   /// $Date: 2014-04-30 15:28:05 +0200 (Wed, 30 Apr 2014) $
   ///
   class TActiveEvent {

      /// Allow the TEvent class to register itself at the right time
      friend class TEvent;

   public:
      /// Access the currently active TVirtualEvent object
      static TVirtualEvent* event();

   private:
      /// Pointer to the currently active TVirtualEvent object
      static TVirtualEvent* s_event;

   }; // class TActiveEvent

} // namespace xAOD

#endif // XAODROOTACCESSINTERFACES_TACTIVEEVENT_H
