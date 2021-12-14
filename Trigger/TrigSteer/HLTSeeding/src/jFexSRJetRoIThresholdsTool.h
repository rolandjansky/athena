/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef HLTSEEDING_JFEXSRJETROITHRESHOLDSTOOL_H
#define HLTSEEDING_JFEXSRJETROITHRESHOLDSTOOL_H

#include "HLTSeedingRoIToolDefs.h"
#include "HLTSeeding/IRoIThresholdsTool.h"
#include "xAODTrigger/jFexSRJetRoI.h"

class jFexSRJetRoIThresholdsTool : public HLTSeedingRoIToolDefs::jFexSRJet::ThresholdBaseClass {
public:
  jFexSRJetRoIThresholdsTool(const std::string& type, const std::string& name, const IInterface* parent)
  : HLTSeedingRoIToolDefs::jFexSRJet::ThresholdBaseClass(type, name, parent) {}

  virtual uint64_t getPattern(const xAOD::jFexSRJetRoI& roi,
                              const ThrVec& menuThresholds,
                              const TrigConf::L1ThrExtraInfoBase& menuExtraInfo) const override;
};

#endif // HLTSEEDING_JFEXSRJETROITHRESHOLDSTOOL_H
