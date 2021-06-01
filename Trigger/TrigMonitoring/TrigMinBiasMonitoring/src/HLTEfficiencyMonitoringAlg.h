/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGMINBIASMONITORING_HLTEFFICIENCYMONITORINGALG_H
#define TRIGMINBIASMONITORING_HLTEFFICIENCYMONITORINGALG_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "StoreGate/ReadHandleKey.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODTrigMinBias/TrigT2MbtsBitsContainer.h"
#include "xAODTrigMinBias/TrigT2MbtsBits.h"

#include <string>

/**
 * @class HLTEfficiencyMonitoringAlg
 * @brief 
 **/
class HLTEfficiencyMonitoringAlg : public AthMonitorAlgorithm
{
public:
  HLTEfficiencyMonitoringAlg(const std::string &name, ISvcLocator *pSvcLocator);
  virtual ~HLTEfficiencyMonitoringAlg() override;

  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms(const EventContext &context) const override;
  virtual StatusCode finalize() override;

private:
  Gaudi::Property<std::vector<std::string>> m_triggerList{this, "triggerList", {}, "Add triggers to this to be monitored"};
  Gaudi::Property<std::vector<std::string>> m_refTriggerList{this, "refTriggerList", {}, "Add ref triggers to this to be monitored"};
  Gaudi::Property<std::string> m_trigger{this, "trigger", {}, "Trigger to be monitored"};
  Gaudi::Property<std::string> m_refTrigger{this, "refTrigger", {}, "Reference trigger"};

  SG::ReadHandleKey<xAOD::TrigCompositeContainer> m_trkCountsKey{this, "TrkCountsKey", "HLT_TrackCount", "Name of Online track counts info object produced by the HLT track counting FEX algorithm"};
  SG::ReadHandleKey<xAOD::TrigT2MbtsBitsContainer> m_TrigT2MbtsBitsContainerKey{this, "MBTSbitsKey", "HLT_MbtsBitsContainer", "Name of MbtsBitsContainer"};
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_offlineTrkKey{this, "OfflineTrkKey", "InDetTrackParticles", "Name of Offline track counts info object produced by the HLT track counting FEX algorithm"};

  ToolHandle<InDet::IInDetTrackSelectionTool> m_trackSelectionTool{this, "TrackSelectionTool", "InDetTrackSelectionTool", "Tool for selecting tracks"};
};

#endif // TRIGMINBIASMONITORING_HLTEFFICIENCYMONITORINGALG_H
