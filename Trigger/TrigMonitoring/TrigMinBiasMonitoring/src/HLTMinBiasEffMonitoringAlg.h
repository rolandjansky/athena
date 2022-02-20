/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGMINBIASMONITORING_HLTMINBIASEFFMONITORINGALG_H
#define TRIGMINBIASMONITORING_HLTMINBIASEFFMONITORINGALG_H

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
class HLTMinBiasEffMonitoringAlg : public AthMonitorAlgorithm
{
public:
  HLTMinBiasEffMonitoringAlg(const std::string &name, ISvcLocator *pSvcLocator);
  virtual ~HLTMinBiasEffMonitoringAlg() override;

  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms(const EventContext &context) const override;
  virtual StatusCode finalize() override;

private:
  Gaudi::Property<std::vector<std::string>> m_triggerList{this, "triggerList", {}, "Add triggers to this to be monitored"};
  Gaudi::Property<std::vector<std::string>> m_refTriggerList{this, "refTriggerList", {}, "Add ref triggers to this to be monitored"};
  std::vector<std::string> m_uniqueTriggerList;

  SG::ReadHandleKey<xAOD::TrigCompositeContainer> m_trkCountsKey{this, "TrkCountsKey", "HLT_TrackCount", "Name of Online track counts info object produced by the HLT track counting FEX algorithm"};
  SG::ReadHandleKey<xAOD::TrigT2MbtsBitsContainer> m_TrigT2MbtsBitsContainerKey{this, "MBTSbitsKey", "HLT_MbtsBitsContainer", "Name of MbtsBitsContainer"};
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_offlineTrkKey{this, "OfflineTrkKey", "InDetTrackParticles", "Name of Offline track counts info object produced by the HLT track counting FEX algorithm"};

  ToolHandle<InDet::IInDetTrackSelectionTool> m_trackSelectionTool{this, "TrackSelectionTool", "InDetTrackSelectionTool", "Tool for selecting tracks"};
  Gaudi::Property<float> m_minPt{ this, "minPt", 200.0, "Consider offline tracks only if above this threshold (in MeV)"};
  
};

#endif // TRIGMINBIASMONITORING_HLTHLTMINBIASEFFMONITORINGALG_H
