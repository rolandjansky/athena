/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGMINBIASMONITORING_HLTMBTSMONITORINGALGMT_H
#define TRIGMINBIASMONITORING_HLTMBTSMONITORINGALGMT_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "StoreGate/ReadHandleKey.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "xAODTrigMinBias/TrigT2MbtsBitsContainer.h"
#include "xAODTrigMinBias/TrigT2MbtsBits.h"

#include <string>

/**
 * @class HLTMBTSMonitoringAlgMT
 * @brief 
 **/
class HLTMBTSMonitoringAlgMT : public AthMonitorAlgorithm
{
public:
  HLTMBTSMonitoringAlgMT(const std::string &name, ISvcLocator *pSvcLocator);
  virtual ~HLTMBTSMonitoringAlgMT() override;

  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms(const EventContext &context) const override;
  virtual StatusCode finalize() override;

private:
  Gaudi::Property<std::vector<std::string>> m_triggerList{this, "triggerList", {}, "Add triggers to this to be monitored"};
  Gaudi::Property<std::vector<std::string>> m_MBTSchannelID{this, "MBTS_channelID", {}, "MBTS channel IDs"};

  SG::ReadHandleKey<xAOD::TrigT2MbtsBitsContainer> m_TrigT2MbtsBitsContainerKey{this, "MBTSbitsKey", "HLT_MbtsBitsContainer", "Name of MbtsBitsContainer"};
};

#endif // TRIGMINBIASMONITORING_HLTMBTSMONITORINGALG_H
