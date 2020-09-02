/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTMONITORING_TRIGHLTMONITORALGORITHM_H
#define TRIGHLTMONITORING_TRIGHLTMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "TrigSteeringEvent/HLTResultMT.h"   
#include "StoreGate/ReadHandleKey.h"

class TrigHLTMonitorAlgorithm : public AthMonitorAlgorithm {
 public:
  TrigHLTMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TrigHLTMonitorAlgorithm();
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

  private:
   SG::ReadHandleKey<HLT::HLTResultMT> m_hltResultReadKey {this, "HLTResultMT", "HLTResultMT", "Key of the HLTResultMT object" };
   ToolHandle<Trig::TrigDecisionTool> m_trigDecTool; 
   ServiceHandle< TrigConf::ITrigConfigSvc > m_trigConfigSvc{ this, "TrigConfigSvc", "" };
   StatusCode fillResultAndConsistencyHistograms( const EventContext& ctx ) const;
};
#endif
