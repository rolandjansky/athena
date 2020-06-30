/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETMONITORING_TRIGL1JETMONITORALGORITHM_H
#define TRIGJETMONITORING_TRIGL1JETMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "xAODTrigger/JetRoIContainer.h"


class TrigL1JetMonitorAlgorithm : public AthMonitorAlgorithm {
 public:
  TrigL1JetMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TrigL1JetMonitorAlgorithm();
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

  // Name of the L1 jet collection to be monitored
  SG::ReadHandleKey<xAOD::JetRoIContainer> m_l1jetContainerkey;
};
#endif
