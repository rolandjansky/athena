/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TrigDecisionTool_Logger_h
#define TrigDecisionTool_Logger_h

/**********************************************************************************
 * @Project:
 * @Package: TrigDecision
 * @Class  : TrigDecision
 *
 * @brief common logger for various non Athena components 
 *
 * @author Tomasz Bold     <Tomasz.Bold@cern.ch>    - UC Irvine - AGH-UST Krakow
 *
 ***********************************************************************************/
#include <string>
#include "AsgTools/AsgToolsConf.h"

#ifdef ASGTOOL_STANDALONE
#include "AsgTools/AsgMessaging.h"
#endif
#ifdef ASGTOOL_ATHENA
#include "AthenaKernel/getMessageSvc.h"
#include "AthenaBaseComps/AthMessaging.h"
#endif

namespace Trig{
  class Logger {
  public:
    MsgStream& msg() const;
    MsgStream& msg(const MSG::Level lvl) const {return msg() << lvl;}
    bool msgLvl(const MSG::Level lvl) const {return Logger::staticStream->msgLvl(lvl);}
#ifdef ASGTOOL_STANDALONE
    void setMessaging(asg::AsgMessaging* messaging){staticStream = messaging;}
    static asg::AsgMessaging* staticStream;
#endif
#ifdef ASGTOOL_ATHENA
    void setMessaging(AthMessaging* messaging){staticStream = messaging;}
    static AthMessaging* staticStream;
#endif
  };
}

#endif
