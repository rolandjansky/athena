/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTS_MONITORING_ALGORITHM_SEED_H
#define ACTS_MONITORING_ALGORITHM_SEED_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/ReadHandleKey.h"

#include "ActsEDM/ActsSpacePoint.h" 
#include "ActsEDM/ActsSeed.h"

namespace ActsTrk {

  class ActsSeedMonitoringAlgorithm :
    public AthMonitorAlgorithm {
  public:
    ActsSeedMonitoringAlgorithm(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~ActsSeedMonitoringAlgorithm() override = default;

    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms(const EventContext& ctx) const override;

  private:
    SG::ReadHandleKey< ActsTrk::SeedContainer > m_inputSeedColletionKey {this,  "InputSeedCollection", "", ""}; 
  };

}

#endif
