// -*- C++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


//only show this header in full Athena
#ifndef XAOD_ANALYSIS

#ifndef TrigDecisionTool_DecisionObjectHandleAthena_h
#define TrigDecisionTool_DecisionObjectHandleAthena_h

#include "TrigDecisionTool/Logger.h"
#include "TrigDecisionTool/DecisionObjectHandle.h"
#include "TrigDecisionEvent/TrigDecision.h"

#include "StoreGate/ReadHandleKey.h"

#include <string>

namespace TrigDec {
  class TrigDecision;
}

namespace Trig {
  /**
   * @brief Decision invalidator for Athena (really doing the job)
   **/
  class DecisionObjectHandleAthena : public DecisionObjectHandle<TrigDec::TrigDecision,TrigDec::TrigDecision>
  {
  public:
    DecisionObjectHandleAthena( SG::ReadHandleKey<TrigDec::TrigDecision>* olddeckey );
    virtual void reset (bool hard = false);
    virtual TrigDec::TrigDecision const * getDecision() const override;
    virtual TrigDec::TrigDecision const * getNavigation() const override;
  private:
    SG::ReadHandleKey<TrigDec::TrigDecision>* m_oldDecKey;
  };
}
#endif // TrigDecisionTool_DecisionObjectHandle_h

#endif //ifndef XAOD_ANALYSIS
