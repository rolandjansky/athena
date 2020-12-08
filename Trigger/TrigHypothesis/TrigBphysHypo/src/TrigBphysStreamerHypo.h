/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

};

#endif  // TRIG_TrigBphysStreamerHypo_H
