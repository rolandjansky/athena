/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef HLTSEEDING_EFEXEMROITHRESHOLDSTOOL_H
#define HLTSEEDING_EFEXEMROITHRESHOLDSTOOL_H

#include "HLTSeedingRoIToolDefs.h"
#include "HLTSeeding/IRoIThresholdsTool.h"
#include "xAODTrigger/eFexEMRoI.h"

class eFexEMRoIThresholdsTool : public HLTSeedingRoIToolDefs::eFexEM::ThresholdBaseClass {
public:
  eFexEMRoIThresholdsTool(const std::string& type, const std::string& name, const IInterface* parent)
  : HLTSeedingRoIToolDefs::eFexEM::ThresholdBaseClass(type, name, parent) {}

  virtual uint64_t getPattern(const xAOD::eFexEMRoI& roi,
                              const ThrVec& menuThresholds,
                              const TrigConf::L1ThrExtraInfoBase& menuExtraInfo) const override;
};

#endif // HLTSEEDING_EFEXEMROITHRESHOLDSTOOL_H
