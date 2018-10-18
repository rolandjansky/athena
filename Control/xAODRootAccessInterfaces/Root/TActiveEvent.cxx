/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ROOT include(s):
#include <ThreadLocalStorage.h>

// Local include(s):
#include "xAODRootAccessInterfaces/TActiveEvent.h"

namespace xAOD {

   /// Pointer to the currently active TVirtualEvent object of the thread
   TTHREAD_TLS( TVirtualEvent* ) s_event;

   TVirtualEvent* TActiveEvent::event() {

      return s_event;
   }

   void TActiveEvent::setEvent( TVirtualEvent* ptr ) {

      s_event = ptr;
      return;
   }

} // namespace xAOD
