/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTMONITORING_TRIGHLTMONITORALGORITHM_H
#define TRIGHLTMONITORING_TRIGHLTMONITORALGORITHM_H

#include <set>
#include <string>
#include <vector>

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "TrigSteeringEvent/HLTResultMT.h"   
#include "StoreGate/ReadHandleKey.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "xAODTrigger/TrigConfKeys.h"

class TrigHLTMonitorAlgorithm : public AthMonitorAlgorithm {
 public:
  TrigHLTMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TrigHLTMonitorAlgorithm();
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

  private:
   SG::ReadHandleKey<xAOD::TrigConfKeys> m_eventKey{this, "EventObjectName", "TrigConfKeys", "Key for the event-level configuration identifier object"};
   SG::ReadHandleKey<xAOD::TrigConfKeys> m_onlineKey{this, "OnlineObjectName", "TrigConfKeysOnline", "Key for the online configuration identifier object"};

   ToolHandle<Trig::TrigDecisionTool> m_trigDecTool; 
   ServiceHandle< TrigConf::ITrigConfigSvc > m_trigConfigSvc{ this, "TrigConfigSvc", "" };
   StatusCode fillResultAndConsistencyHistograms( const EventContext& ctx ) const;

};
#endif
