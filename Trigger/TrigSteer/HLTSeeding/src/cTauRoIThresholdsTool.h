/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef HLTSEEDING_CTAUROITHRESHOLDSTOOL_H
#define HLTSEEDING_CTAUROITHRESHOLDSTOOL_H

#include "HLTSeedingRoIToolDefs.h"
#include "HLTSeeding/IRoIThresholdsTool.h"
#include "xAODTrigger/eFexTauRoI.h"
#include "StoreGate/ReadDecorHandleKey.h"

class cTauRoIThresholdsTool : public HLTSeedingRoIToolDefs::cTau::ThresholdBaseClass {
public:
  cTauRoIThresholdsTool(const std::string& type, const std::string& name, const IInterface* parent)
  : HLTSeedingRoIToolDefs::cTau::ThresholdBaseClass(type, name, parent) {}

  // Override initialize to initialize the extra data handle
  virtual StatusCode initialize() override;

  virtual uint64_t getPattern(const xAOD::eFexTauRoI& eTau,
                              const ThrVec& menuThresholds,
                              const TrigConf::L1ThrExtraInfoBase& menuExtraInfo) const override;
private:
  SG::ReadDecorHandleKey<xAOD::eFexTauRoIContainer> m_jTauLinkKey {
    this, "jTauLinkKey", "L1_cTauRoI.jTauLink",
    "Decoration for the link from eTau to the matching jTau"};
};

#endif // HLTSEEDING_CTAUROITHRESHOLDSTOOL_H
