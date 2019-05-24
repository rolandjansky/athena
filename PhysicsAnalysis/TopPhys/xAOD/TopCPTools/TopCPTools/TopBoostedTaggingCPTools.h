/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TOPCPTOOLS_TOPBOOSTEDTAGGINGCPTOOLS_H_
#define TOPCPTOOLS_TOPBOOSTEDTAGGINGCPTOOLS_H_

// Include what you use
#include <vector>
#include <string>
#include <map>

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "AsgTools/ToolHandleArray.h"
#include "AsgTools/AnaToolHandle.h"
#include "JetAnalysisInterfaces/IJetSelectorTool.h"

namespace top {

class TopConfig;

class BoostedTaggingCPTools final : public asg::AsgTool {
 public:
  explicit BoostedTaggingCPTools(const std::string& name);
  virtual ~BoostedTaggingCPTools() {}

  StatusCode initialize();

 private:
  std::shared_ptr<top::TopConfig> m_config;
  int m_release_series = 24;  // Default to 2.4
  
  std::unordered_map<std::string,asg::AnaToolHandle<IJetSelectorTool> > m_taggers;
};
}  // namespace top

#endif  // TOPCPTOOLS_TOPBOOSTEDTAGGINGCPTOOLS_H_
