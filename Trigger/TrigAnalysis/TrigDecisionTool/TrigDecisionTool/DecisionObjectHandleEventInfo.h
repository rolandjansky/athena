// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigDecisionTool_DecisionObjectHandleEventInfo_h
#define TrigDecisionTool_DecisionObjectHandleEventInfo_h

#include <string>
#include "AsgTools/AsgToolsConf.h"
#ifdef ASGTOOL_ATHENA
#include "StoreGate/DataHandle.h"
#else
template<typename T> struct DataHandle{};
#endif


#include "TrigDecisionTool/Logger.h"
#include "TrigDecisionTool/DecisionObjectHandle.h"



class TriggerInfo;
class EventInfo;
#if defined(ASGTOOL_ATHENA) and !defined(XAOD_ANALYSIS)
#include "EventInfo/EventInfo.h"
#endif

#include "AsgTools/AsgMessaging.h"

class StoreGateSvc;

namespace Trig {
  /**
   * @brief Decision invalidator for EventInfo (really doing the job)
   **/
  class DecisionObjectHandleEventInfo : public DecisionObjectHandle<TriggerInfo,void>, 
					public DataHandle<EventInfo>,
					public asg::AsgMessaging
  {
  public:
    DecisionObjectHandleEventInfo( StoreGateSvc* sg, const std::string& key );
    virtual void reset();
    virtual TriggerInfo const * getDecision() const;
    virtual void const * getNavigation() const; 
  private:
    StoreGateSvc* m_sg;
    const std::string m_key;
    mutable EventInfo const * m_object;
  };
}

#endif // TrigDecisionTool_DecisionObjectHandleEventInfo_h
