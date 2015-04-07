// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigDecisionTool_DecisionObjectHandleAthena_h
#define TrigDecisionTool_DecisionObjectHandleAthena_h

#include <string>
#include "StoreGate/DataHandle.h"
#include "TrigDecisionTool/Logger.h"
#include "TrigDecisionTool/DecisionObjectHandle.h"

#include "TrigDecisionEvent/TrigDecision.h"

#include "AsgTools/AsgMessaging.h"

class StoreGateSvc;

namespace TrigDec {
  class TrigDecision;
}


namespace Trig {
  /**
   * @brief Decision invalidator for Athena (really doing the job)
   **/
  class DecisionObjectHandleAthena : public DecisionObjectHandle<TrigDec::TrigDecision,TrigDec::TrigDecision>, 
				     public DataHandle<TrigDec::TrigDecision>,
				     public asg::AsgMessaging
  {
  public:
    DecisionObjectHandleAthena( StoreGateSvc* sg, const std::string& key );
    virtual void reset();
    virtual TrigDec::TrigDecision const * getDecision() const;
    virtual TrigDec::TrigDecision const * getNavigation() const;
  private:
    StoreGateSvc* m_sg;
    const std::string m_key;
    mutable TrigDec::TrigDecision const * m_object;
  };
}




#endif // TrigDecisionTool_DecisionObjectHandle_h
