/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef HLTSEEDING_EFEXTAUROITHRESHOLDSTOOL_H
#define HLTSEEDING_EFEXTAUROITHRESHOLDSTOOL_H

#include "HLTSeeding/IRoIThresholdsTool.h"
#include "xAODTrigger/eFexTauRoI.h"
#include "xAODTrigger/eFexTauRoIContainer.h"

namespace eFexTauRoIThresholdsToolParams {
  extern const char ContainerName[];
  extern const char ThresholdType[];
  using BaseClass = RoIThresholdsTool<xAOD::eFexTauRoI, xAOD::eFexTauRoIContainer, ContainerName, ThresholdType>;
}

class eFexTauRoIThresholdsTool : public eFexTauRoIThresholdsToolParams::BaseClass {
public:
  eFexTauRoIThresholdsTool(const std::string& type, const std::string& name, const IInterface* parent)
  : eFexTauRoIThresholdsToolParams::BaseClass(type, name, parent) {}

  virtual uint64_t getPattern(const xAOD::eFexTauRoI& roi,
                              const ThrVec& menuThresholds,
                              const TrigConf::L1ThrExtraInfoBase& menuExtraInfo) const override;
};

#endif // HLTSEEDING_EFEXTAUROITHRESHOLDSTOOL_H
