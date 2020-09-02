/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCRAWDATAMONITORING_TGCRAWDATAMONITORALGORITHM_H
#define TGCRAWDATAMONITORING_TGCRAWDATAMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTrigger/MuonRoIContainer.h"
#include "MuonTrigCoinData/TgcCoinDataContainer.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "MuonPrepRawData/TgcPrepDataContainer.h"

class TgcRawDataMonitorAlgorithm : public AthMonitorAlgorithm {
 public:
  TgcRawDataMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TgcRawDataMonitorAlgorithm()=default;
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;
  
  struct TagDef{
    TString eventTrig;
    TString tagTrig;
  };
  struct MyMuon{
    const xAOD::Muon* muon;
    TLorentzVector fourvec;
    std::vector<TVector3> extPos;
    std::vector<TVector3> extVec;
    bool tagged;
    bool isolated;
    bool probeOK_any;
    bool probeOK_Z;
    std::set<int> matchedL1ThrExclusive;
    std::set<int> matchedL1ThrInclusive;
  };

  struct TgcHit{
    float x;
    float y;
    float z;
    float shortWidth;
    float longWidth;
    float length;
    int isStrip;
    int gasGap;
    int channel;
    int eta;
    int phi;
    int station;
    int bunch;
  };
  struct TgcTrig{
    float x_In;
    float y_In;
    float z_In;
    float x_Out;
    float y_Out;
    float z_Out;
    float width_In;
    float width_Out;
    float width_R;
    float width_Phi;
    int isAside;
    int isForward;
    int isStrip;
    int isInner;
    int isPositiveDeltaR;
    int type;
    int trackletId;
    int trackletIdStrip;
    int phi;
    int roi;
    int pt;
    int delta;
    int sub;
    int veto;
    int bunch;
    int inner;
  };

  
 private:
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

  SG::ReadHandleKey<xAOD::MuonContainer> m_MuonContainerKey{this,"MuonContainerName","Muons","Offline muon track container"};
  SG::ReadHandleKey<xAOD::MuonRoIContainer> m_MuonRoIContainerKey{this,"MuonRoIContainerName","LVL1MuonRoIs","L1 muon RoI container"};
  
  SG::ReadHandleKey<Muon::TgcPrepDataContainer> m_TgcPrepDataContainerKey{this,"TgcPrepDataContainerName","TGC_MeasurementsAllBCs","current BC TGC PRD"};
  SG::ReadHandleKey<Muon::TgcCoinDataContainer> m_TgcCoinDataContainerCurrBCKey{this,"TgcCoinDataContainerCurrBCName","TrigT1CoinDataCollection","TGC Coin Data Container CurrBC"};
  SG::ReadHandleKey<Muon::TgcCoinDataContainer> m_TgcCoinDataContainerNextBCKey{this,"TgcCoinDataContainerNextBCName","TrigT1CoinDataCollectionNextBC","TGC Coin Data Container NextBC"};
  SG::ReadHandleKey<Muon::TgcCoinDataContainer> m_TgcCoinDataContainerPrevBCKey{this,"TgcCoinDataContainerPrevBCName","TrigT1CoinDataCollectionPriorBC","TGC Coin Data Container PrevBC"};
  
  StringProperty m_packageName{this,"PackageName","TgcRawDataMonitor","group name for histograming"};
  StringProperty m_trigTagList{this,"TagTrigList","HLT_mu26_ivarmedium_L1MU20","list of triggers to be used for trigger matching"};
  BooleanProperty m_TagAndProbe{this,"TagAndProbe",true,"switch to perform tag-and-probe method"};
  BooleanProperty m_anaTgcPrd{this,"AnaTgcPrd",false,"switch to perform analysis on TGC PRD/Coin"};
  BooleanProperty m_anaOfflMuon{this,"AnaOfflMuon",true,"switch to perform analysis on xAOD::Muon"};
  BooleanProperty m_anaMuonRoI{this,"AnaMuonRoI",true,"switch to perform analysis on xAOD::LVL1MuonRoI"};
  StringProperty m_MuonEFContainerName{this,"MuonEFContainerName","HLT_MuonsCB_RoI","HLT RoI-based muon track container"};
  DoubleProperty m_trigMatchWindow{this,"TrigMatchingWindow",0.005,"Window size in R for trigger matching"};
  DoubleProperty m_l1trigMatchWindow1{this,"L1TrigMatchingWindow1",0.15,"Window size in R for L1 trigger matching: param 1"};
  DoubleProperty m_l1trigMatchWindow2{this,"L1TrigMatchingWindow2",0.3,"Window size in R for L1 trigger matching: param 2"};
  DoubleProperty m_l1trigMatchWindow3{this,"L1TrigMatchingWindow3",-0.01,"Window size in R for L1 trigger matching: param 3"};
  DoubleProperty m_l1trigMatchWindow4{this,"L1TrigMatchingWindow4",0.36,"Window size in R for L1 trigger matching: param 4"};
  DoubleProperty m_l1trigMatchWindow5{this,"L1TrigMatchingWindow5",-0.0016,"Window size in R for L1 trigger matching: param 5"};
  DoubleProperty m_isolationWindow{this,"IsolationWindow",0.1,"Window size in R for isolation with other muons"};
  DoubleProperty m_M1_Z{this,"M1_Z",13605.0,"z-position of TGC M1-station in mm for track extrapolate"};
  DoubleProperty m_M2_Z{this,"M2_Z",14860.0,"z-position of TGC M2-station in mm for track extrapolate"};
  DoubleProperty m_M3_Z{this,"M3_Z",15280.0,"z-position of TGC M3-station in mm for track extrapolate"};
  DoubleProperty m_EI_Z{this,"EI_Z",7425.0,"z-position of TGC EI-station in mm for track extrapolate"};
  DoubleProperty m_FI_Z{this,"FI_Z",7030.0,"z-position of TGC FI-station in mm for track extrapolate"};
  DoubleProperty m_muonMass{this,"MuonMass",105.6583755,"muon invariant mass in MeV"};
  DoubleProperty m_zMass{this,"ZMass",91187.6,"muon invariant mass in MeV"};
  DoubleProperty m_zMassWindow{this,"ZMassWindow",10000,"muon invariant mass half-window in MeV"};
  DoubleProperty m_endcapPivotPlaneMinimumRadius{this,"endcapPivotPlaneMinimumRadius",0.,"minimum radius of pivot plane in endcap region"};
  DoubleProperty m_endcapPivotPlaneMaximumRadius{this,"endcapPivotPlaneMaximumRadius", 11977.,"maximum radius of pivot plane in endcap region"};
  DoubleProperty m_barrelPivotPlaneHalfLength{this,"barrelPivotPlaneHalfLength", 9500.,"half length of pivot plane in barrel region"};
  
  std::vector<TagDef> m_trigTagDefs;
  std::vector<double> m_extZposition;
  
  StatusCode triggerMatching(const xAOD::Muon* , const std::vector<TagDef>& ) const;

  /* track extrapolator tool */
  enum TargetDetector { UNDEF, TGC, RPC };
  void extrapolate(const xAOD::Muon*, MyMuon&) const;
  bool extrapolate(const xAOD::TrackParticle* trackParticle,
		   const Amg::Vector3D& pos,
		   const int detector,
		   Amg::Vector2D& eta,
		   Amg::Vector2D& phi,
		   Amg::Vector3D& mom) const;
  const Trk::TrackParameters*
    extrapolateToTGC(const Trk::TrackStateOnSurface* tsos,
		     const Amg::Vector3D& pos,
		     Amg::Vector2D& distance) const;
  const Trk::TrackParameters*
    extrapolateToRPC(const Trk::TrackStateOnSurface* tsos,
		     const Amg::Vector3D& pos,
		     Amg::Vector2D& distance) const;
  double getError(const std::vector<double>& inputVec) const;
  ToolHandle<Trk::IExtrapolator> m_extrapolator{this,"TrackExtrapolator","Trk::Extrapolator/AtlasExtrapolator","Track extrapolator"};
};
#endif
