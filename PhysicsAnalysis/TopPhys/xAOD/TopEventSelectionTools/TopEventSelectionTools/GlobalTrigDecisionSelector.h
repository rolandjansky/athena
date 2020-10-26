/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#ifndef GLOBALTRIGDECISIONSELECTOR_H_
#define GLOBALTRIGDECISIONSELECTOR_H_

#include "TopEventSelectionTools/EventSelectorBase.h"

#include <memory>

#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

#include "TriggerAnalysisInterfaces/ITrigGlobalEfficiencyCorrectionTool.h"


namespace top {
  class Event;
  class TopConfig;

  class GlobalTrigDecisionSelector: public EventSelectorBase {
  public:
    GlobalTrigDecisionSelector();

    bool apply(top::Event const& event) const override;
    std::string name() const override;
  private:
    mutable ToolHandle<ITrigGlobalEfficiencyCorrectionTool> m_globalTriggerSF;
    mutable ToolHandle<ITrigGlobalEfficiencyCorrectionTool> m_globalTriggerSFLoose;
  };
}

#endif
