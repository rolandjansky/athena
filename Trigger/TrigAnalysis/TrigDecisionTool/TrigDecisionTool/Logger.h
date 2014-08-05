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


#include "GaudiKernel/MsgStream.h"

#define TDTLOG(x)   if (log().level()<=MSG::x) log() << MSG::x


namespace Trig {
  class Logger {
  public:
    Logger(const std::string& name);
    Logger();
    virtual ~Logger() {}
    MsgStream& log() const;
    bool msgLvl(const MSG::Level lvl) const;
    static void updateMsgStream(MsgStream* log);
    virtual void setOutputLevel(int level);
    
  private:
    static MsgStream* m_stream;
    std::string m_name;
  };


  inline bool
  Logger::msgLvl(const MSG::Level lvl) const {
    return log().level() <= lvl;
  }


}

#endif 
