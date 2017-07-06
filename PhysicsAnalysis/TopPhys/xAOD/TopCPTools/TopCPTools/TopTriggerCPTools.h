/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TOPCPTOOLS_TOPTRIGGERCPTOOLS_H_
#define TOPCPTOOLS_TOPTRIGGERCPTOOLS_H_

// Include what you use
#include <vector>
#include <string>

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "AsgTools/ToolHandleArray.h"
#include "AsgTools/AnaToolHandle.h"

// Trigger include(s):
#include "TrigConfInterfaces/ITrigConfigTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TriggerMatchingTool/IMatchingTool.h"
#include "TrigTauMatching/ITrigTauMatching.h"

namespace top {

class TopConfig;

class TriggerCPTools final : public asg::AsgTool {
 public:
  explicit TriggerCPTools(const std::string& name);
  virtual ~TriggerCPTools() {}

  StatusCode initialize();

 private:
  std::shared_ptr<top::TopConfig> m_config;

  int m_release_series = 24;  // Default to 2.4

  ToolHandle<TrigConf::ITrigConfigTool> m_trigConfTool;
  ToolHandle<Trig::TrigDecisionTool> m_trigDecisionTool;
  ToolHandle<Trig::IMatchingTool> m_trigMatchTool;
  ToolHandle<Trig::ITrigTauMatchingTool> m_trigMatchTauTool;

};
}  // namespace top

#endif  // TOPCPTOOLS_TOPTRIGGERCPTOOLS_H_
