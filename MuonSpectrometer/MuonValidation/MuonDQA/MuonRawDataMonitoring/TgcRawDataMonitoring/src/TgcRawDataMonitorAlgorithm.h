/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCRAWDATAMONITORING_TGCRAWDATAMONITORALGORITHM_H
#define TGCRAWDATAMONITORING_TGCRAWDATAMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadDecorHandleKey.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTrigger/MuonRoIContainer.h"
#include "MuonTrigCoinData/TgcCoinDataContainer.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "MuonAnalysisInterfaces/IMuonSelectionTool.h"
#include "ITgcRawDataMonitorTool.h"
#include "MuonPrepRawData/TgcPrepDataContainer.h"
#include "TrigConfData/L1Menu.h"
#include <memory>
#include <vector>
#include <set>

class TgcRawDataMonitorAlgorithm : public AthMonitorAlgorithm {
 public:
  TgcRawDataMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TgcRawDataMonitorAlgorithm()=default;
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;
  
  struct MyMuon{
    const xAOD::Muon* muon{};
    std::vector<double> extPosZ;
    std::vector<TVector3> extPos;
    std::vector<TVector3> extVec;
    std::set<int> matchedL1ThrExclusive;
    std::set<int> matchedL1ThrInclusive;
    std::set<std::string> matchedL1Items;
    bool matchedL1Charge{};
    bool passBW3Coin{};
    bool passInnerCoin{};
    bool passGoodMF{};
    bool passIsMoreCandInRoI{};
    void clear(){
      extPosZ.clear();
      extPos.clear();
      extVec.clear();
      matchedL1ThrExclusive.clear();
      matchedL1ThrInclusive.clear();
      matchedL1Items.clear();
    }
  };
  struct ExtPos{
    double extPosZ{};
    Amg::Vector3D extPos;
    Amg::Vector3D extVec;
    int matched{};
    const xAOD::Muon* muon{};
    std::set<std::string> passedChambers;
    std::set<std::string> chambersHasHit;
  };
  struct TgcTrig{
    int lb{};
    double x_In{};
    double y_In{};
    double z_In{};
    double x_Out{};
    double y_Out{};
    double z_Out{};
    double eta{};
    double phi{};
    double etain{};
    double etaout{};
    double width_In{};
    double width_Out{};
    double width_R{};
    double width_Phi{};
    int isAside{};
    int isForward{};
    int isStrip{};
    int isInner{};
    int isPositiveDeltaR{};
    int type{};
    int trackletId{};
    int trackletIdStrip{};
    int sector{};
    int roi{};
    int pt{};
    int delta{};
    int sub{};
    int veto{};
    int bunch{};
    int inner{};
  };
  struct CtpDecMonObj{
    std::string trigItem;
    std::string title;
    long unsigned int multiplicity{};
    int rpcThr{};
    int tgcThr{};
    int sys{};//system: 1 for barrel, 2 for endcap, 3 for forward
    int threshold{};
    int charge{};
    bool tgcF{}; // full-station flag
    bool tgcC{}; // inner-coincidence flag
    bool tgcH{}; // hot roi mask flag
    bool rpcR{}; // masking feet trigger
    bool rpcM{}; // isMoreCand
    double eta{};
    double phi{};
    unsigned int roiWord{};
  };
  
 private:
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

  SG::ReadHandleKey<TrigConf::L1Menu> m_L1MenuKey {this, "L1TriggerMenu", "DetectorStore+L1TriggerMenu","L1 Menu key"};
  SG::ReadDecorHandleKey<xAOD::MuonRoIContainer> m_thresholdPatternsKey{this,"MuRoIThresholdPatternsKey","LVL1MuonRoIs.thresholdPatterns","Name of the muon RoI container decoration for the threshold patterns"};
  BooleanProperty m_monitorThresholdPatterns{this,"MonitorThresholdPatterns",true,"start monitoring tirgger threshold patterns"};
  StringProperty m_thrPatternList{this,"ThrPatternList","MU4,MU6,MU10,MU11,MU20,MU21","list of single L1MU items to be monitored by the threshold pattern"};

  SG::ReadHandleKey<xAOD::MuonContainer> m_MuonContainerKey{this,"MuonContainerName","Muons","Offline muon track container"};
  SG::ReadHandleKey<xAOD::MuonRoIContainer> m_MuonRoIContainerKey{this,"MuonRoIContainerName","LVL1MuonRoIs","L1 muon RoI container"};
  
  SG::ReadHandleKey<Muon::TgcPrepDataContainer> m_TgcPrepDataContainerKey{this,"TgcPrepDataContainerName","TGC_MeasurementsAllBCs","current BC TGC PRD"};
  SG::ReadHandleKey<Muon::TgcCoinDataContainer> m_TgcCoinDataContainerCurrBCKey{this,"TgcCoinDataContainerCurrBCName","TrigT1CoinDataCollection","TGC Coin Data Container CurrBC"};
  SG::ReadHandleKey<Muon::TgcCoinDataContainer> m_TgcCoinDataContainerNextBCKey{this,"TgcCoinDataContainerNextBCName","TrigT1CoinDataCollectionNextBC","TGC Coin Data Container NextBC"};
  SG::ReadHandleKey<Muon::TgcCoinDataContainer> m_TgcCoinDataContainerPrevBCKey{this,"TgcCoinDataContainerPrevBCName","TrigT1CoinDataCollectionPriorBC","TGC Coin Data Container PrevBC"};
  
  StringProperty m_packageName{this,"PackageName","TgcRawDataMonitor","group name for histograming"};
  StringProperty m_ctpDecMonList{this,"CtpDecisionMoniorList","Tit:L1_2MU4,Mul:2,HLT:HLT_2mu4,RPC:1,TGC:1;","list of L1MU items to be monitored for before/after CTP decision"};
  BooleanProperty m_monitorTriggerMultiplicity{this,"MonitorTriggerMultiplicity",false,"start monitoring tirgger multiplicity performance"};
  BooleanProperty m_printAvailableMuonTriggers{this,"PrintAvailableMuonTriggers",false,"debugging purpose. print out all available muon triggers in the event"};
  BooleanProperty m_TagAndProbe{this,"TagAndProbe",true,"switch to perform tag-and-probe method"};
  BooleanProperty m_TagAndProbeZmumu{this,"TagAndProbeZmumu",false,"switch to perform tag-and-probe method Z->mumu"};
  BooleanProperty m_anaTgcPrd{this,"AnaTgcPrd",false,"switch to perform analysis on TGC PRD"};
  BooleanProperty m_anaTgcCoin{this,"AnaTgcCoin",false,"switch to perform analysis on TGC Coin"};
  BooleanProperty m_fillGapByGapHistograms{this,"FillGapByGapHistograms",true,"filling gap-by-gap histograms (many many)"};
  BooleanProperty m_anaOfflMuon{this,"AnaOfflMuon",true,"switch to perform analysis on xAOD::Muon"};
  BooleanProperty m_offlMuonCutOnMuonType{this,"OfflMuonCutOnMuonType",true,"applying cut on offline muon muonType"};
  BooleanProperty m_offlMuonCutOnAuthor{this,"OfflMuonCutOnAuthor",true,"applying cut on offline muon author"};
  BooleanProperty m_offlMuonCutOnQuality{this,"OfflMuonCutOnQuality",true,"applying cut on offline muon quality"};
  BooleanProperty m_anaMuonRoI{this,"AnaMuonRoI",true,"switch to perform analysis on xAOD::LVL1MuonRoI"};
  DoubleProperty m_trigMatchWindow{this,"TrigMatchingWindow",0.005,"Window size in R for trigger matching"};
  DoubleProperty m_l1trigMatchWindow1{this,"L1TrigMatchingWindow1",0.15,"Window size in R for L1 trigger matching: param 1"};
  DoubleProperty m_l1trigMatchWindow2{this,"L1TrigMatchingWindow2",0.3,"Window size in R for L1 trigger matching: param 2"};
  DoubleProperty m_l1trigMatchWindow3{this,"L1TrigMatchingWindow3",-0.01,"Window size in R for L1 trigger matching: param 3"};
  DoubleProperty m_l1trigMatchWindow4{this,"L1TrigMatchingWindow4",0.36,"Window size in R for L1 trigger matching: param 4"};
  DoubleProperty m_l1trigMatchWindow5{this,"L1TrigMatchingWindow5",-0.0016,"Window size in R for L1 trigger matching: param 5"};
  DoubleProperty m_isolationWindow{this,"IsolationWindow",1.0,"Window size in R for isolation with other muons"};
  BooleanProperty m_requireIsolated{this,"RequireIsolated",true,"Probe muon should be isolated from other muons"};
  BooleanProperty m_useIDTrackForExtrapolation{this,"UseIDTrackForExtrapolation",false,"Use InnerDetectorTrackParticle for extrapolation"};
  BooleanProperty m_useMSTrackForExtrapolation{this,"UseMSTrackForExtrapolation",false,"Use MuonSpectrometerTrackParticle for extrapolation"};
  BooleanProperty m_useCBTrackForExtrapolation{this,"UseCBTrackForExtrapolation",false,"CombinedTrackParticle for extrapolation"};
  BooleanProperty m_useExtMSTrackForExtrapolation{this,"UseExtMSTrackForExtrapolation",false,"Use ExtrapolatedMuonSpectrometerTrackParticle for extrapolation"};
  BooleanProperty m_useMSOnlyExtMSTrackForExtrapolation{this,"UseMSOnlyExtMSTrackForExtrapolation",false,"Use MSOnlyExtrapolatedMuonSpectrometerTrackParticle for extrapolation"};
  BooleanProperty m_useDirectPrimaryTrackForExtrapolation{this,"UseDirectPrimaryTrackForExtrapolation",true,"Use DirectPrimaryTrackForExtrapolation for extrapolation"};
  BooleanProperty m_useOnlyCombinedMuons{this,"UseOnlyCombinedMuons",false,"use only CombinedMuons"};
  BooleanProperty m_useOnlyMuidCoStacoMuons{this,"UseOnlyMuidCoStacoMuons",false,"use only MuidCo and Staco Muons"};
  BooleanProperty m_useMuonSelectorTool{this,"UseMuonSelectorTool",true,"use MuonSelectorTool"};
  DoubleProperty m_pTCutOnExtrapolation{this,"pTCutOnExtrapolation",6000.,"pT [in MeV] cut on the extrapolation tracks"}; 
  DoubleProperty m_M1_Z{this,"M1_Z",13436.5,"z-position of TGC M1-station in mm for track extrapolate"}; 
  DoubleProperty m_M2_Z{this,"M2_Z",14728.2,"z-position of TGC M2-station in mm for track extrapolate"};
  DoubleProperty m_M3_Z{this,"M3_Z",15148.2,"z-position of TGC M3-station in mm for track extrapolate"};
  DoubleProperty m_EI_Z{this,"EI_Z",7364.7,"z-position of TGC EI-station in mm for track extrapolate"};
  DoubleProperty m_FI_Z{this,"FI_Z",6978.2,"z-position of TGC FI-station in mm for track extrapolate"};
  DoubleProperty m_muonMass{this,"MuonMass",105.6583755,"muon invariant mass in MeV"};
  DoubleProperty m_zMass{this,"ZMass",91187.6,"muon invariant mass in MeV"};
  DoubleProperty m_zMassWindow{this,"ZMassWindow",10000,"muon invariant mass half-window in MeV"};
  DoubleProperty m_endcapPivotPlaneMinimumRadius{this,"endcapPivotPlaneMinimumRadius",0.,"minimum radius of pivot plane in endcap region"};
  DoubleProperty m_endcapPivotPlaneMaximumRadius{this,"endcapPivotPlaneMaximumRadius", 11977.,"maximum radius of pivot plane in endcap region"};
  DoubleProperty m_barrelPivotPlaneHalfLength{this,"barrelPivotPlaneHalfLength", 9500.,"half length of pivot plane in barrel region"};
  DoubleProperty m_residualWindow{this,"ResidualWindow", 200.,"Window size in mm between hit position and track-extrapolated position"};
  DoubleProperty m_dPhiCutOnM3{this,"dPhiCutOnM3", 0.2,"Window size in delta phi on M3 between hit position and track-extrapolated position"};
  DoubleProperty m_dRCutOnM3{this,"dRCutOnM3", 1000.,"Window size in delta R (radious) on M3 between hit position and track-extrapolated position"};
  
  std::vector<double> m_extZposition;
  std::vector<CtpDecMonObj> m_CtpDecMonObj;
  std::set<std::string> m_thrMonList;

  using MonVariables=std::vector < std::reference_wrapper < Monitored::IMonitoredVariable >>;
  void fillTgcCoin(const std::string&,
		   const std::vector<TgcTrig>&, 
		   std::vector<Monitored::ObjectsCollection<std::vector<TgcTrig>, double>>&,
		   MonVariables&) const;
  void fillTgcCoinEff(const std::string&,
		      const std::vector<TgcTrig>&, 
		      const std::vector<ExtPos>&,
		      std::vector<double>&,std::vector<double>&,std::vector<double>&,
		      std::vector<Monitored::ObjectsCollection<std::vector<double>, double>>&,
		      MonVariables&) const;
  
  ToolHandle<Trk::IExtrapolator> m_extrapolator{this,"TrackExtrapolator","Trk::Extrapolator/AtlasExtrapolator","Track extrapolator"};
  ToolHandle<ITgcRawDataMonitorTool> m_tgcMonTool{this,"TgcRawDataMonitorTool","TgcDawDataMonitorTool","TgcRawDataMonitorTool"};
  ToolHandle<CP::IMuonSelectionTool> m_muonSelectionTool{this,"MuonSelectionTool","CP::MuonSelectionTool/MuonSelectionTool","MuonSelectionTool"};
  SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey {this, "DetectorManagerKey","MuonDetectorManager","Key of input MuonDetectorManager condition data"}; 

};
#endif
