/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMETMONITORING_TRIGMETMONITORALGORITHM_H
#define TRIGMETMONITORING_TRIGMETMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoring/Monitored.h"


class TrigMETMonitorAlgorithm : public AthMonitorAlgorithm {
 public:
  TrigMETMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TrigMETMonitorAlgorithm();
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;
};
#endif
