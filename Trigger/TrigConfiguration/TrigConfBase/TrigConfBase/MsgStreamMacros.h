///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MsgStreamMacros.h (shamelessly copied from AthMsgStreamMacros.h)
// Header file for useful macros when comes to using MsgStream.
// Note: the macros here are called TRG_MSG_XYZ instead of ATH_MSG_XYZ.
// This is to avoid clashes with the identical macros in AtlasCore.
// 
// Original author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef TRIGCONFBASE_MSGSTREAMMACROS_H 
#define TRIGCONFBASE_MSGSTREAMMACROS_H 1

// FIXME: operator precedence ?!
#define TRG_MSG_LVL_NOCHK(lvl, x)               \
  this->msg(lvl) << x << TrigConf::endmsgtc

#define TRG_MSG_LVL(lvl, x)                     \
  if (this->msgLvl (lvl)) {TRG_MSG_LVL_NOCHK(lvl, x);}

#define TRG_MSG_VERBOSE(x) TRG_MSG_LVL(TrigConf::MSGTC::VERBOSE, x)
#define TRG_MSG_DEBUG(x)   TRG_MSG_LVL(TrigConf::MSGTC::DEBUG, x)
// note that we are using the _NOCHK variant here
#define TRG_MSG_INFO(x)    TRG_MSG_LVL_NOCHK(TrigConf::MSGTC::INFO, x)
#define TRG_MSG_WARNING(x) TRG_MSG_LVL_NOCHK(TrigConf::MSGTC::WARNING, x)
#define TRG_MSG_ERROR(x)   TRG_MSG_LVL_NOCHK(TrigConf::MSGTC::ERROR,  x)
#define TRG_MSG_FATAL(x)   TRG_MSG_LVL_NOCHK(TrigConf::MSGTC::FATAL,  x)
#define TRG_MSG_ALWAYS(x)  TRG_MSG_LVL_NOCHK(TrigConf::MSGTC::ALWAYS, x)

// can be used like so: TRG_MSG(INFO) << "hello" << endmsg;
#define TRG_MSG(lvl) \
  if (this->msgLvl(TrigConf::MSGTC::lvl)) this->msg(TrigConf::MSGTC::lvl)

#endif //> !TRIGCONFBASE_MSGSTREAMMACROS_H

