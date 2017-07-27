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

// Boosted tagging includes
#include "BoostedJetTaggers/SmoothedTopTagger.h"
#include "BoostedJetTaggers/SmoothedWZTagger.h"

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

  //bool to decide if add the tagging informations as decoration of the jet.
  std::string m_topTag_configFile_80 = "";
  std::string m_topTag_configFile_50 = "";
  std::string m_WTag_configFile_80
      = "SmoothedWZTaggers/SmoothedContainedWTagger_AntiKt10LCTopoTrimmed_FixedSignalEfficiency80_MC15c_20161215.dat";
  std::string m_WTag_configFile_50
      = "SmoothedWZTaggers/SmoothedContainedWTagger_AntiKt10LCTopoTrimmed_FixedSignalEfficiency50_MC15c_20161215.dat";
  std::string m_ZTag_configFile_80
      = "SmoothedWZTaggers/SmoothedContainedZTagger_AntiKt10LCTopoTrimmed_FixedSignalEfficiency80_MC15c_20161215.dat";
  std::string m_ZTag_configFile_50
      = "SmoothedWZTaggers/SmoothedContainedZTagger_AntiKt10LCTopoTrimmed_FixedSignalEfficiency50_MC15c_20161215.dat";

  std::unique_ptr<SmoothedTopTagger> m_topTag50 = nullptr; //tight
  std::unique_ptr<SmoothedTopTagger> m_topTag80 = nullptr; //loose

  std::unique_ptr<SmoothedWZTagger> m_WTag50 = nullptr;
  std::unique_ptr<SmoothedWZTagger> m_WTag80 = nullptr;
  std::unique_ptr<SmoothedWZTagger> m_ZTag50 = nullptr;
  std::unique_ptr<SmoothedWZTagger> m_ZTag80 = nullptr;


};
}  // namespace top

#endif  // TOPCPTOOLS_TOPBOOSTEDTAGGINGCPTOOLS_H_
