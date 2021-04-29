/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigDecisionTool/Logger.h"

#ifdef XAOD_STANDALONE // AnalysisBase
asg::AsgMessaging* Trig::Logger::staticStream = 0;
#endif
#ifndef XAOD_STANDALONE // AthAnalysis or full Athena
AthMessaging* Trig::Logger::staticStream = 0;
#endif

namespace Trig{
MsgStream& Logger::msg() const {
  if(!Logger::staticStream){
#ifdef XAOD_STANDALONE // AnalysisBase
    Logger::staticStream = new asg::AsgMessaging("TrigDecisionTool");
#endif
#ifndef XAOD_STANDALONE // AthAnalysis or full Athena
Logger::staticStream = new AthMessaging(Athena::getMessageSvc(),"TrigDecisionTool");
#endif
  }
  return Logger::staticStream->msg();
}
}
