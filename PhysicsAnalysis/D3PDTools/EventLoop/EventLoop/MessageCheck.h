/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_LOOP__MESSAGE_CHECK_H
#define EVENT_LOOP__MESSAGE_CHECK_H

//        Copyright Iowa State University 2017.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


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
