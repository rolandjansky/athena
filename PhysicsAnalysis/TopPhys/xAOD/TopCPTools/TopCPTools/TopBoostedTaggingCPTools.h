/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TOPCPTOOLS_TOPBOOSTEDTAGGINGCPTOOLS_H_
#define TOPCPTOOLS_TOPBOOSTEDTAGGINGCPTOOLS_H_

// Include what you use
#include <vector>
#include <string>
#include <unordered_map>

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "AsgTools/ToolHandleArray.h"
#include "AsgTools/AnaToolHandle.h"
#include "JetInterface/IJetDecorator.h"
#include "JetCPInterfaces/ICPJetUncertaintiesTool.h"

namespace top {
  class TopConfig;

  class BoostedTaggingCPTools final: public asg::AsgTool {
  public:
    explicit BoostedTaggingCPTools(const std::string& name);
    virtual ~BoostedTaggingCPTools() {}

    StatusCode initialize();
  private:
    std::shared_ptr<top::TopConfig> m_config;

    std::vector<std::string> m_jetCollections;
    std::vector<std::string> m_taggersTypes;
    std::unordered_map<std::string, std::string > m_taggersConfigs;
    std::unordered_map<std::string, std::string > m_taggersCalibAreas;
    std::unordered_map<std::string, std::string > m_taggerSFsConfigs;
    std::unordered_map<std::string, std::string > m_taggerSFsNames;

    void initTaggersMaps();
    void initSFsMaps();

    std::unordered_map<std::string, asg::AnaToolHandle<IJetDecorator> > m_taggers;
    std::unordered_map<std::string, ToolHandle<ICPJetUncertaintiesTool> > m_tagSFuncertTool;
  };
}  // namespace top

#endif  // TOPCPTOOLS_TOPBOOSTEDTAGGINGCPTOOLS_H_
