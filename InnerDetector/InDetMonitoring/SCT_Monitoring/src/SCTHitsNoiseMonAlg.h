// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCTHITSNOISEMONALG_H
#define SCTHITSNOISEMONALG_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "SCT_Monitoring/SCT_MonitoringNumbers.h"


#include "Identifier/Identifier.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "TrkSpacePoint/SpacePointContainer.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

#include "GaudiKernel/ToolHandle.h"

#include <shared_mutex>

// Forward declarations
class ISCT_ConfigurationConditionsTool;
class SCT_ID;

using namespace std;

class SCTHitsNoiseMonAlg : public AthMonitorAlgorithm {
 public:
  SCTHitsNoiseMonAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SCTHitsNoiseMonAlg() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode fillHistograms(const EventContext& ctx) const override final;

 private:
   enum Thresholds {thresh100, thresh1000, thresh10000, nThreshes};
  static const double s_thresholds[nThreshes];
  static const std::string s_thresholdNames[nThreshes];


  mutable atomic<int> m_current_lb{0};
  mutable atomic<int> m_last_reset_lb{0};
  //Count number of events
  mutable atomic<int> m_numberOfEvents{0};
  //Count number of events in the selected stream
  mutable atomic<int> m_numberOfEventsTrigger{0};
  //Count number of events since last reset
  mutable atomic<int> m_numberOfEventsRecent{0};
  //CAM adds skip events counter
  mutable atomic<int> m_skipEvents{0};
      
 
  mutable std::shared_mutex m_sharedMutex ATLAS_THREAD_SAFE;

  struct Data
  {
    std::array<std::atomic<int>, SCT_Monitoring::NBINS_LBs+1> m_noisyM[nThreshes];
    std::array<std::atomic<int>, SCT_Monitoring::NBINS_LBs+1> m_occ_lb[SCT_Monitoring::N_REGIONS+1];
    std::array<std::atomic<int>, SCT_Monitoring::NBINS_LBs+1> m_noisyMTrigger[nThreshes];
    std::array<std::atomic<int>, SCT_Monitoring::NBINS_LBs+1> m_occTrigger_lb[SCT_Monitoring::N_REGIONS+1];

    std::array<std::atomic<int>, SCT_Monitoring::NBINS_LBs+1> m_noisyMWithHO[nThreshes];
    std::array<std::atomic<int>, SCT_Monitoring::NBINS_LBs+1> m_hitocc_lb[SCT_Monitoring::N_REGIONS+1];
    std::array<std::atomic<int>, SCT_Monitoring::NBINS_LBs+1> m_noisyMWithHOTrigger[nThreshes];
    std::array<std::atomic<int>, SCT_Monitoring::NBINS_LBs+1> m_hitoccTrigger_lb[SCT_Monitoring::N_REGIONS+1];

    ///additional maps for track NO to compare with SP NO calc
    std::array<std::atomic<float>, SCT_Monitoring::N_WAFERS> m_occSumUnbiased;
    std::array<std::atomic<float>, SCT_Monitoring::N_WAFERS> m_occSumUnbiasedTrigger;
    std::array<std::atomic<float>, SCT_Monitoring::N_WAFERS> m_occSumUnbiasedRecent;

    std::array<std::atomic<float>, SCT_Monitoring::N_WAFERS> m_occSumUnbiased_lb[SCT_Monitoring::N_REGIONS+1];
    std::array<std::atomic<float>, SCT_Monitoring::N_WAFERS> m_occSumUnbiasedTrigger_lb[SCT_Monitoring::N_REGIONS+1];

    std::array<std::atomic<float>, SCT_Monitoring::N_WAFERS> m_hitoccSumUnbiased;
    std::array<std::atomic<float>, SCT_Monitoring::N_WAFERS> m_hitoccSumUnbiasedTrigger;
    std::array<std::atomic<float>, SCT_Monitoring::N_WAFERS> m_hitoccSumUnbiasedRecent;

    std::array<std::atomic<float>, SCT_Monitoring::N_WAFERS> m_hitoccSumUnbiased_lb[SCT_Monitoring::N_REGIONS+1];
    std::array<std::atomic<float>, SCT_Monitoring::N_WAFERS> m_hitoccSumUnbiasedTrigger_lb[SCT_Monitoring::N_REGIONS+1];
  };
  std::unique_ptr<Data> m_data;

  
  mutable atomic<int> m_events_lb{0};
  mutable atomic<int> m_eventsTrigger_lb{0};

  
  std::vector<int> m_nSP_buf{};   
  mutable atomic<int> m_nSP_pos{0};
  std::vector<int> m_nHits_buf{};
  mutable atomic<int> m_nHits_pos{0};
  std::vector<int> m_nmaxHits_buf{};
  std::vector<Identifier> m_nmaxModule_buf{};
  std::vector<int> m_nminHits_buf{};

 
  /// Name of the L1 Type to use for filling the extra NO histograms
  StringProperty m_NOTriggerItem{this, "NOTrigger", "L1_RD0_EMPTY"};
  BooleanProperty m_doTrigger{this, "doTrigger", false};
  /// Switch on or off the hitmaps histograms
  //BooleanProperty m_boolhitmaps{this, "doHitmapHistos", false};
  /// Switch on or off the endcaps
  BooleanProperty m_doPositiveEndcap{this, "doPositiveEndcap", true};
  BooleanProperty m_doNegativeEndcap{this, "doNegativeEndcap", true};
  ///Select the noise algorithm
  BooleanProperty m_doTrackBasedNoise{this, "doTrackBasedNoise", false};
  BooleanProperty m_doSpacePointBasedNoise{this, "doSpacePointBasedNoise", true};
  /// Add time-bin filtering to space point NO algorithm
  BooleanProperty m_doTimeBinFilteringForNoise{this, "doTimeBinFilteringForNoise", true};
  /// CheckHists() frequency
  IntegerProperty m_checkrate{this, "CheckRate", 1000};
  IntegerProperty m_checkrecent{this, "CheckRecent", 30};
  /// ChechHists() frequency
  IntegerProperty m_evtsbins{this, "EvtsBins", 5000};
  //Things for doing time bin distributions for track hits
  BooleanProperty m_doTrackHits{this, "doTrackHits", true};
  UnsignedIntegerProperty m_maxTracks{this, "MaxTracks", 1000};
  ///Determines whether a local summary is written
  IntegerProperty m_localSummary{this, "localSummary", 0};

  /// Name of the Track collection to use
  SG::ReadHandleKey<TrackCollection> m_tracksName{this, "tracksName", "CombinedInDetTracks"};
  SG::ReadHandleKey<InDet::SCT_ClusterContainer> m_clusContainerKey{this, "ClusterKey", "SCT_Clusters"};
  //CAM adds map for SP NO
  SG::ReadHandleKey<SpacePointContainer> m_SCTSPContainerName{this, "SpacePointKey", "SCT_SpacePoints"};
  /// Data object name: for the SCT this is "SCT_RDOs"
  SG::ReadHandleKey<SCT_RDO_Container> m_dataObjectName{this, "RDOKey", "SCT_RDOs"};
  // For P->T converter of SCT_Clusters
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};

  //@name Tool methods
  //@{
  ToolHandle<ISCT_ConfigurationConditionsTool> m_ConfigurationTool{this, "conditionsTool",
      "SCT_ConfigurationConditionsTool/InDetSCT_ConfigurationConditionsTool", "Tool to retrieve SCT Configuration Tool"};
  //@}

  //@nameService members
  //@{  
  ///SCT Helper class
  const SCT_ID* m_pSCTHelper{nullptr};
  //@}
  
  StatusCode generalHistsandNoise(const std::vector<Identifier>& rdosOnTracks) const;
  StatusCode makeVectorOfTrackRDOIdentifiers(std::vector<Identifier>& rdosOnTracks, const EventContext& ctx) const;
  StatusCode makeSPvsEventNumber() const;
  void resetCaches() const;


};

#endif // SCTHITSNOISEMONALG_H
