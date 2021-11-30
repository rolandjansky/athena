/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef HLTSEEDING_JFEXTAUROITHRESHOLDSTOOL_H
#define HLTSEEDING_JFEXTAUROITHRESHOLDSTOOL_H

#include "HLTSeedingRoIToolDefs.h"
#include "HLTSeeding/IRoIThresholdsTool.h"
#include "xAODTrigger/jFexTauRoI.h"

class jFexTauRoIThresholdsTool : public HLTSeedingRoIToolDefs::jFexTau::ThresholdBaseClass {
public:
  jFexTauRoIThresholdsTool(const std::string& type, const std::string& name, const IInterface* parent)
  : HLTSeedingRoIToolDefs::jFexTau::ThresholdBaseClass(type, name, parent) {}

  virtual uint64_t getPattern(const xAOD::jFexTauRoI& roi,
                              const ThrVec& menuThresholds,
                              const TrigConf::L1ThrExtraInfoBase& menuExtraInfo) const override;
};

#endif // HLTSEEDING_JFEXTAUROITHRESHOLDSTOOL_H
