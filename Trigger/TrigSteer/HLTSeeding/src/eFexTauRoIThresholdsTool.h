/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef HLTSEEDING_EFEXTAUROITHRESHOLDSTOOL_H
#define HLTSEEDING_EFEXTAUROITHRESHOLDSTOOL_H

#include "HLTSeedingRoIToolDefs.h"
#include "HLTSeeding/IRoIThresholdsTool.h"
#include "xAODTrigger/eFexTauRoI.h"

class eFexTauRoIThresholdsTool : public HLTSeedingRoIToolDefs::eFexTau::ThresholdBaseClass {
public:
  eFexTauRoIThresholdsTool(const std::string& type, const std::string& name, const IInterface* parent)
  : HLTSeedingRoIToolDefs::eFexTau::ThresholdBaseClass(type, name, parent) {}

  virtual uint64_t getPattern(const xAOD::eFexTauRoI& roi,
                              const ThrVec& menuThresholds,
                              const TrigConf::L1ThrExtraInfoBase& menuExtraInfo) const override;
};

#endif // HLTSEEDING_EFEXTAUROITHRESHOLDSTOOL_H
