/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TActiveEvent.cxx 594923 2014-04-30 13:28:05Z krasznaa $

// Local include(s):
#include "xAODRootAccessInterfaces/TActiveEvent.h"

namespace xAOD {

   // Initialize the event pointer:
   TVirtualEvent* TActiveEvent::s_event = 0;

   TVirtualEvent* TActiveEvent::event() {

      return s_event;
   }

} // namespace xAOD
