/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEGAMMAMONITORING_TRIGEGAMMAMONITORALGORITHM_H
#define TRIGEGAMMAMONITORING_TRIGEGAMMAMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"


class TrigEgammaMonitorAlgorithm : public AthMonitorAlgorithm {
 public:
  TrigEgammaMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TrigEgammaMonitorAlgorithm();
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

  ///Name of the trigger items to be monitored.
  //std::vector<std::string> m_trigItems; 
};
#endif
