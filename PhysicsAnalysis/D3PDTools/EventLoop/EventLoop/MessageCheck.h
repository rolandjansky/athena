/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef EVENT_LOOP__MESSAGE_CHECK_H
#define EVENT_LOOP__MESSAGE_CHECK_H

#include <EventLoop/Global.h>

#include <AsgTools/MessageCheck.h>

namespace EL
{
  ANA_MSG_HEADER (msgEventLoop)

  namespace Detail
  {
    /// \brief print out the currently evaluated exception
    void report_exception ();
  }
}

#endif
