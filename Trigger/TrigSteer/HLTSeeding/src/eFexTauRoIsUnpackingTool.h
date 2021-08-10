/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef HLTSEEDING_EFEXTAUROISUNPACKINGTOOL_H
#define HLTSEEDING_EFEXTAUROISUNPACKINGTOOL_H 1

#include "RoIsUnpackingToolBase.h"
#include "TrigConfData/L1Menu.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "xAODTrigger/eFexTauRoIContainer.h"
#include "StoreGate/ReadDecorHandleKey.h"

class eFexTauRoIsUnpackingTool : public RoIsUnpackingToolBase {
public:
  eFexTauRoIsUnpackingTool(const std::string& type,
                           const std::string& name,
                           const IInterface* parent);

  using RoIsUnpackingToolBase::unpack;
  virtual StatusCode unpack(const EventContext& ctx,
                            const xAOD::TrigComposite& l1TriggerResult,
                            const HLT::IDSet& activeChains) const override;

  virtual StatusCode initialize() override;
  virtual StatusCode start() override;

private:
  SG::ReadHandleKey<TrigConf::L1Menu> m_l1MenuKey{
    this, "L1TriggerMenu", "DetectorStore+L1TriggerMenu",
    "Name of the L1Menu object to read configuration from"};

  SG::WriteHandleKey<TrigRoiDescriptorCollection> m_trigRoIsKey{
    this, "OutputTrigRoIs", "HLT_eTAURoIs",
    "Name of the TrigRoiDescriptor collection produced by the unpacker"};

  Gaudi::Property<std::string> m_eFexTauRoILinkName {
    this, "eFexTauRoILinkName", "L1_eTauRoI",
    "Name of the link to read from L1TriggerResult for eFexTauRoI container"};

  SG::ReadDecorHandleKey<xAOD::eFexTauRoIContainer> m_thresholdPatternsKey {
    this, "eFexTauRoIThresholdPatternsKey", "L1_eTauRoI.thresholdPatterns",
    "Name of the eFexTauRoI container decoration for the threshold patterns"};

  Gaudi::Property<float> m_roiWidth{
    this, "RoIWidth", 0.1, "Size of RoI in eta/phi"};
};

#endif //> !HLTSEEDING_EFEXTAUROISUNPACKINGTOOL_H
