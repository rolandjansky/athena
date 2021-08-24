/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef HLTSEEDING_JFEXTAUROITHRESHOLDSTOOL_H
#define HLTSEEDING_JFEXTAUROITHRESHOLDSTOOL_H

#include "HLTSeeding/IRoIThresholdsTool.h"
#include "xAODTrigger/jFexTauRoI.h"
#include "xAODTrigger/jFexTauRoIContainer.h"

namespace jFexTauRoIThresholdsToolParams {
  extern const char ContainerName[];
  extern const char ThresholdType[];
  using BaseClass = RoIThresholdsTool<xAOD::jFexTauRoI, xAOD::jFexTauRoIContainer, ContainerName, ThresholdType>;
}

class jFexTauRoIThresholdsTool : public jFexTauRoIThresholdsToolParams::BaseClass {
public:
  jFexTauRoIThresholdsTool(const std::string& type, const std::string& name, const IInterface* parent)
  : jFexTauRoIThresholdsToolParams::BaseClass(type, name, parent) {}

  virtual uint64_t getPattern(const xAOD::jFexTauRoI& roi,
                              const ThrVec& menuThresholds,
                              const TrigConf::L1ThrExtraInfoBase& menuExtraInfo) const override;
};

#endif // HLTSEEDING_JFEXTAUROITHRESHOLDSTOOL_H
