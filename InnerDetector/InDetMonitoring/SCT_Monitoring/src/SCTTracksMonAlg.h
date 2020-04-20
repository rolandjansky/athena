// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCTTRACKSMONALG_H
#define SCTTRACKSMONALG_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"

#include "SCT_Monitoring/SCT_MonitoringNumbers.h"

#include "StoreGate/ReadHandleKey.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"
#include "TrkToolInterfaces/IUpdator.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "TrkTrack/TrackCollection.h"
#include "xAODEventInfo/EventInfo.h"

#include "GaudiKernel/ToolHandle.h"

//Standard C++
#include <bitset>
#include <string>
#include <vector>

class SCT_ID;

class SCTTracksMonAlg : public AthMonitorAlgorithm {
 public:
  SCTTracksMonAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SCTTracksMonAlg() = default;	
  virtual StatusCode initialize() override final;
  virtual StatusCode fillHistograms(const EventContext& ctx) const override final;
   
private:
  ///enumerated constant for the types of level 1 triggers, corresponds to the string m_triggerNames
  enum TriggerTypes {RNDM=0, BPTX, L1CAL, TGC, RPC, MBTS, COSM, CALIB, N_TRIGGER_TYPES };
  
  const std::string m_regionNames[3]{"EndCapC", "Barrel", "EndCapA"}; //3 Regions: This can be improved


  ///Abbreviations for level 1 trigger types
  static const std::string s_triggerNames[N_TRIGGER_TYPES];

  // Data members, which are not changed after initialization
  std::string m_stream{"/stat"};
  std::string m_path{""};

  // Data members, which are changed during event processing
  // These have to be converted to local variables or be protected by std::atomic or std::mutex
  // before or when this tool is migrated to the new AthenaMT compatible DQ framework.
  mutable std::atomic_int m_numberOfEvents{0}; // This should be conveted to std::atomic_int in AthenaMT.


  /// Cut on number of SCT hits on track
  IntegerProperty m_trackHitCut{this, "trackHitCut", 3};
  /// CheckHists() frequency
  IntegerProperty m_checkrate{this, "CheckRate", 1000};
  /// Tracks vs evt
  IntegerProperty m_evtsbins{this, "EvtsBins", 5000};

  BooleanProperty m_useIDGlobal{this, "useIDGlobal", false};
  BooleanProperty m_doTrigger{this, "doTrigger", false};
  BooleanProperty m_doPositiveEndcap{this, "doPositiveEndcap", true};
  BooleanProperty m_doNegativeEndcap{this, "doNegativeEndcap", true};
  BooleanProperty m_doUnbiasedCalc{this, "doUnbiasedCalc", true};

  /// Name of the Track collection to use
  SG::ReadHandleKey<TrackCollection> m_tracksName{this, "tracksName", "CombinedInDetTracks"};


  //@name Tool and service members
  //@{
  ToolHandle<Trk::ITrackSummaryTool> m_trackSummaryTool{this, "TrackSummaryTool", "InDetTrackSummaryTool"};

  /// Kalman Updator for SCT Unbiased states in Residual calculation
  ToolHandle<Trk::IResidualPullCalculator> m_residualPullCalculator{this, "ResPullCalc", "Trk::ResidualPullCalculator/ResidualPullCalculator"};
  ToolHandle<Trk::IUpdator> m_updator{this, "KalmanUpdator", "Trk::KalmanUpdator/TrkKalmanUpdator", ""};

  ///SCT Helper class
  const SCT_ID* m_pSCTHelper{nullptr};

  //@}
  //@name  Trigger related methods
  //{
  ///Fill the m_firedTriggers bitset according to event information
  StatusCode checkTriggers(std::bitset<N_TRIGGER_TYPES>& firedTriggers) const;
  ///Get the status of a particular trigger bit (trigger bit 0-7)
  bool hasTriggerFired(const unsigned int trigger, const std::bitset<N_TRIGGER_TYPES>& firedTriggers) const;
  //@}
  
  //@name Service methods
  //@{
  /// Calculate Pull value for MeasuredAtPlane TrackStates
  float calculatePull(const float, const float, const float) const;
  
  //@}

};

#endif // SCTTRACKSMONALG_H
