/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGBJETMONITORING_TRIGBJETMONITORALGORITHM_H
#define TRIGBJETMONITORING_TRIGBJETMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoring/Monitored.h"


class TrigBjetMonitorAlgorithm : public AthMonitorAlgorithm {
 public:
  TrigBjetMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TrigBjetMonitorAlgorithm();
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;
};
#endif
