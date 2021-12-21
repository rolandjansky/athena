/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef HLTSEEDING_IROITHRESHOLDSTOOL_H
#define HLTSEEDING_IROITHRESHOLDSTOOL_H

#include "TrigConfData/L1Menu.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteDecorHandle.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IAlgTool.h"

class IRoIThresholdsTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(IRoIThresholdsTool, 1, 0);
  virtual StatusCode decorateThresholds(const EventContext& eventContext) const = 0;
};

template<typename T_RoI, typename T_RoIContainer, const char* N_RoIContainer, const char* N_ThresholdType>
class RoIThresholdsTool : public extends<AthAlgTool, IRoIThresholdsTool> {
public:
  using ThrVec = std::vector<std::shared_ptr<TrigConf::L1Threshold>>;
  using ThrVecRef = std::reference_wrapper<const ThrVec>;
  using ExtraInfoRef = std::reference_wrapper<const TrigConf::L1ThrExtraInfoBase>;

  RoIThresholdsTool(const std::string& type, const std::string& name, const IInterface* parent)
  : base_class(type, name, parent) {}

  virtual StatusCode initialize() override {
    ATH_CHECK(m_l1MenuKey.initialize());
    ATH_CHECK(m_thresholdPatternsKey.initialize());
    return StatusCode::SUCCESS;
  }

  virtual std::optional<ThrVecRef> getMenuThresholds(const TrigConf::L1Menu& l1Menu) const {
    // Retrieve the thresholds vector from L1 menu configuration
    std::optional<ThrVecRef> menuThresholds;
    try {
      menuThresholds = ThrVecRef(l1Menu.thresholds(N_ThresholdType));
    }
    catch (const std::exception& ex) {
      ATH_MSG_ERROR("Failed to retrieve " << N_ThresholdType << " thresholds from L1 menu. Exception:" << ex.what());
      return {};
    }
    return menuThresholds;
  }

  virtual std::optional<ExtraInfoRef> getMenuThresholdExtraInfo(const TrigConf::L1Menu& l1Menu) const {
    // Retrieve the thresholds vector from L1 menu configuration
    std::optional<ExtraInfoRef> menuExtraInfo;
    try {
      menuExtraInfo = ExtraInfoRef(l1Menu.thrExtraInfo().thrExtraInfo(N_ThresholdType));
    }
    catch (const std::exception& ex) {
      ATH_MSG_ERROR("Failed to retrieve " << N_ThresholdType << " threshold extra info from L1 menu. Exception:" << ex.what());
      return {};
    }
    return menuExtraInfo;
  }

  virtual StatusCode decorateThresholds(const EventContext& eventContext) const override {
    // Retrieve the L1 menu configuration
    SG::ReadHandle<TrigConf::L1Menu> l1Menu = SG::makeHandle(m_l1MenuKey, eventContext);
    ATH_CHECK(l1Menu.isValid());
    std::optional<ThrVecRef> menuThresholds = getMenuThresholds(*l1Menu);
    ATH_CHECK(menuThresholds.has_value());
    std::optional<ExtraInfoRef> menuExtraInfo = getMenuThresholdExtraInfo(*l1Menu);
    ATH_CHECK(menuExtraInfo.has_value());

    // Decorate the RoI objects with threshold patterns
    SG::WriteDecorHandle<T_RoIContainer, uint64_t> thresholdPatterns(m_thresholdPatternsKey, eventContext);
    for (const T_RoI* roi: *thresholdPatterns) {
      try {
        thresholdPatterns(*roi) = getPattern(*roi, menuThresholds.value().get(), menuExtraInfo.value().get());
      }
      catch (const std::exception& ex) {
        ATH_MSG_ERROR("getPattern() for " << N_RoIContainer << " failed with exception: " << ex.what());
        return StatusCode::FAILURE;
      }
    }

    return StatusCode::SUCCESS;
  }

  /// To be implemented by each template instance
  virtual uint64_t getPattern(const T_RoI& roi,
                              const ThrVec& menuThresholds,
                              const TrigConf::L1ThrExtraInfoBase& menuExtraInfo) const = 0;

protected:
  SG::ReadHandleKey<TrigConf::L1Menu> m_l1MenuKey {
    this, "L1TriggerMenu", "DetectorStore+L1TriggerMenu",
    "Name of the L1Menu object to read configuration from"};

  SG::WriteDecorHandleKey<T_RoIContainer> m_thresholdPatternsKey {
    this, "ThresholdPatternsDecorKey", std::string(N_RoIContainer)+".thresholdPatterns",
    "Decoration for the threshold patterns"};
};

#endif // HLTSEEDING_IROITHRESHOLDSTOOL_H
