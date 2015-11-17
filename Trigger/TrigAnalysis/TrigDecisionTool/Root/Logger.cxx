/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigDecisionTool/Logger.h"

#ifdef ASGTOOL_STANDALONE
asg::AsgMessaging* Trig::Logger::staticStream = 0;
#endif
#ifdef ASGTOOL_ATHENA
AthMessaging* Trig::Logger::staticStream = 0;
#endif

namespace Trig{
MsgStream& Logger::msg() const {
  if(!Logger::staticStream){
#ifdef ASGTOOL_STANDALONE
    Logger::staticStream = new asg::AsgMessaging("TrigDecisionTool");
#endif
#ifdef ASGTOOL_ATHENA
Logger::staticStream = new AthMessaging(Athena::getMessageSvc(),"TrigDecisionTool");
#endif
  }
  return Logger::staticStream->msg();
}
}
