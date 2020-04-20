/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAATHENAPOOL_EGAMMAMSGUTIL
#define EGAMMAATHENAPOOL_EGAMMAMSGUTIL

#include "AthenaBaseComps/AthMessaging.h"
#include "GaudiKernel/MsgStream.h"
 
#ifdef MSG_DEBUG
#undef MSG_DEBUG
#endif
#ifdef MSG_VERBOSE
#undef MSG_VERBOSE
#endif
#define MSG_DEBUG(log, x) \
     do { if ((log).level()<=MSG::DEBUG) { (log) << MSG::DEBUG << (x) << endmsg; } } while (0)
#define MSG_VERBOSE(log, x) \
     do { if ((log).level()<=MSG::VERBOSE) { (log) << MSG::VERBOSE << (x) << endmsg; } } while (0)

#endif
