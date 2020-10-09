// -*- C++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


//only show this header in full Athena
#ifndef XAOD_ANALYSIS

#ifndef TrigDecisionTool_DecisionObjectHandleEventInfo_h
#define TrigDecisionTool_DecisionObjectHandleEventInfo_h

#include <string>
#include "StoreGate/DataHandle.h"

#include "TrigDecisionTool/EventPtrDef.h"
#include "TrigDecisionTool/Logger.h"
#include "TrigDecisionTool/DecisionObjectHandle.h"
#include "EventInfo/EventInfo.h"
#include "AsgMessaging/AsgMessaging.h"

#include "StoreGate/ReadHandleKey.h"

class TriggerInfo;
class EventInfo;
class StoreGateSvc;

namespace Trig {
  /**
   * @brief Decision invalidator for EventInfo (really doing the job)
   **/
  class DecisionObjectHandleEventInfo : public DecisionObjectHandle<TriggerInfo,void>, 
					public DataHandle<EventInfo>
  {
  public:
    DecisionObjectHandleEventInfo( SG::ReadHandleKey<EventInfo>* oldEventInfoKey );
    virtual void reset (bool hard = false) override;
    virtual TriggerInfo const * getDecision() const override;
    virtual void const * getNavigation() const override; 
  private:
    SG::ReadHandleKey<EventInfo>* m_oldEventInfoKey;
    mutable EventInfo const * m_object;
  };
}

#endif // TrigDecisionTool_DecisionObjectHandleEventInfo_h

#endif //ifndef XAOD_ANALYSIS
