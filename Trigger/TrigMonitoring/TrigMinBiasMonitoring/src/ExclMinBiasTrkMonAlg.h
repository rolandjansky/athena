/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGMINBIASMONITORING_ExclMinBiasTrkMonAlg_H
#define TRIGMINBIASMONITORING_ExclMinBiasTrkMonAlg_H

// Framework includes
#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "xAODTrigger/EnergySumRoI.h"
#include "xAODEventInfo/EventInfo.h"

// STL includes
#include <string>
#include <regex>
#include <unordered_map>

/**
* @class ExclMinBiasTrkMonAlg
* @brief
**/
class ExclMinBiasTrkMonAlg : public AthMonitorAlgorithm {
  private:
    /// Track number/pT cut parameters.
    struct CutParameters {
      int minTrk;
      int maxTrk;
      float minPt;
    };

public:
  ExclMinBiasTrkMonAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~ExclMinBiasTrkMonAlg() override = default;

  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms(const EventContext& context) const override;
  virtual StatusCode finalize() override;

private:
  SG::ReadHandleKey<xAOD::TrigCompositeContainer>  m_trkCountsKey {this, "TrkCountsKey", "HLT_TrackCount", "Name of Online track counts info object produced by the HLT track counting FEX algorithm"};
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_offlineTrkKey {this, "OfflineTrkKey", "InDetTrackParticles", "Name of Offline track counts info object produced by the HLT track counting FEX algorithm"};
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_onlineTrkKey {this, "OnlineTrkKey", "HLT_IDTrack_MinBias_IDTrig", "Name of track counts info object produced by the HLT track counting FEX algorithm"};

  ToolHandle<InDet::IInDetTrackSelectionTool> m_trackSelectionTool {this, "TrackSelectionTool", "InDetTrackSelectionTool", "Tool for selecting tracks"};

  Gaudi::Property<std::vector<std::string>> m_triggerList{this, "triggerList", {}, "Add triggers to this to be monitored"};
  Gaudi::Property<std::vector<std::string>> m_refTriggerList{this, "refTriggerList", {}, "List of reference triggers used in efficiency calculation"};
  Gaudi::Property<float> m_minPt{this, "minPt", 100.0, "Consider offline tracks only if above this threshold (in MeV)"};

  std::unordered_map<std::string, CutParameters> m_cuts;
};
#endif // TRIGMINBIASMONITORING_ExclMinBiasTrkMonAlg_H
