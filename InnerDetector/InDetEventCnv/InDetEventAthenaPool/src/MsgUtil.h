/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETEVENTATHENAPOOL_MSGFIX
#define INDETEVENTATHENAPOOL_MSGFIX

//Defines similar to ATH_MSG_DEBUG and ATH_MSG_VERBOSE which accepts the MsgStream as an argument: 
#ifdef MSG_DEBUG
#undef MSG_DEBUG
#endif
#ifdef MSG_VERBOSE
#undef MSG_VERBOSE
#endif
#define MSG_DEBUG(log, x) \
     do { if (log.level()<=MSG::DEBUG) { log << MSG::DEBUG << x << endmsg; } } while (0)
#define MSG_VERBOSE(log, x) \
     do { if (log.level()<=MSG::VERBOSE) { log << MSG::VERBOSE << x << endmsg; } } while (0)

#endif
