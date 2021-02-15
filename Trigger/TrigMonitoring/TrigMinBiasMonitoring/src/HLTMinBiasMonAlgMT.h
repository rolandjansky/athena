/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGMINBIASMONITORING_HLTMINBIASMONALGMT_H
#define TRIGMINBIASMONITORING_HLTMINBIASMONALGMT_H

// Framework includes

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "GaudiKernel/ToolHandle.h"
//#include "TriggerD3PDMaker/TrigDecisionFillerTool.h"

// STL includes
#include <string>

/**
* @class HLTMinBiasMonAlgMT
* @brief
**/
class HLTMinBiasMonAlgMT : public AthMonitorAlgorithm {
public:
  HLTMinBiasMonAlgMT(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~HLTMinBiasMonAlgMT() override;

  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms(const EventContext& context) const override;
  virtual StatusCode finalize() override;

private:
  SG::ReadHandleKey<xAOD::TrigCompositeContainer> m_spCountsKey { this, "SPCountsKey", "HLT_SpacePointCounts", "Name of Space Points info object produced by the HLT SP counting FEX algorithm" };
  SG::ReadHandleKey<xAOD::TrigCompositeContainer>  m_trkCountsKey  { this, "TrkCountsKey", "HLT_TrackCount", "Name of Online track counts info object produced by the HLT track counting FEX algorithm" };
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_offlineTrkKey { this, "OfflineTrkKey", "InDetTrackParticles", "Name of Offline track counts info object produced by the HLT track counting FEX algorithm" };
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_onlineTrkKey { this, "OnlineTrkKey", "HLT_IDTrack_MinBias_IDTrig", "Name of track counts info object produced by the HLT track counting FEX algorithm" };

  Gaudi::Property<std::vector<std::string>> m_triggerList{this, "triggerList",{}, "Add triggers to this to be monitored"};
  StatusCode monitorPurities(const EventContext& context) const;
  StatusCode monitorSPCounts(const EventContext& context) const;
  StatusCode monitorTrkCounts(const EventContext& context) const;
};
#endif // TRIGMINBIASMONITORING_HLTMINBIASMONALGMT_H
