/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef HLTSEEDING_ROISUNPACKINGTOOLBASE_H
#define HLTSEEDING_ROISUNPACKINGTOOLBASE_H

#include "IRoIsUnpackingTool.h"

#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigConfData/L1Menu.h"
#include "TrigConfData/HLTMenu.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "xAODTrigger/TrigComposite.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

namespace ROIB {
  class RoIBResult;
}

/**
 * @class RoIsUnpackingToolBase
 * @brief Base class for RoI unpackers
 *
 * Provides common features needed by all unpackers. By itself it does not do anything useful.
 */
class RoIsUnpackingToolBase : public extends<AthAlgTool, IRoIsUnpackingTool> {
public:
  RoIsUnpackingToolBase(const std::string& type,
                        const std::string& name,
                        const IInterface* parent);

  virtual StatusCode initialize() override;

  virtual StatusCode unpack(const EventContext& /*ctx*/,
                            const ROIB::RoIBResult& /*roib*/,
                            const HLT::IDSet& /*activeChains*/) const override { return StatusCode::SUCCESS; }

  virtual StatusCode unpack(const EventContext& /*ctx*/,
                            const xAOD::TrigComposite& /*l1TriggerResult*/,
                            const HLT::IDSet& /*activeChains*/) const override { return StatusCode::SUCCESS; }

protected:
  /// @name Data dependencies
  /// @{
  SG::WriteHandleKey<TrigCompositeUtils::DecisionContainer> m_decisionsKey{
    this, "Decisions", "RoIDecisions", "Decisions for each RoI"};

  SG::WriteHandleKey<TrigCompositeUtils::DecisionContainer> m_decisionsKeyProbe{
    this, "DecisionsProbe", "", "Optional secondary set of Decisions for each RoI for probe a.k.a. delayed a.k.a. rerun chains"};

  SG::WriteHandleKey<TrigRoiDescriptorCollection> m_trigRoIsKey{
    this, "OutputTrigRoIs", "", "Name of the RoIs collection produced by the unpacker"};

  SG::ReadHandleKey<TrigConf::L1Menu> m_l1MenuKey{
    this, "L1TriggerMenu", "DetectorStore+L1TriggerMenu", "Name of the L1Menu object to read configuration from"};

  SG::ReadHandleKey<TrigConf::HLTMenu> m_HLTMenuKey{
    this, "HLTTriggerMenu", "DetectorStore+HLTTriggerMenu", "Name of the HLTMenu object to read configuration from"};
  /// @}

  ToolHandle<GenericMonitoringTool> m_monTool{
    this, "MonTool", "", "Monitoring tool"};

  std::map<HLT::Identifier, HLT::IDVec> m_thresholdToChainMapping;
  std::map<HLT::Identifier, HLT::Identifier> m_legToChainMapping;

  using ThrVec = std::vector<std::shared_ptr<TrigConf::L1Threshold>>;
  using ThrVecRef = std::reference_wrapper<const ThrVec>;
  /**
   * Retrieve a vector of thresholds with type @c thrType from L1Menu
   *
   * @param[in] l1Menu The L1Menu object
   * @param[in] thrType The threshold type, e.g. EM, MU, eTAU
   * @param[out] thrVec The output will be passed into this parameter
   * @return FAILURE on exception reading the menu (e.g. thrType not in menu), SUCCESS otherwise
   */
  StatusCode getL1Thresholds(const TrigConf::L1Menu& l1Menu, const std::string& thrType, std::optional<ThrVecRef>& thrVec) const;

  /**
   * Concatenate the probe identifier string with the threshold name string
   *
   * Creates an alternate threshold name, used by lower pT probe legs of tag+probe chains.
   **/
  static std::string getProbeThresholdName(const std::string& thresholdName);

  /**
   * Fills mapping from L1 threshold -> to HLT chain
   *
   * @arg filter Function that, using the threshold name, defines if this decoder instance should take care
   * of this threshold
   **/
  StatusCode decodeMapping( std::function< bool(const std::string&)>&& filter );

  void addChainsToDecision( HLT::Identifier thresholdId,
                            TrigCompositeUtils::Decision* d,
                            const HLT::IDSet& activeChains ) const;
};

#endif
