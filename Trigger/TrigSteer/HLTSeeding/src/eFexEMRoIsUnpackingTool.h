/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef HLTSEEDING_EFEXEMROISUNPACKINGTOOL_H
#define HLTSEEDING_EFEXEMROISUNPACKINGTOOL_H 1

#include "RoIsUnpackingToolBase.h"
#include "xAODTrigger/eFexEMRoIContainer.h"
#include "StoreGate/ReadDecorHandleKey.h"

class eFexEMRoIsUnpackingTool : public RoIsUnpackingToolBase {
public:
  eFexEMRoIsUnpackingTool(const std::string& type,
                          const std::string& name,
                          const IInterface* parent);

  using RoIsUnpackingToolBase::unpack;
  virtual StatusCode unpack(const EventContext& ctx,
                            const xAOD::TrigComposite& l1TriggerResult,
                            const HLT::IDSet& activeChains) const override;

  virtual StatusCode initialize() override;
  virtual StatusCode start() override;

private:
  Gaudi::Property<std::string> m_eFexEMRoILinkName {
    this, "eFexEMRoILinkName", "L1_eEMRoI",
    "Name of the link to read from L1TriggerResult for eFexEMRoI container"};

  SG::ReadDecorHandleKey<xAOD::eFexEMRoIContainer> m_thresholdPatternsKey {
    this, "eFexEMRoIThresholdPatternsKey", "L1_eEMRoI.thresholdPatterns",
    "Name of the eFexEMRoI container decoration for the threshold patterns"};

  Gaudi::Property<float> m_roiWidth{
    this, "RoIWidth", 0.1, "Size of RoI in eta/phi"};
};

#endif //> !HLTSEEDING_EFEXEMROISUNPACKINGTOOL_H
