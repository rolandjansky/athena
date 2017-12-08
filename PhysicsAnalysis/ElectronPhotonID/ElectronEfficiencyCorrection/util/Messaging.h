/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ELECTRONEFFICIENCYCORRECTIONMESSAGING__
#define __ELECTRONEFFICIENCYCORRECTIONMESSAGING__
#include "AsgTools/AsgMessaging.h"

static  asg::AsgMessaging dummymsg("");

#define DUMMY_MSG( lvl, ARG ) {dummymsg.msg(lvl)<<ARG<<endmsg ;}
#define MSG_DEBUG( ARG ) {DUMMY_MSG(MSG::DEBUG, ARG);}
#define MSG_INFO( ARG ) {DUMMY_MSG(MSG::INFO, ARG);}
#define MSG_WARNING( ARG ) {DUMMY_MSG(MSG::WARNING, ARG);}
#define MSG_ERROR( ARG ) {DUMMY_MSG(MSG::ERROR, ARG);}
#define MSG_FATAL( ARG ) {DUMMY_MSG(MSG::FATAL, ARG);}
#define MSG_ABORT( ARG ) {DUMMY_MSG(MSG::FATAL, ARG); std::abort();} 

namespace MSGHELPERS{
  const asg::AsgMessaging& getMsgStream();
}
const asg::AsgMessaging& MSGHELPERS::getMsgStream(){return dummymsg;}

#endif
