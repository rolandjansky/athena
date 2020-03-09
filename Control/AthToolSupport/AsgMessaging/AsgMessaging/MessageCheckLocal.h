/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ASG_MESSAGING__MESSAGE_CHECK_LOCAL_H
#define ASG_MESSAGING__MESSAGE_CHECK_LOCAL_H

#include <AsgMessaging/MessageCheck.h>

namespace asg
{
  // putting this in a separate header files, as users of the main
  // header file will mostly not be interested in it, and indeed
  // should not be using it.
  ANA_MSG_HEADER (msgAsgMessaging)
}

#endif
