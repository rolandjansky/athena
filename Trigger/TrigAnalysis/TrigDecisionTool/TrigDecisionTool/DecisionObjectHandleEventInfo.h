// -*- C++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


//only show this header in full Athena
#ifndef XAOD_ANALYSIS

#ifndef TrigDecisionTool_DecisionObjectHandleEventInfo_h
#define TrigDecisionTool_DecisionObjectHandleEventInfo_h

#include <string>

#include "TrigDecisionTool/DecisionObjectHandle.h"
#include "EventInfo/EventInfo.h"

#include "StoreGate/ReadHandleKey.h"

class TriggerInfo;
class EventInfo;

namespace Trig {
  /**
   * @brief Decision invalidator for EventInfo (really doing the job)
   **/
  class DecisionObjectHandleEventInfo : public DecisionObjectHandle<TriggerInfo,void>
  {
  public:
    DecisionObjectHandleEventInfo( SG::ReadHandleKey<EventInfo>* oldEventInfoKey );
    virtual void reset (bool hard = false);
    virtual TriggerInfo const * getDecision() const override;
    virtual void const * getNavigation() const override; 
  private:
    SG::ReadHandleKey<EventInfo>* m_oldEventInfoKey;
  };
}

#endif // TrigDecisionTool_DecisionObjectHandleEventInfo_h

#endif //ifndef XAOD_ANALYSIS
