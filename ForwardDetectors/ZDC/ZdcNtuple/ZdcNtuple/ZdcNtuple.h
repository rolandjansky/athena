/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZdcNtuple_ZdcNtuple_H
#define ZdcNtuple_ZdcNtuple_H

//#include <EventLoop/Algorithm.h>

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <AsgTools/AnaToolHandle.h>

//#include "TrigDecisionTool/TrigDecisionTool.h"
//#include "TrigDecisionTool/ChainGroup.h"
//#include "TrigConfxAOD/xAODConfigTool.h"

#include <TrigConfInterfaces/ITrigConfigTool.h>
#include <TrigDecisionTool/TrigDecisionTool.h>
#include <TriggerMatchingTool/IMatchingTool.h>

#include "xAODEventInfo/EventInfo.h"
//#include "GoodRunsLists/GoodRunsListSelectionTool.h"
#include <AsgAnalysisInterfaces/IGoodRunsListSelectionTool.h>
#include <TH1.h>
#include <TH2.h>
#include "xAODForward/MBTSModuleContainer.h"
#include "xAODForward/ZdcModuleContainer.h"
#include "xAODForward/ForwardEventInfoContainer.h"
#include <TTree.h>
#include <xAODHIEvent/HIEventShapeContainer.h>
#include <xAODTracking/VertexContainer.h>
#include <xAODTracking/TrackParticleContainer.h>
#include <xAODTrigger/TrigDecision.h>
#include "xAODTrigger/MuonRoIContainer.h"
#include <xAODTrigger/EnergySumRoI.h>
#include <xAODTrigL1Calo/TriggerTowerContainer.h>

#include <xAODTrigMinBias/TrigT2MbtsBitsContainer.h>
#include <xAODJet/JetContainer.h>
//#include "JetSelectorTools/JetCleaningTool.h"
#include <JetInterface/IJetSelector.h>

#include "xAODCaloEvent/CaloClusterContainer.h"
//#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"

#include "xAODMuon/MuonContainer.h"
#include <MuonAnalysisInterfaces/IMuonSelectionTool.h>

//#include "MuonSelectorTools/MuonSelectionTool.h"
//#include "MuonMomentumCorrections/MuonCalibrationAndSmearingTool.h"
#include <MuonAnalysisInterfaces/IMuonCalibrationAndSmearingTool.h>
//#include "MuonEfficiencyCorrections/MuonEfficiencyScaleFactors.h"
//#include "TrigMuonMatching/TrigMuonMatching.h"
#include "xAODForward/MBTSModuleContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "PATInterfaces/SystematicVariation.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicCode.h"
//#include "xAODBTaggingEfficiency/BTaggingSelectionTool.h"

#include "xAODEgamma/PhotonContainer.h"
//#include "ElectronPhotonSelectorTools/AsgPhotonIsEMSelector.h"
//#include "EgammaAnalysisInterfaces/IAsgPhotonIsEMSelector.h"
//#include "TrigEgammaMatchingTool/TrigEgammaMatchingTool.h"

#include "xAODEgamma/ElectronContainer.h"
//#include "ElectronPhotonSelectorTools/AsgElectronLikelihoodTool.h"
//#include "ElectronPhotonSelectorTools/AsgElectronIsEMSelector.h"
#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"
#include "EgammaAnalysisInterfaces/IAsgElectronIsEMSelector.h"
#include "IsolationSelection/IIsolationSelectionTool.h"

#include "boost/unordered_map.hpp"

//#include "HIEventUtils/HIVertexSelectionTool.h"
//#include "HIEventUtils/MBTSTimeSelectionTool.h"
//#include "HIEventUtils/ZdcRecTool.h"
//#include "ZdcAnalysis/ZdcAnalysisTool.h"
#include "ZdcAnalysis/IZdcAnalysisTool.h"

class ZdcNtuple : public EL::AnaAlgorithm
{
  // put your configuration variables here as public variables.
  // that way they can be set directly from CINT and python.
public:
  // float cutValue;
  bool slimmed; // assume slimmed output
  bool useGRL; // use a GRL (used by default)
  std::string grlFilename;
  bool enableOutputTree; // enable output TTree
  bool enableOutputSamples; // write samples into TTree
  bool enableTrigger; // use trigger info (skip this for laser runs)
  bool writeOnlyTriggers; // only write passed triggers to ntuple
  bool enableClusters; // store tracks
  bool enableTracks; // store clusters
  bool enableMuons; // store muons in ntuple
  bool enableElectrons; // store electrons in ntuple
  bool enablePhotons; // store photons in ntuple
  bool enableTT; // store trigger towers
  bool enableTruth; // store truth info
  bool enableJets; // store jets
  bool enableTriggerJets; // store trigger jets in ntuple
  bool zdcCalib; // run calibration file
  bool zdcLaser; // run laser file
  bool zdcLowGainOnly; // ZDC LG only
  size_t trackLimit; // max tracks for writing tree
  bool trackLimitReject;
  bool flipDelay; // whether or not to flip un/delay for EM ZDC modules
  bool reprocZdc; // use ZdcAnalysisTool to reprocess ZDC, otherwise use what is there
  std::string auxSuffix; // what to add to name the new data, when reprocessing
  size_t nsamplesZdc; // nsamples expected by ZDC tool
  bool hion4; // derivation (limited variables)
  bool upc2015;
  bool upcL2015;
  bool mb2015;
  bool main2016A;
  bool upc2016A;
  bool express2016A;
  bool main2016B;
  bool upc2016B; // p+Pb
  bool upc2016C; // Pb+p
  bool upc2018;
  bool express2016B;
  bool mboverlay2016;

  bool doZdcCalib;
  std::string zdcConfig;

  std::string outputName; // output for tree
  int outputTreeScaledown; // scaledown factor
public:

  // Tools
  asg::AnaToolHandle<TrigConf::ITrigConfigTool> m_trigConfigTool;
  asg::AnaToolHandle<Trig::TrigDecisionTool>  m_trigDecisionTool;
  asg::AnaToolHandle<Trig::IMatchingTool> m_trigMatchingTool;

  asg::AnaToolHandle<IGoodRunsListSelectionTool> m_grl;
  asg::AnaToolHandle<IGoodRunsListSelectionTool> m_grl_mb;

  asg::AnaToolHandle<IJetSelector> m_jetCleaning;
  //BTaggingSelectionTool* m_btagtool; // b tag tool
  //BTaggingSelectionTool* m_btagtool_tj; // track jet
  //asg::AnaToolHandle<IIsolationSelectionTool> m_isoTool;// isolation tool
  //std::vector<InDet::InDetTrackSelectionTool*> m_trackSelectionTools;
  asg::AnaToolHandle<CP::IMuonSelectionTool> m_muonSelection;
  asg::AnaToolHandle<CP::IMuonCalibrationAndSmearingTool> m_muonCalibrationAndSmearingTool;

  //Trig::TrigMuonMatching* m_muonTrigMatchTool;
  /*
  CP::MuonEfficiencyScaleFactors* m_muonEfficiencyScaleFactors;
  CP::SystematicSet m_sysup;
  CP::SystematicSet m_sysdown;
  CP::SystematicSet m_statup;
  CP::SystematicSet m_statdown;
  CP::SystematicSet m_sysupLowpT;
  CP::SystematicSet m_sysdownLowpT;
  CP::SystematicSet m_statupLowpT;
  CP::SystematicSet m_statdownLowpT;
  */

  asg::AnaToolHandle<IAsgElectronIsEMSelector> m_electronLooseEMSelector;
  asg::AnaToolHandle<IAsgElectronIsEMSelector> m_electronMediumEMSelector;
  asg::AnaToolHandle<IAsgElectronLikelihoodTool> m_electronLooseLHSelector;
  asg::AnaToolHandle<IAsgElectronLikelihoodTool> m_electronMediumLHSelector;
  //asg::AnaToolHandle<IAsgPhotonIsEMSelector> m_photonLooseIsEMSelector;
  //asg::AnaToolHandle<IAsgPhotonIsEMSelector> m_photonTightIsEMSelector;
  //Trig::TrigEgammaMatchingTool* m_trigEgammaMatchingTool;

  //HI::HIVertexSelectionTool* m_hiVertexSelectionTool;
  //HI::MBTSTimeSelectionTool* m_hiMBTSTimeSelectionTool;
  //ZDC::ZdcRecTool* m_zdcRecTool;

  asg::AnaToolHandle<ZDC::IZdcAnalysisTool> m_zdcAnalysisTool;

  const xAOD::EventInfo* m_eventInfo;
  // end of Tools

  // Containers
  const xAOD::HIEventShapeContainer *m_caloSums;
  const xAOD::HIEventShapeContainer *m_eventShapes;
  const xAOD::HIEventShapeContainer *m_TTeventShapes;
  const xAOD::ZdcModuleContainer* m_zdcModules;
  const xAOD::ZdcModuleContainer* m_zdcSums;
  const xAOD::ForwardEventInfoContainer* m_mbtsInfo;
  const xAOD::MBTSModuleContainer* m_mbtsModules;
  const xAOD::TrigT2MbtsBitsContainer* m_trigT2MbtsBits;
  const xAOD::VertexContainer* m_primaryVertices;
  const xAOD::CaloClusterContainer* m_caloClusters;
  const xAOD::TrackParticleContainer* m_trackParticles;
  const xAOD::MuonContainer* m_muons;
  const xAOD::TrackParticleContainer* m_extrapolatedMuonTrackParticles;
  const xAOD::PhotonContainer* m_photons;
  const xAOD::ElectronContainer* m_electrons;
  const xAOD::TrigDecision* m_trigDecision;
  const xAOD::MuonRoIContainer* m_muonRoIContainer;
  const xAOD::EnergySumRoI* m_lvl1EnergySumRoI;
  const xAOD::TruthParticleContainer* m_truthParticleContainer;
  const xAOD::TriggerTowerContainer* m_TTcontainer;

  std::vector<std::string> m_jetContainerNames;
  std::vector<const xAOD::JetContainer*> m_jetContainers;
  std::vector<bool> m_jetCleaningFlag;

  std::vector< CP::SystematicSet > muonSysList;

  //const xAOD::JetContainer* m_AntiKt4HIJets;
  //const xAOD::JetContainer* m_TrigAntiKt4HIJets;

  int m_nTriggers;
  int m_eventCounter;
  bool m_isMC;
  bool m_setupTrigHist;
  int m_scaledownCounter;
  std::vector<const Trig::ChainGroup*> m_chainGroups;
  std::vector<const Trig::ChainGroup*> m_rerunChainGroups;

  std::vector<std::string> m_el_triggers;
  std::vector<std::string> m_ph_triggers;
  std::vector<std::string> m_muon_triggers;
  std::vector<std::string> m_dimuon_triggers;

  float t_L1ET;
  float t_L1ET24;

  // Histograms
  void setupTriggerHistos();
  TH1* h_zdcTriggers;
  TH1* h_zdcTriggersTBP;

  // containers for histogram vectors
  std::vector<TH1*> h_zdc_LG_mod; // low gain for each module
  std::vector<TH1*> h_zdc_HG_mod; // high gain for each module
  std::vector<TH1*> h_zdc_LG_reproc_mod; // low gain for each module
  std::vector<TH1*> h_zdc_HG_reproc_mod; // high gain for each module
  std::vector<TH1*> h_zdc_LG; // low gain for arm
  std::vector<TH1*> h_zdc_HG; // high gain for arm
  std::vector<TH1*> h_zdc_LG_reproc; // low gain for arm
  std::vector<TH1*> h_zdc_HG_reproc; // high gain for arm

  std::vector<TH2*> h_zdc_LG_corr;
  std::vector<TH2*> h_zdc_LG_reproc_corr;
  std::vector<TH2*> h_zdc_HG_corr;
  std::vector<TH2*> h_zdc_HG_reproc_corr;

  TTree* m_outputTree;
  uint32_t t_runNumber;
  uint32_t t_eventNumber;
  uint32_t t_lumiBlock;
  uint32_t t_bcid;
  uint32_t t_passBits;
  float t_avgIntPerCrossing;
  float t_actIntPerCrossing;

  uint64_t t_trigger;
  uint32_t t_trigger_TBP;
  //std::vector<uint16_t> t_decisions;
  //std::vector<float> t_prescales;
  float t_prescales[200];
  bool t_decisions[200];
  bool t_rerunDecisions[200];

  //bool t_vertexToolDecision;
  //bool t_mbtsToolDecision;

  float t_mbts_in_e[2][8];
  float t_mbts_out_e[2][4];
  float t_mbts_in_t[2][8];
  float t_mbts_out_t[2][4];

  float t_T2mbts_in_e[2][8];
  float t_T2mbts_out_e[2][4];
  float t_T2mbts_in_t[2][8];
  float t_T2mbts_out_t[2][4];

  uint32_t t_tav[16];
  uint32_t t_tbp[16];

  float t_amp[2][4];
  float t_amp_rp[2][4];
  float t_time[2][4];
  uint16_t t_raw7[2][4][2][2][7];
  uint16_t t_raw15[2][4][2][2][15];
  float t_ampHG[2][4];
  float t_ampLG[2][4];
  float t_sumHG[2];
  float t_sumLG[2];
  float t_ampHG_rp[2][4];
  float t_ampLG_rp[2][4];
  float t_sumHG_rp[2];
  float t_sumLG_rp[2];

  float t_ZdcAmp[2];
  float t_ZdcAmpErr[2];
  float t_ZdcEnergy[2];
  float t_ZdcEnergyErr[2];
  float t_ZdcTime[2];
  short t_ZdcStatus[2];
  unsigned int t_ZdcModuleMask;
  float t_ZdcTrigEff[2];

  float t_ZdcModuleAmp[2][4];
  float t_ZdcModuleTime[2][4];
  float t_ZdcModuleFitAmp[2][4];
  float t_ZdcModuleFitT0[2][4];
  float t_ZdcModuleChisq[2][4];
  unsigned int t_ZdcModuleStatus[2][4];
  float t_ZdcModuleCalibAmp[2][4];
  float t_ZdcModuleCalibTime[2][4];
  float t_ZdcModuleAmpError[2][4];
  float t_ZdcModuleBkgdMaxFraction[2][4];
  float t_ZdcModuleMinDeriv2nd[2][4];
  float t_ZdcModulePresample[2][4];
  float t_ZdcModulePreSampleAmp[2][4];

  int   t_nvx;
  float t_vx[3];
  int t_vxntrk;
  std::vector<int> t_vx_trk_index;
  int t_vxtype;
  int t_pvindex;
  float t_vxcov[6];
  float t_vxsumpt2;
  float t_puvxz;
  int t_puvxntrk;
  float t_puvxsumpt;
  int t_nstrong;
  int t_vxnlooseprimary;
  int t_vxnminbias;
  int t_vxngoodmuon;

  int t_nvtx;
  std::vector<int8_t> t_vtx_type;
  std::vector<float> t_vtx_x;
  std::vector<float> t_vtx_y;
  std::vector<float> t_vtx_z;
  std::vector<int16_t> t_vtx_ntrk_all;
  std::vector<float> t_vtx_sumpt2_all;
  std::vector<int16_t> t_vtx_ntrk;
  std::vector<float> t_vtx_sumpt2;
  std::vector< std::vector<int16_t> > t_vtx_trk_index;

  // FCal
  float t_fcalEt;
  float t_fcalEtA;
  float t_fcalEtC;
  float t_fcalEtA_TT;
  float t_fcalEtC_TT;
  float t_fcalEtA_TTsum;
  float t_fcalEtC_TTsum;

  float t_totalEt;
  float t_totalEt_TTsum;
  float t_totalEt24;
  float t_totalEt24_TTsum;

  // gaps
  float t_edgeGapA;
  float t_edgeGapC;
  float m_gapPtMin;

  float m_gapThresholds[98] =
    {4.7426,5.11018,5.07498,5.0969,5.10695,5.04098,5.07106,4.98087,5.11647,5.08988,5.16267,
     5.17202,5.23803,5.25314,5.29551,5.35092,5.40863,5.44375,5.38075,5.25022,5.37933,5.25459,5.37719,5.25169,5.73985,
     5.79174,5.79266,5.79588,5.7963,5.81949,5.82273,5.85658,5.85442,5.84779,5.77679,5.83323,5.84524,5.84439,5.84488,
     5.84744,5.84683,5.84524,5.84594,5.84656,5.84639,5.84461,5.84515,5.84206,5.8396,5.84497,5.84801,5.84608,5.84608,
     5.84783,5.84726,5.84844,5.8477,5.84796,5.84757,5.84822,5.84814,5.84617,5.83451,5.77658,5.84309,5.85496,5.85761,
     5.82555,5.82206,5.78982,5.78482,5.7778,5.78327,5.74898,5.25459,5.37503,5.25459,5.37283,5.25169,5.37862,5.44473,
     5.41041,5.34498,5.29551,5.25602,5.2283,5.17428,5.14504,5.09342,5.12256,4.98721,5.07106,5.02642,5.10031,5.11018,
     5.05447,5.10031,4.7426};
  TH1* h_TCSigCut;

  // mbts
  uint16_t t_mbts_countA;
  uint16_t t_mbts_countC;
  float t_mbts_timeA;
  float t_mbts_timeC;
  float t_mbts_timeDiff;

  uint16_t t_T2mbts_countAin;
  uint16_t t_T2mbts_countCin;

  // jets - a vector for each container
  std::vector<int> t_njet;
  std::vector< std::vector<float>> t_jet_pt;
  std::vector< std::vector<float>> t_jet_eta;
  std::vector< std::vector<float>> t_jet_phi;
  std::vector< std::vector<float>> t_jet_m;
  std::vector< std::vector<uint8_t>> t_jet_acc;

  // Tjets
  /*
  int t_nTjet;
  std::vector<float> t_Tjet_pt;
  std::vector<float> t_Tjet_eta;
  std::vector<float> t_Tjet_phi;
  std::vector<float> t_Tjet_m;
  std::vector<uint8_t> t_Tjet_acc;
  */

  // photons
  int t_nphoton;
  std::vector<uint8_t> t_ph_author;
  std::vector<uint8_t> t_ph_conv;
  std::vector<uint32_t> t_ph_oq;
  std::vector<float> t_ph_pt;
  std::vector<float> t_ph_eta;
  std::vector<float> t_ph_eta2;
  std::vector<float> t_ph_phi;
  std::vector<float> t_ph_e;
  std::vector<uint32_t> t_ph_isEMTight;
  std::vector<uint32_t> t_ph_isEMLoose;
  std::vector<uint8_t> t_ph_tight;
  std::vector<uint8_t> t_ph_loose;
  std::vector<bool> t_ph_iso;
  std::vector<float> t_ph_etcone40;
  std::vector<float> t_ph_topoetcone40;
  std::vector<std::vector<bool>> t_ph_HLT_match;
  std::vector<std::vector<float>> t_ph_HLT_dR;

  // electrons
  int t_nelectron;
  std::vector<uint8_t> t_el_author;
  std::vector<uint32_t> t_el_oq;
  std::vector<int8_t> t_el_charge;
  std::vector<float> t_el_pt;
  std::vector<float> t_el_eta;
  std::vector<float> t_el_eta2;
  std::vector<float> t_el_phi;
  std::vector<float> t_el_e;
  std::vector<float> t_el_trackpt;
  std::vector<int> t_el_trk_index;
  std::vector<float> t_el_d0;
  std::vector<float> t_el_sigmad0;
  std::vector<float> t_el_z0;
  std::vector<float> t_el_vz;
  std::vector<float> t_el_vtxz;
  std::vector<float> t_el_theta;
  std::vector<uint32_t> t_el_isEMMedium;
  std::vector<uint32_t> t_el_isEMLoose;
  std::vector<uint8_t> t_el_medium;
  std::vector<uint8_t> t_el_loose;
  std::vector<uint8_t> t_el_lhmedium;
  std::vector<uint8_t> t_el_lhloose;
  std::vector<float> t_el_etcone20;
  std::vector<float> t_el_topoetcone20;
  std::vector<bool> t_el_iso;
  std::vector<std::vector<bool>> t_el_HLT_match;
  std::vector<std::vector<float>> t_el_HLT_dR;

  // muons
  int t_nmuon;
  std::vector<float> t_muon_pt;
  std::vector<float> t_muon_eta;
  std::vector<float> t_muon_phi;
  std::vector<float> t_muon_m;
  std::vector<int8_t> t_muon_charge;
  std::vector<int8_t> t_muon_author;
  std::vector<uint8_t> t_muon_acc;
  std::vector<uint8_t> t_muon_qual;
  std::vector<int8_t> t_muon_l1match;
  std::vector<float> t_muon_l1dR;
  std::vector<float> t_muon_cluster_E;
  std::vector<float> t_muon_cluster_t;
  std::vector<float> t_muon_MS_pt;
  std::vector<float> t_muon_MS_pt_orig;
  std::vector<float> t_muon_MS_eta;
  std::vector<float> t_muon_MS_phi;
  std::vector<float> t_muon_ID_pt;
  std::vector<int> t_muon_ID_index;
  std::vector<bool> t_muon_iso;
  std::vector<std::vector<bool>> t_muon_HLT_match;
  std::vector<std::vector<float>> t_muon_HLT_dR;
  std::vector<float> t_muon_MSEx_pt;
  std::vector<float> t_muon_MSEx_eta;
  std::vector<float> t_muon_MSEx_phi;
  std::vector<uint8_t> t_muon_assocPV;
  std::vector<uint8_t> t_muon_nPrecision;
  std::vector<uint8_t> t_muon_nPrecisionHoles;
  std::vector<uint8_t> t_muon_hasTrack;
  std::vector<uint8_t> t_muon_nPixHits;
  std::vector<uint8_t> t_muon_nPixDead;
  std::vector<uint8_t> t_muon_nPixHoles;
  std::vector<uint8_t> t_muon_nSctHits;
  std::vector<uint8_t> t_muon_nSctDead;
  std::vector<uint8_t> t_muon_nSctHoles;
  std::vector<uint8_t> t_muon_nTrtHits;
  std::vector<uint8_t> t_muon_nTrtOutliers;
  std::vector<float> t_muon_d0;
  std::vector<float> t_muon_z0;
  std::vector<float> t_muon_vtxz;

  std::vector<float> t_muon_theta;

  std::vector<float> t_muon_rho;
  std::vector<float> t_muon_qOverPsigma;
  std::vector<float> t_muon_qOverPsignif;
  std::vector<float> t_muon_reducedChi2;
  std::vector<int8_t> t_muon_type;

  std::vector<float> t_muon_eff;
  std::vector<float> t_muon_sf;
  std::vector<std::vector<float> > t_muon_sfvar;
  std::vector<std::vector<float> > t_muon_sfvarLowpT;

  std::vector<std::vector<float> > t_muon_smear_pt; // ID,MS,momentum
  std::vector<std::vector<int8_t> > t_muon_smear_charge; // ID,MS,momentum
  std::vector<std::vector<float> > t_muon_smear_ID; // ID,MS,momentum
  std::vector<std::vector<float> > t_muon_smear_MS; // ID,MS,momentum

  const double m_L1MuonMatchdR = 1.0; // cross check this number

  // dimuon info
  int t_ndimuon;
  std::vector<int8_t> t_dimuon_mu0;
  std::vector<int8_t> t_dimuon_mu1;
  std::vector<std::vector<bool>> t_dimuon_HLT_match;
  std::vector<float> t_dimuon_m;
  std::vector<float> t_dimuon_rap;
  std::vector<float> t_dimuon_phi;
  std::vector<float> t_dimuon_pt;

  // extrapolated muons
  int t_nmsex;
  std::vector<float> t_msex_eta;
  std::vector<float> t_msex_phi;
  std::vector<float> t_msex_pt;
  std::vector<float> t_msex_m;
  std::vector<int8_t> t_msex_charge;
  std::vector<uint8_t> t_msex_nPrecision;
  std::vector<uint8_t> t_msex_nPrecisionHoles;

  // muon ROI
  int t_nmuroi;
  std::vector<float> t_muroi_phi;
  std::vector<float> t_muroi_eta;
  std::vector<float> t_muroi_thr;


  // tracks
  uint32_t t_ntrk;
  std::vector<float> t_trk_pt;
  std::vector<float> t_trk_eta;
  std::vector<float> t_trk_phi;
  std::vector<float> t_trk_e;
  std::vector<float> t_trk_theta;
  std::vector<float> t_trk_d0;
  std::vector<float> t_trk_z0;
  std::vector<float> t_trk_vz;
  std::vector<float> t_trk_vtxz;
  std::vector<int8_t> t_trk_charge;
  std::vector<int16_t> t_trk_quality;
  std::vector<int> t_trk_index;
  std::vector<uint8_t> t_trk_nPixHits;
  std::vector<uint8_t> t_trk_nSctHits;
  std::vector<uint8_t> t_trk_nPixDead;
  std::vector<uint8_t> t_trk_nSctDead;
  std::vector<uint8_t> t_trk_nPixHoles;
  std::vector<uint8_t> t_trk_nSctHoles;
  std::vector<uint8_t> t_trk_nTrtHits;
  std::vector<uint8_t> t_trk_nTrtOutliers;
  std::vector<uint8_t> t_trk_inPixHits;
  std::vector<uint8_t> t_trk_exPixHits;
  std::vector<uint8_t> t_trk_ninPixHits;
  std::vector<uint8_t> t_trk_nexPixHits;
  std::vector<float> t_trk_pixeldEdx;

  // clusters
  uint32_t t_nclus;
  std::vector<float> t_cc_pt;
  std::vector<float> t_cc_eta;
  std::vector<float> t_cc_phi;
  std::vector<float> t_cc_e;
  std::vector<float> t_cc_sig;
  std::vector<int> t_cc_layer;
  std::vector<float> t_cc_raw_m;
  std::vector<float> t_cc_raw_eta;
  std::vector<float> t_cc_raw_phi;
  std::vector<float> t_cc_raw_e;
  std::vector<std::vector<float>> t_cc_raw_samp;

  float t_clusEt;
  float t_clusEtMax;
  float t_clusetaMax;
  float t_clusphiMax;

  // trigger towers
  uint32_t t_ntt;
  std::vector<float> t_tt_pt;
  std::vector<float> t_tt_eta;
  std::vector<float> t_tt_phi;
  std::vector<float> t_tt_e;
  std::vector<uint8_t> t_tt_samp;

  // truth muons
  uint32_t t_ntrmu;
  std::vector<float> t_trmu_pt;
  std::vector<float> t_trmu_eta;
  std::vector<float> t_trmu_phi;
  std::vector<float> t_trmu_m;
  std::vector<float> t_trmu_vx;
  std::vector<float> t_trmu_vy;
  std::vector<float> t_trmu_vz;
  std::vector<int> t_trmu_pdgId;
  std::vector<int> t_trmu_barcode;
  std::vector<int> t_trmu_status;
  std::vector<int> t_trmu_muindex;
  std::vector<float> t_trmu_muDr;
  std::vector<int> t_trmu_L1MUindex;
  std::vector<float> t_trmu_L1MUDr;

  // end of Histograms

  // this is a standard constructor
  ZdcNtuple (const std::string& name, ISvcLocator* pSvcLocator);

  void processEventInfo();
  bool processTriggerDecision();
  uint32_t acceptEvent();
  void processModules();
  //void processZdcNtuple(); // old version directly from ZdcAnalysisTool
  void processZdcNtupleFromModules(); // new version directly from output of ZdcAnalysisTool - which is much cleaner
  void processFCal();
  void processMBTS();
  void processInDet();
  void writeTrack(const xAOD::TrackParticle*, const xAOD::Vertex* vertex, int );
  int trackQuality(const xAOD::TrackParticle* tp, const xAOD::Vertex* vertex);
  void processClusters();
  void reprocessZdcModule(const xAOD::ZdcModule* zdcMod, bool flipdelay=0);
  void processJets();
  int processMuons();
  void processMuonVertex(xAOD::MuonContainer* muons);
  void processTruthMuons();
  void processPhotons();
  void processElectrons();
  void processTriggerTowers();
  void processGaps();

  double dR(const double eta1,     const double phi1,     const double eta2,     const double phi2);


  virtual StatusCode initialize () override;
  virtual StatusCode execute () override;
  virtual StatusCode finalize () override;
};

#endif
