/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef HLTSEEDING_GFEXLRJETROITHRESHOLDSTOOL_H
#define HLTSEEDING_GFEXLRJETROITHRESHOLDSTOOL_H

#include "HLTSeedingRoIToolDefs.h"
#include "HLTSeeding/IRoIThresholdsTool.h"
#include "xAODTrigger/gFexJetRoI.h"

class gFexLRJetRoIThresholdsTool : public HLTSeedingRoIToolDefs::gFexLRJet::ThresholdBaseClass {
public:
  gFexLRJetRoIThresholdsTool(const std::string& type, const std::string& name, const IInterface* parent)
  : HLTSeedingRoIToolDefs::gFexLRJet::ThresholdBaseClass(type, name, parent) {}

  virtual uint64_t getPattern(const xAOD::gFexJetRoI& roi,
                              const ThrVec& menuThresholds,
                              const TrigConf::L1ThrExtraInfoBase& menuExtraInfo) const override;
};

#endif // HLTSEEDING_GFEXLRJETROITHRESHOLDSTOOL_H
