/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETEVENTATHENAPOOL_MSGFIX
#define INDETEVENTATHENAPOOL_MSGFIX

// A few utilities for using MSG service easily in converters. Here
// temporarily until someone comes up with a more general approach.
//
// Thomas Kittelmann March 2011

#include "AthenaBaseComps/AthMessaging.h"
#include "GaudiKernel/MsgStream.h"
#include "CxxUtils/no_sanitize_undefined.h"
 
namespace IDEvtAthPool {
  //Small slightly dirty workaround so we can set the source name on the
  //AthMessaging message streams to something else than "AthenaPoolConverter""
  class MsgStreamSourceSettable : public MsgStream {
  public:
    void setSource(const char*c) { if (m_source!=c) m_source=c; }
  };
  inline void setMsgName NO_SANITIZE_UNDEFINED (AthMessaging*a,const char*c) {
    if (a&&c) static_cast<MsgStreamSourceSettable*>(&(a->msg()))->setSource(c);
  }
}

//Defines similar to ATH_MSG_DEBUG and ATH_MSG_VERBOSE which accepts the MsgStream as an argument: 
#ifdef MSG_DEBUG
#undef MSG_DEBUG
#endif
#ifdef MSG_VERBOSE
#undef MSG_VERBOSE
#endif
#define MSG_DEBUG(log, x) \
     do { if (log.level()<=MSG::DEBUG) { log << MSG::DEBUG << x << endreq; } } while (0)
#define MSG_VERBOSE(log, x) \
     do { if (log.level()<=MSG::VERBOSE) { log << MSG::VERBOSE << x << endreq; } } while (0)

#endif
