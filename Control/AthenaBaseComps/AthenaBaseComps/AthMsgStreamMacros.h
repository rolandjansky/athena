///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthMsgStreamMacros.h 
// Header file for useful macros when comes to using MsgStream
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENABASECOMPS_ATHMSGSTREAMMACROS_H 
#define ATHENABASECOMPS_ATHMSGSTREAMMACROS_H 1

// CxxUtils
#include "CxxUtils/AthUnlikelyMacros.h"

// FIXME: operator precedence ?!
#define ATH_MSG_LVL_NOCHK(lvl, x)               \
  this->msg(lvl) << x << endmsg

#define ATH_MSG_LVL(lvl, x)                     \
  do {                                          \
    if (ATH_UNLIKELY(this->msgLvl (lvl))) {     \
      ATH_MSG_LVL_NOCHK(lvl, x);                \
    }                                           \
  } while (0)

#define ATH_MSG_VERBOSE(x) ATH_MSG_LVL(MSG::VERBOSE, x)
#define ATH_MSG_DEBUG(x)   ATH_MSG_LVL(MSG::DEBUG, x)
// note that we are using the _NOCHK variant here
#define ATH_MSG_INFO(x)    ATH_MSG_LVL_NOCHK(MSG::INFO, x)
#define ATH_MSG_WARNING(x) ATH_MSG_LVL_NOCHK(MSG::WARNING, x)
#define ATH_MSG_ERROR(x)   ATH_MSG_LVL_NOCHK(MSG::ERROR,  x)
#define ATH_MSG_FATAL(x)   ATH_MSG_LVL_NOCHK(MSG::FATAL,  x)
#define ATH_MSG_ALWAYS(x)  ATH_MSG_LVL_NOCHK(MSG::ALWAYS, x)

// can be used like so: ATH_MSG(INFO) << "hello" << endmsg;
#define ATH_MSG(lvl) \
  if (this->msgLvl(MSG::lvl)) this->msg(MSG::lvl)

#endif //> !ATHENABASECOMPS_ATHMSGSTREAMMACROS_H

