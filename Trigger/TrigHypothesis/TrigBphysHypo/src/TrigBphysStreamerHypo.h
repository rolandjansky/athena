/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TrigBphysStreamerHypo_H
#define TRIG_TrigBphysStreamerHypo_H

#include <string>

#include "DecisionHandling/HypoBase.h"

#include "TrigBphysStreamerHypoTool.h"

/**
 * @class TrigBphysStreamerHypo
 * @brief Implements a streamer (no selection) for the HLT framework
 */
class TrigBphysStreamerHypo : public ::HypoBase {
 public:
  TrigBphysStreamerHypo(const std::string& name, ISvcLocator* pSvcLocator);
  TrigBphysStreamerHypo() = delete;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext&) const override;

 private:
  ToolHandleArray<TrigBphysStreamerHypoTool> m_hypoTools {this, "HypoTools", {}, "Tools to perform selection"};
  Gaudi::Property<std::string> m_triggerLevel {this, "triggerLevel", "EF", "use xAOD::L2StandAloneMuon at L2 level and xAOD::Muon at EF level as a trigger feature object"};

  Gaudi::Property<std::vector<std::string>> m_triggerList {this, "triggerList", {}, "The streamer will copy ID from the previous Decision if it matches with some trigger from the list; all IDs will be copied if no trigger is specified"};
  TrigCompositeUtils::DecisionIDContainer m_triggerIDs;
};

#endif  // TRIG_TrigBphysStreamerHypo_H
