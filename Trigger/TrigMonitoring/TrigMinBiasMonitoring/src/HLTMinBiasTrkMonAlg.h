/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGMINBIASMONITORING_HLTMinBiasTrkMonAlg_H
#define TRIGMINBIASMONITORING_HLTMinBiasTrkMonAlg_H

// Framework includes

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "xAODTrigger/EnergySumRoI.h" 

// STL includes
#include <string>

/**
* @class HLTMinBiasTrkMonAlg
* @brief
**/
class HLTMinBiasTrkMonAlg : public AthMonitorAlgorithm {
public:
  HLTMinBiasTrkMonAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~HLTMinBiasTrkMonAlg() override;

  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms(const EventContext& context) const override;
  virtual StatusCode finalize() override;

private:
  SG::ReadHandleKey<xAOD::TrigCompositeContainer> m_spCountsKey { this, "SPCountsKey", "HLT_SpacePointCounts", "Name of Space Points info object produced by the HLT SP counting FEX algorithm" };
  SG::ReadHandleKey<xAOD::TrigCompositeContainer>  m_trkCountsKey  { this, "TrkCountsKey", "HLT_TrackCount", "Name of Online track counts info object produced by the HLT track counting FEX algorithm" };
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_offlineTrkKey { this, "OfflineTrkKey", "InDetTrackParticles", "Name of Offline track counts info object produced by the HLT track counting FEX algorithm" };
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_onlineTrkKey { this, "OnlineTrkKey", "HLT_IDTrack_MinBias_IDTrig", "Name of track counts info object produced by the HLT track counting FEX algorithm" };
  
  SG::ReadHandleKey<xAOD::EnergySumRoI> m_lvl1EnergySumROIKey { this, "lvl1EnergySumROIKey", "LVL1EnergySumRoI", "Name of Sum of Energy info object produced by the HLT track counting FEX algorithm" };

  Gaudi::Property<std::vector<std::string>> m_triggerList{this, "triggerList",{}, "Add triggers to this to be monitored"};
  ToolHandle<InDet::IInDetTrackSelectionTool> m_trackSelectionTool {this, "TrackSelectionTool", "InDetTrackSelectionTool", "Tool for selecting tracks"};
  Gaudi::Property<float> m_minPt{ this, "minPt", 0.0, "Consider offline tracks only if above this threshold (in MeV)"};
  Gaudi::Property<float> m_z0{ this, "z0", 400.0, "Consider offline tracks only if the point to vertex that is withink this value"};
  
  StatusCode monitorPurities(const EventContext& context) const;
  StatusCode monitorSPCounts(const EventContext& context) const;
  StatusCode monitorTrkCounts(const EventContext& context) const;

};
#endif // TRIGMINBIASMONITORING_HLTMinBiasTrkMonAlg_H
