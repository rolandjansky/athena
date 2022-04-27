/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSTRKANALYSIS_ESTIMATEDTRACKPARAMSANALYSIS_H
#define ACTSTRKANALYSIS_ESTIMATEDTRACKPARAMSANALYSIS_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/ReadHandleKey.h"
#include "ActsTrkEvent/TrackParameters.h"

namespace ActsTrk {

  class EstimatedTrackParamsAnalysis final :
    public AthMonitorAlgorithm {
  public:
    EstimatedTrackParamsAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~EstimatedTrackParamsAnalysis() override = default;

    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms(const EventContext& ctx) const override;

  private:
    SG::ReadHandleKey< ActsTrk::BoundTrackParametersContainer > m_inputTrackParamsColletionKey {this,  "InputTrackParamsCollection", "", ""}; 
  };

}

#endif
