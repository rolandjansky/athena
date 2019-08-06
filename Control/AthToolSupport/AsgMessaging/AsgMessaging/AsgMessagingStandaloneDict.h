/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ASGMESSAGING_ASGMESSAGINGSTANDALONEDICT_H
#define ASGMESSAGING_ASGMESSAGINGSTANDALONEDICT_H

/// This file is used exclusively in standalone compilation to generate
/// dictionaries for types that in Athena are defined in other packages, and
/// hence get their dictionaries from elsewhere. As a result, the Athena
/// compilation must not use this file to generate a dictionary.

#ifdef __GNUC__
# pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#include "AsgMessaging/INamedInterface.h"
#include "AsgMessaging/MsgLevel.h"
#include "AsgMessaging/MsgStream.h"
#include "AsgMessaging/StatusCode.h"

#endif // not ASGMESSAGING_ASGMESSAGINGSTANDALONEDICT_H
