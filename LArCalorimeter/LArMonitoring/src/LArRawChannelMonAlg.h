/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARMONITORING_LARRAWCHANNELMONALG_H
#define LARMONITORING_LARRAWCHANNELMONALG_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "GaudiKernel/ToolHandle.h"
#include "LArRawEvent/LArRawChannelContainer.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "AthenaMonitoring/DQAtlasReadyFilterTool.h"
#include "LArRecConditions/LArBadChannelMask.h"

#include <string>
#include <vector>
#include <set>
#include <array>

class CaloDetDescrManager;
class CaloIdManager;
class LArOnlineID;
class CaloNoise;

// A monitoring algorithm for the LAr's Raw Channels
//
// This AthMonitorAlgorithm is largely based upon the original class
// LArRawChannelMonTool authored by Rob McPherson &Frank Berghaus.
// For the time being, it provides only a subset of the monitoring
// histograms defined in the original class.
class LArRawChannelMonAlg : public AthMonitorAlgorithm {
 public:
  LArRawChannelMonAlg(const std::string&, ISvcLocator*);

  virtual ~LArRawChannelMonAlg();

  // Connects to services and tools
  //
  // The LArOnlineID and LArCablingService are retrieved.
  // Mapping from FEB hash to sub-detector is created.
  virtual StatusCode initialize() override;

  // Fill histograms with monitoring quantities
  //
  // Returns success if no LArRawChannelContainer exists, returns failure if
  // LArRawChannelContainer cannot be retrieved.
  virtual StatusCode fillHistograms(const EventContext& ctx) const override;

 private:
  // Public alg properties
  Gaudi::Property<bool> m_monitor_occupancy{
      this, "monitor_occupancy", false, ""};
  Gaudi::Property<bool> m_monitor_signal{
      this, "monitor_signal", true, ""};
  Gaudi::Property<bool> m_monitor_positive_noise{
      this, "monitor_positive_noise", true, ""};
  Gaudi::Property<bool> m_monitor_negative_noise{
      this, "monitor_negative_noise", true, ""};
  Gaudi::Property<bool> m_monitor_time{
      this, "monitor_time", true, ""};
  Gaudi::Property<bool> m_monitor_quality{
      this, "monitor_quality", false, ""};
  Gaudi::Property<bool> m_monitor_burst{
      this, "monitor_burst", true, ""};
  Gaudi::Property<bool> m_monitor_febs{
      this, "monitor_febs", false, ""};
  Gaudi::Property<bool> m_monitor_feedthroughs{
      this, "monitor_feedthroughs", false, ""};
  Gaudi::Property<bool> m_monitor_detectors{
      this, "monitor_detectors", true, ""};
  Gaudi::Property<std::vector<double>> m_occupancy_thresholds{
      this, "occupancy_thresholds", {8, 500. * Gaudi::Units::MeV}, ""};
  Gaudi::Property<std::vector<double>> m_signal_thresholds{
      this, "signal_thresholds", {8, 500. * Gaudi::Units::MeV}, ""};
  Gaudi::Property<std::vector<int>> m_pos_noise_thresholds{
      this, "pos_noise_thresholds", {8, 3}, ""};
  Gaudi::Property<std::vector<int>> m_neg_noise_thresholds{
      this, "neg_noise_thresholds", {8, 3}, ""};
  Gaudi::Property<double> m_bcid_signal_threshold{
      this, "bcid_signal_threshold", 500. * Gaudi::Units::MeV, ""};
  Gaudi::Property<short> m_time_threshold{
      this, "time_threshold", 5, ""};
  Gaudi::Property<unsigned short> m_quality_threshold{
      this, "quality_threshold", 4000, ""};
  Gaudi::Property<short> m_noise_threshold{
      this, "noise_threshold", 3, ""};
  Gaudi::Property<std::vector<double>> m_noise_burst_percent_thresholds{
      this, "noise_burst_percent_threshold", {8, 1.}, ""};
  Gaudi::Property<std::vector<unsigned>> m_noise_burst_nChannel_thresholds{
      this, "noise_burst_nChannel_threshold", {8, 10}, ""};
  Gaudi::Property<std::vector<std::string>> m_noise_streams{
      this, "noise_streams", {}, ""};
  Gaudi::Property<bool> m_db_and_ofc_only{
      this, "db_and_ofc_only", true, "use channels "
      "only if pedestal and adc2e value from DB and OFC was used"};
  Gaudi::Property<std::vector<std::string>> m_problemsToMask{
      this,"ProblemsToMask", {}, "Bad-Channel categories to mask"};
  ToolHandleArray<IDQFilterTool> m_atlasReady_tools{
      this, "AtlasReadyFilterTool", {}};

  // --- ReadHandle keys ---
  SG::ReadHandleKey<LArRawChannelContainer> m_LArRawChannel_container_key{
      this, "LArRawChannelContainerKey", "LArRawChannels"};

  // --- tools ---
  const LArOnlineID* m_lar_online_id_ptr{nullptr};
  const CaloIdManager* m_calo_id_mgr_ptr{nullptr};

  // Handle to bad-channel mask
  LArBadChannelMask m_bcMask;
  SG::ReadCondHandleKey<LArBadChannelCont> m_bcContKey{
      this, "BadChanKey", "LArBadChannel", "SG key for LArBadChan object"};
  SG::ReadCondHandleKey<CaloNoise> m_noiseKey{
      this, "NoiseKey", "totalNoise", "SG key for noise" };
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{
      this,"CablingKey", "LArOnOffIdMap",
      "SG Key of LArOnOffIdMapping object"};
  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey{
      this, "CaloDetDescrManager", "CaloDetDescrManager",
      "SG Key for CaloDetDescrManager in the Condition Store"};

  // Index of the GenericMonitoringTool associated to a given partition
  // in the vector AthMonitorAlgorithm::m_tools
  std::array<int, 8> m_monitoring_tool_index;

  // Dictionary FEB hash <-> partition
  std::vector<int8_t> m_feb_hash_to_detector;

  // Number of LArRawChannels connected in each detector
  std::array<uint32_t, 8> m_det_to_nchannels;

  // Same as m_noise_streams but retaining only unique streams
  std::set<std::string> m_noise_streams_set;
};

#endif // LARMONITORING_LARRAWCHANNELMONALG_H
