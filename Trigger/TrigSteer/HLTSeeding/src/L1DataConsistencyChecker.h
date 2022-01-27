/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef HLTSEEDING_L1DATACONSISTENCYCHECKER_H
#define HLTSEEDING_L1DATACONSISTENCYCHECKER_H

// Trigger includes
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigConfData/HLTMenu.h"
#include "TrigConfData/L1Menu.h"
#include "xAODTrigger/TrigCompositeContainer.h"

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "StoreGate/ReadHandleKey.h"

/**
 * @class L1DataConsistencyChecker
 * @brief Checks the consistency between CTP active items and L1 RoIs (TOBs) received from RoIB/readout
 */
class L1DataConsistencyChecker : public AthAlgTool {
public:
  L1DataConsistencyChecker(const std::string& type, const std::string& name, const IInterface* parent)
  : AthAlgTool(type, name, parent) {}

  /// @name IStateful transition implementations
  // @{
  virtual StatusCode initialize() override;
  virtual StatusCode start() override;
  // @}

  /// Perform the check and fill the corresponding histograms
  StatusCode consistencyCheck(const HLT::IDVec& l1SeededChains, const EventContext& ctx) const;

private:
  SG::ReadHandleKey<TrigConf::HLTMenu> m_hltMenuKey {
    this, "HLTMenu", "DetectorStore+HLTTriggerMenu", "HLT Menu"};
  SG::ReadHandleKey<TrigConf::L1Menu>  m_l1MenuKey {
    this, "L1Menu", "DetectorStore+L1TriggerMenu", "L1 Menu"};

  ToolHandle<GenericMonitoringTool> m_monTool {
    this, "MonTool", "", "Monitoring tool"};

  Gaudi::Property<std::map<std::string,std::string>> m_thresholdToDecisionMap {
    this, "ThresholdToDecisionMap", {}, "Map between L1 threshold type and the corresponding DecisionContainer name"};

  Gaudi::Property<std::vector<std::string>> m_excludedThresholdTypes {
    this, "ExcludedThresholdTypes", {"internal", "TOPO", "R2TOPO", "TE", "XE", "XS"},
    "L1 threshold types which are not subject to the consistency check "
    "(because either there are no corresponding TOBs or HLT doesn't use them)"};

  using MultiplicityInfo = std::unordered_map<TrigCompositeUtils::DecisionID, unsigned int>; //< {L1ThresholdHash, multiplicity}
  std::unordered_map<TrigCompositeUtils::DecisionID, MultiplicityInfo> m_thresholdMap; //< {HLTChainHash, MultiplicityInfo}
  std::unordered_map<TrigCompositeUtils::DecisionID, std::string> m_thresholdNames; //< {L1ThresholdHash, L1ThresholdName}
  std::set<TrigCompositeUtils::DecisionID> m_monitoredThresholds;
};

#endif // HLTSEEDING_L1DATACONSISTENCYCHECKER_H
