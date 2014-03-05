// MsgStreamMacros.h

#ifndef MsgStreamMacros_H
#define MsgStreamMacros_H

// David Adams
// October 2013
//
// Message stream macros.

#include "AsgTools/AsgToolsConf.h"
#ifdef ASGTOOL_ATHENA
#include "AthenaBaseComps/MsgStreamMacros.h"
#else

#undef ERROR

#include <iostream>
#include "AsgTools/MsgLevel.h"

#define ATH_MSG_LVL(lvl, xmsg) msg() << lvl << xmsg << endmsg

#define ATH_MSG_VERBOSE(xmsg)  ATH_MSG_LVL(MSG::VERBOSE, xmsg)
#define ATH_MSG_DEBUG(xmsg)    ATH_MSG_LVL(MSG::DEBUG, xmsg)
#define ATH_MSG_INFO(xmsg)     ATH_MSG_LVL(MSG::INFO,  xmsg)
#define ATH_MSG_WARNING(xmsg)  ATH_MSG_LVL(MSG::WARNING, xmsg)
#define ATH_MSG_ERROR(xmsg)    ATH_MSG_LVL(MSG::ERROR, xmsg)
#define ATH_MSG_FATAL(xmsg)    ATH_MSG_LVL(MSG::FATAL, xmsg)

#endif
#endif
