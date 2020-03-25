// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**    @file SCTTracksMonTool.h
 *   Class declaration for SCTTracksMonTool
 *                         
 *    @author Luca Fiorini, based on code from Shaun Roe, Manuel Diaz Gomez
 *    and Maria Jose Casta.
 *  
 */

#ifndef SCTTRACKSMONTOOL_H
#define SCTTRACKSMONTOOL_H

#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include "SCT_Monitoring/SCT_MonitoringNumbers.h"

#include "StoreGate/ReadHandleKey.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"
#include "TrkToolInterfaces/IUpdator.h"
#include "TrkTrack/TrackCollection.h"
#include "xAODEventInfo/EventInfo.h"

#include "GaudiKernel/ToolHandle.h"

//Standard C++
#include <bitset>
#include <string>
#include <vector>

// Forward declarations
class TH1I;
class TH1F;
class TH2F;
class TProfile2D;
class TProfile;
class SCT_ID;

class SCTTracksMonTool : public ManagedMonitorToolBase {
 public:
  SCTTracksMonTool(const std::string& type, const std::string& name, const IInterface* parent); 
  virtual ~SCTTracksMonTool() = default;
  //initialize
  virtual StatusCode initialize() override final;
  /**    @name Book, fill & check (reimplemented from baseclass) */
  //@{
  ///Book histograms in initialization
  virtual StatusCode bookHistogramsRecurrent() override final;
  virtual StatusCode bookHistograms() override final;
  ///Fill histograms in each loop
  virtual StatusCode fillHistograms() override final;
  ///process histograms at the end (we only use 'isEndOfRun')
  virtual StatusCode procHistograms() override final;
  ///helper function used in procHistograms
  virtual StatusCode checkHists(bool fromFinalize) override final;
  //@} 
  
 private:
  ///enumerated constant for the types of level 1 triggers, corresponds to the string m_triggerNames
  enum TriggerTypes {RNDM=0, BPTX, L1CAL, TGC, RPC, MBTS, COSM, CALIB, N_TRIGGER_TYPES };

  ///Abbreviations for level 1 trigger types
  static const std::string s_triggerNames[N_TRIGGER_TYPES];

  // Data members, which are not changed after initialization
  std::string m_path{""};

  // Data members, which are changed during event processing
  // These have to be converted to local variables or be protected by std::atomic or std::mutex
  // before or when this tool is migrated to the new AthenaMT compatible DQ framework.
  std::vector<int> m_nTracks_buf{}; // This requires a care in AthenaMT.
  int m_nTracks_pos{0}; // This requires a care in AthenaMT.
  int m_numberOfEvents{0}; // This should be conveted to std::atomic_int in AthenaMT.

  //@name Histograms related members
  //@{
  TH1I* m_nTracks{nullptr};
  TH1I* m_trackTrigger{nullptr};
  TH1F* m_totalResidual[SCT_Monitoring::N_REGIONS]{};
  TH1F* m_totalPull[SCT_Monitoring::N_REGIONS]{};


  // Pointer to 1D histogram of number of tracks in each LB
  TH1F* m_tracksPerRegion{nullptr};

  //Pointer to profile histogram of track rates
  TProfile* m_trackRate{nullptr};

  /// Pointer to 1D histogram of Number of SCT Clusters per Track
  TH1F* m_trk_ncluHisto{nullptr};

  /// Pointer to 1D histogram of Track chi2
  TH1F* m_trk_chi2{nullptr};
  
  TH1F* m_trk_N{nullptr};

  /// Pointer to 1D histogram of Track pt
  TH1F* m_trk_pt{nullptr};

  /// Pointer to 1D histogram of Track d0
  TH1F* m_trk_d0{nullptr};

  /// Pointer to 1D histogram of Track z0
  TH1F* m_trk_z0{nullptr};

  /// Pointer to 1D histogram of Track eta
  TH1F* m_trk_eta{nullptr};
  
  /// Pointer to 1D histogram of Track phi
  TH1F* m_trk_phi{nullptr};

  //@}

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
  /// ReadHandleKey of EventInfo
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this, "EventInfoKey", "EventInfo", "Key of xAOD::EventInfo"};

  //@name Tool and service members
  //@{
  /// Kalman Updator for SCT Unbiased states in Residual calculation
  ToolHandle<Trk::IResidualPullCalculator> m_residualPullCalculator{this, "ResPullCalc", "Trk::ResidualPullCalculator/ResidualPullCalculator"};
  ToolHandle<Trk::IUpdator> m_updator{this, "KalmanUpdator", "Trk::KalmanUpdator/TrkKalmanUpdator", ""};

  ///SCT Helper class
  const SCT_ID* m_pSCTHelper{nullptr};
  
  //@}
  //@name  Histograms related methods
  //@{
  // Book Track related  Histograms
  StatusCode bookGeneralHistos();
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
  
  
  ///Factory + register for the 1D histograms, returns whether successfully registered
  StatusCode h1Factory(const std::string& name, const std::string& title, const float extent, MonGroup& registry, std::vector<TH1F*>& storageVector) const;
  //@}
};

#endif
