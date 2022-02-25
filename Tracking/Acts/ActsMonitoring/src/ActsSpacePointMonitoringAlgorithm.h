/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTS_MONITORING_ALGORITHM_SPACE_POINT_H
#define ACTS_MONITORING_ALGORITHM_SPACE_POINT_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/ReadHandleKey.h"

#include "ActsEDM/ActsSpacePoint.h" 

namespace ActsTrk {

  class ActsSpacePointMonitoringAlgorithm :
    public AthMonitorAlgorithm {
  public:
    ActsSpacePointMonitoringAlgorithm(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~ActsSpacePointMonitoringAlgorithm() override = default;

    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms(const EventContext& ctx) const override;

  private:
    SG::ReadHandleKey< ActsTrk::SpacePointContainer > m_inputSpacePointColletionKey {this,  "InputSpacePointCollection", "", ""}; 
  };

}

#endif
