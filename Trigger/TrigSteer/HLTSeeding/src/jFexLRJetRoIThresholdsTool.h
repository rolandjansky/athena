/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef HLTSEEDING_JFEXLRJETROITHRESHOLDSTOOL_H
#define HLTSEEDING_JFEXLRJETROITHRESHOLDSTOOL_H

#include "HLTSeedingRoIToolDefs.h"
#include "HLTSeeding/IRoIThresholdsTool.h"
#include "xAODTrigger/jFexLRJetRoI.h"

class jFexLRJetRoIThresholdsTool : public HLTSeedingRoIToolDefs::jFexLRJet::ThresholdBaseClass {
public:
  jFexLRJetRoIThresholdsTool(const std::string& type, const std::string& name, const IInterface* parent)
  : HLTSeedingRoIToolDefs::jFexLRJet::ThresholdBaseClass(type, name, parent) {}

  virtual uint64_t getPattern(const xAOD::jFexLRJetRoI& roi,
                              const ThrVec& menuThresholds,
                              const TrigConf::L1ThrExtraInfoBase& menuExtraInfo) const override;
};

#endif // HLTSEEDING_JFEXLRJETROITHRESHOLDSTOOL_H
