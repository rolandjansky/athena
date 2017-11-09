/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TOPCPTOOLS_TOPJETMETCPTOOLS_H_
#define TOPCPTOOLS_TOPJETMETCPTOOLS_H_

// Include what you use
#include <vector>
#include <string>

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "AsgTools/ToolHandleArray.h"
#include "AsgTools/AnaToolHandle.h"

// Jet include(s):
#include "JetCalibTools/IJetCalibrationTool.h"
#include "JetCPInterfaces/ICPJetUncertaintiesTool.h"
#include "JetInterface/IJetUpdateJvt.h"
#include "JetInterface/IJetSelector.h"
#include "JetInterface/IJetModifier.h"
#include "JetResolution/IJERTool.h"
#include "JetResolution/IJERSmearingTool.h"
#include "JetJvtEfficiency/IJetJvtEfficiency.h"
#include "JetSelectorTools/IEventCleaningTool.h"

// MET include(s):
#include "METInterface/IMETMaker.h"
#include "METInterface/IMETSystematicsTool.h"

namespace top {

class TopConfig;

class JetMETCPTools final : public asg::AsgTool {
 public:
  explicit JetMETCPTools(const std::string& name);
  virtual ~JetMETCPTools() {}

  StatusCode initialize();

 private:
  std::shared_ptr<top::TopConfig> m_config;

  int m_release_series = 24;  // Default to 2.4

  std::string m_jetJVT_ConfigFile;

  std::string m_jetAntiKt4_Data_ConfigFile;
  std::string m_jetAntiKt4_Data_CalibSequence;

  std::string m_jetAntiKt4_MCFS_ConfigFile;
  std::string m_jetAntiKt4_MCFS_CalibSequence;

  std::string m_jetAntiKt4_MCAFII_ConfigFile;
  std::string m_jetAntiKt4_MCAFII_CalibSequence;

  std::string m_jetAntiKt4_PFlow_MCFS_ConfigFile;
  std::string m_jetAntiKt4_PFlow_MCFS_CalibSequence;

  ToolHandle<IJetCalibrationTool> m_jetCalibrationTool;
  ToolHandle<IJetCalibrationTool> m_jetCalibrationToolLargeR;

  ToolHandle<ICPJetUncertaintiesTool> m_jetUncertaintiesToolLargeR_strong;
  ToolHandle<ICPJetUncertaintiesTool> m_jetUncertaintiesToolLargeR_medium;
  ToolHandle<ICPJetUncertaintiesTool> m_jetUncertaintiesToolLargeR_weak;

  ToolHandle<ICPJetUncertaintiesTool> m_jetUncertaintiesTool;
  ToolHandle<ICPJetUncertaintiesTool> m_jetUncertaintiesToolFrozenJMS;
  ToolHandle<ICPJetUncertaintiesTool> m_jetUncertaintiesToolReducedNPScenario1;
  ToolHandle<ICPJetUncertaintiesTool> m_jetUncertaintiesToolReducedNPScenario2;
  ToolHandle<ICPJetUncertaintiesTool> m_jetUncertaintiesToolReducedNPScenario3;
  ToolHandle<ICPJetUncertaintiesTool> m_jetUncertaintiesToolReducedNPScenario4;

  ToolHandle<IJetSelector> m_jetCleaningToolLooseBad;
  ToolHandle<IJetSelector> m_jetCleaningToolTightBad;

  // Implement event object cleaning tool 
  ToolHandle<ECUtils::IEventCleaningTool> m_jetEventCleaningToolLooseBad;
  ToolHandle<ECUtils::IEventCleaningTool> m_jetEventCleaningToolTightBad;

  ToolHandle<IJERTool> m_jetJERTool;
  ToolHandle<IJERSmearingTool> m_jetJERSmearingTool;
  ToolHandle<IJetUpdateJvt> m_jetUpdateJvtTool;
  ToolHandle<IJetModifier> m_fjvtTool;

  ToolHandle<CP::IJetJvtEfficiency> m_jetJvtTool;

  ToolHandle<IMETMaker> m_met_maker;
  ToolHandle<IMETSystematicsTool> m_met_systematics;



  StatusCode setupJetsCalibration();
  StatusCode setupLargeRJetsCalibration();
  StatusCode setupJetsScaleFactors();
  StatusCode setupMET();

  ICPJetUncertaintiesTool*
    setupJetUncertaintiesTool(const std::string& name,
                              const std::string& jet_def,
                              const std::string& mc_type,
                              const std::string& config_file,
                              std::vector<std::string>* variables,
                              const std::string& analysis_file = "");

  IJetSelector* setupJetCleaningTool(const std::string& WP);
  ECUtils::IEventCleaningTool* setupJetEventCleaningTool(const std::string& WP, ToolHandle<IJetSelector> JetCleaningToolHandle);

};
}  // namespace top

#endif  // TOPCPTOOLS_TOPJETMETCPTOOLS_H_
