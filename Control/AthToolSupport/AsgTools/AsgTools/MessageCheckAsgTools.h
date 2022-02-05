/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FILE_AsgTools_MessageCheckAsgTools_h
#define FILE_AsgTools_MessageCheckAsgTools_h

// normally I would just call this header files MessageCheck, but that
// name is already taken by a file I don't want to add this to, so
// MessageCheckAsgTools it is.

#include <AsgMessaging/MessageCheck.h>

namespace asg
{
  ANA_MSG_HEADER (msgComponentConfig)
  ANA_MSG_HEADER (msgProperty)
  ANA_MSG_HEADER (msgToolHandle)
  ANA_MSG_HEADER (msgToolStore)
}

#endif
