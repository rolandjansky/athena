/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCALOMONITORING_TRIGCALOMONITORALGORITHM_H
#define TRIGCALOMONITORING_TRIGCALOMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoring/Monitored.h"


class TrigCaloMonitorAlgorithm : public AthMonitorAlgorithm {
 public:
  TrigCaloMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TrigCaloMonitorAlgorithm();
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;
};
#endif
