/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGMINBIASMONITORING_TRIGAFPSIDHYPOMONITORINGALG_H
#define TRIGMINBIASMONITORING_TRIGAFPSIDHYPOMONITORINGALG_H

// Framework includes
#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

// STL includes
#include <string>

/**
 * @class TrigAFPSidHypoMonitoringAlg
 * @brief Monitors performance of AF SID reco and related hypo
 **/
class TrigAFPSidHypoMonitoringAlg : public AthMonitorAlgorithm {
public:
  TrigAFPSidHypoMonitoringAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TrigAFPSidHypoMonitoringAlg() override;

  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms(const EventContext& context) const override;
  virtual StatusCode finalize() override;

private:
  Gaudi::Property<std::vector<std::string>> m_chains {this, "Chains", {}, "Chains to monitor"};
};

#endif // TRIGMINBIASMONITORING_TRIGAFPSIDHYPOMONITORINGALG_H
