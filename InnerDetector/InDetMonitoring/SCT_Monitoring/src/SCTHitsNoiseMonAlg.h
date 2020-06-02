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

#include <array>
#include <unordered_set>

// Forward declarations
class ISCT_ConfigurationConditionsTool;
class SCT_ID;

class SCTHitsNoiseMonAlg : public AthMonitorAlgorithm {
 public:
  SCTHitsNoiseMonAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SCTHitsNoiseMonAlg() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode fillHistograms(const EventContext& ctx) const override final;

 private:

  //Count number of events
  mutable std::atomic<int> m_numberOfEvents{0};
  //Count number of events in the selected stream
  mutable std::atomic<int> m_numberOfEventsTrigger{0};
  //Count number of events since last reset
  mutable std::atomic<int> m_numberOfEventsRecent{0};
  //CAM adds skip events counter
  mutable std::atomic<int> m_skipEvents{0};
      
  mutable std::atomic<int> m_events_lb{0};
  mutable std::atomic<int> m_eventsTrigger_lb{0};

  
  std::vector<int> m_nSP_buf{};   
  mutable std::atomic<int> m_nSP_pos{0};
  std::vector<int> m_nHits_buf{};
  mutable std::atomic<int> m_nHits_pos{0};
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
  
  StatusCode generalHistsandNoise(const std::array<std::unordered_set<Identifier>, SCT_Monitoring::N_WAFERS>& rdosOnTracks, const EventContext& ctx) const;
  StatusCode makeVectorOfTrackRDOIdentifiers(std::array<std::unordered_set<Identifier>, SCT_Monitoring::N_WAFERS>& rdosOnTracks, const EventContext& ctx) const;
  StatusCode makeSPvsEventNumber() const;
};

#endif // SCTHITSNOISEMONALG_H
