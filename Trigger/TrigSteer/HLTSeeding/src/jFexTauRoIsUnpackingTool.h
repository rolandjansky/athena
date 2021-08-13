/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef HLTSEEDING_JFEXTAUROISUNPACKINGTOOL_H
#define HLTSEEDING_JFEXTAUROISUNPACKINGTOOL_H 1

#include "RoIsUnpackingToolBase.h"
#include "xAODTrigger/jFexTauRoIContainer.h"
#include "StoreGate/ReadDecorHandleKey.h"

class jFexTauRoIsUnpackingTool : public RoIsUnpackingToolBase {
public:
  jFexTauRoIsUnpackingTool(const std::string& type,
                           const std::string& name,
                           const IInterface* parent);

  using RoIsUnpackingToolBase::unpack;
  virtual StatusCode unpack(const EventContext& ctx,
                            const xAOD::TrigComposite& l1TriggerResult,
                            const HLT::IDSet& activeChains) const override;

  virtual StatusCode initialize() override;
  virtual StatusCode start() override;

private:
  Gaudi::Property<std::string> m_jFexTauRoILinkName {
    this, "jFexTauRoILinkName", "L1_jFexTauRoI",
    "Name of the link to read from L1TriggerResult for jFexTauRoI container"};

  SG::ReadDecorHandleKey<xAOD::jFexTauRoIContainer> m_thresholdPatternsKey {
    this, "jFexTauRoIThresholdPatternsKey", "L1_jFexTauRoI.thresholdPatterns",
    "Name of the jFexTauRoI container decoration for the threshold patterns"};

  Gaudi::Property<float> m_roiWidth{
    this, "RoIWidth", 0.1, "Size of RoI in eta/phi"};
};

#endif //> !HLTSEEDING_JFEXTAUROISUNPACKINGTOOL_H
