/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSTRKANALYSIS_SEEDANALYSIS_H
#define ACTSTRKANALYSIS_SEEDANALYSIS_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "ActsTrkEvent/Seed.h"

namespace ActsTrk {

  class SeedAnalysis final :
    public AthMonitorAlgorithm {
  public:
    SeedAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~SeedAnalysis() override = default;

    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms(const EventContext& ctx) const override;

  private:
    SG::ReadHandleKey< ActsTrk::SeedContainer > m_inputSeedColletionKey {this,  "InputSeedCollection", "", ""}; 
  };

}

#endif
