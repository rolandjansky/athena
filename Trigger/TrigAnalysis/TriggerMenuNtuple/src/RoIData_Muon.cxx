/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  RoIData_Muon.cxx
*/
#include <cmath>
#include "TriggerMenuNtuple/RoIData_Muon.h"
#include "TrigMuonEvent/TrigMuonEFTrack.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"

using namespace std;


RoIData_Muon::RoIData_Muon(int create) : 
  RoIData(create), 
  MuFast(false), MuFast_pt(0.0), MuFast_eta(0.0), MuFast_phi(0.0), 
  InDet(false), InDet_pt(0.0), InDet_eta(0.0), InDet_phi(0.0), 
  MuComb(false), MuComb_pt(0.0), MuComb_eta(0.0), MuComb_phi(0.0),
  MuIso(false), MuIso_pt(0.0), MuIso_eta(0.0), MuIso_phi(0.0),
  MuonEF(false), MuonEF_pt(0.0), MuonEF_eta(0.0), MuonEF_phi(0.0),
  //MuFastChains(0),InDetChains(0),MuCombChains(0),MuIsoChains(0),TrigMuonEFChains(0){
  MuFast_ChainNames(0), MuFast_ChainStatuses(0),
  InDet_ChainNames(0), InDet_ChainStatuses(0),
  MuComb_ChainNames(0), MuComb_ChainStatuses(0),
  MuIso_ChainNames(0), MuIso_ChainStatuses(0),
  MuonEF_ChainNames(0), MuonEF_ChainStatuses(0){

  //L1 RoI
  L1_RoISource = 0;//0:Barrel, 1:Endcap, 2:Forward
  L1_ThrNumber = 0;
  L1_RoINumber = 0;
  L1_SectorAddress = 0;//full sector address
  L1_isAside = false;
  L1_isFirstCandidate = false;
  L1_isMoreCandInRoI = false;
  L1_isMoreCandInSector = false;

  L1_pt = 0;
  L1_eta = 0;
  L1_phi = 0;

  //muFast extended variables
  MuFast_algoID = 0;
  MuFast_RoIID = 0;
  MuFast_sAddress = 0;
  MuFast_radius = 0;
  MuFast_dirPhi = 0;
  MuFast_zeta = 0;
  MuFast_dirZeta = 0;
  MuFast_beta = 0;
  MuFast_sp1R = 0;
  MuFast_sp1Z = 0;
  MuFast_sp1Slope = 0;
  MuFast_sp2R = 0;
  MuFast_sp2Z = 0;
  MuFast_sp2Slope = 0;
  MuFast_sp3R = 0;
  MuFast_sp3Z = 0;
  MuFast_sp3Slope = 0;
  MuFast_brRadius = 0;
  MuFast_brSagitta = 0;
  MuFast_ECAlpha = 0;
  MuFast_ECBeta = 0;

  //muFast MuonFeatureDetails variables
  MuFast_RoIThreshold = 0;
  MuFast_RoIEta = 0;
  MuFast_RoIPhi = 0;

  MuFast_RPC1x = 0;
  MuFast_RPC1y = 0;
  MuFast_RPC1z = 0;
  MuFast_RPC2x = 0;
  MuFast_RPC2y = 0;
  MuFast_RPC2z = 0;
  MuFast_RPC3x = 0;
  MuFast_RPC3y = 0;
  MuFast_RPC3z = 0;

  MuFast_TGCMid1Eta = 0;
  MuFast_TGCMid1Phi = 0;
  MuFast_TGCMid1R = 0;
  MuFast_TGCMid1Z = 0;
  MuFast_TGCMid2Eta = 0;
  MuFast_TGCMid2Phi = 0;
  MuFast_TGCMid2R = 0;
  MuFast_TGCMid2Z = 0;
  MuFast_TGCMidRhoChi2 = 0;
  MuFast_TGCMidRhoN = 0;
  MuFast_TGCMidPhiChi2 = 0;
  MuFast_TGCMidPhiN = 0;
  MuFast_TGCInnEta = 0;
  MuFast_TGCInnPhi = 0;
  MuFast_TGCInnR = 0;
  MuFast_TGCInnZ = 0;
  MuFast_TGCInnRhoStd = 0;
  MuFast_TGCInnRhoN = 0;
  MuFast_TGCInnPhiStd = 0;
  MuFast_TGCInnPhiN = 0;
  MuFast_TGCPt = 0;
  MuFast_ExtrInnerEta = 0;
  MuFast_ExtrInnerPhi = 0;

  MuFast_MDTInnerFitChi = 0;
  MuFast_MDTMiddleFitChi = 0;
  MuFast_MDTOuterFitChi = 0;

  MuFast_PhiMap = 0;


  //muComb
  MuComb_sigmaPt=0.0;
  MuComb_ID_algo=0.0;
  MuComb_ID_pt=0.0;
  MuComb_ID_ept=0.0;
  MuComb_ID_eta=0.0;
  MuComb_ID_eeta=0.0;
  MuComb_ID_phi0=0.0;
  MuComb_ID_ephi0=0.0;
  MuComb_ID_z0=0.0;
  MuComb_ID_ez0=0.0;
  MuComb_ID_a0=0.0;
  MuComb_ID_ea0=0.0;
  MuComb_ID_chi2=0.0;
  MuComb_ID_nPIX=0;
  MuComb_ID_nSCT=0;
  MuComb_ID_hitPattern=0;
  MuComb_ID_nStrawHits=0;
  MuComb_ID_nStraw=0;
  MuComb_ID_nStrawTime=0;
  MuComb_ID_nTRHits=0;

  //L2IDtracks
  ID_algo.clear();
  ID_pt.clear();
  ID_ept.clear();
  ID_eta.clear();
  ID_eeta.clear();
  ID_phi0.clear();
  ID_ephi0.clear();
  ID_z0.clear();
  ID_ez0.clear();
  ID_a0.clear();
  ID_ea0.clear();
  ID_chi2.clear();
  ID_nPIX.clear();
  ID_nSCT.clear();
  ID_hitPattern.clear();
  ID_nStrawHits.clear();
  ID_nStraw.clear();
  ID_nStrawTime.clear();
  ID_nTRHits.clear();

  //muIso

  MuIso_EtICEC = 0;
  MuIso_EtOCEC = 0;
  MuIso_EtICHC = 0;
  MuIso_EtOCHC = 0;

  MuIso_nTracks = 0;
  MuIso_sumPtTracks = 0;
  MuIso_ptMuTracks = 0;

  MuIso_RoIId = 0;
  MuIso_LArWeight = 0;
  MuIso_TileWeight = 0;

  //muIso extended variables
  MuIso_EtICEC = 0;
  MuIso_EtOCEC = 0;
  MuIso_EtICHC = 0;
  MuIso_EtOCHC = 0;

  MuIso_nTracks = 0;
  MuIso_sumPtTracks = 0;
  MuIso_ptMuTracks = 0;

  MuIso_RoIId = 0;
  MuIso_LArWeight = 0;
  MuIso_TileWeight = 0;

  //MuonEF extended variables
  MuonEF_nTracks = 0;
  MuonEF_nSegments = 0;
  MuonEF_nMDT = 0;
  MuonEF_nRPC = 0;
  MuonEF_nTGC = 0;
  MuonEF_nCSC = 0;

  //MS
  MuonEFMS_pt = 0;
  MuonEFMS_eta = 0;
  MuonEFMS_phi = 0;
  MuonEFMS_chi2 = 0;
  MuonEFMS_chi2prob = 0;

  MuonEFMS_d0 = 0;
  MuonEFMS_z0 = 0;

  //MuonEFMS_nMDT = 0;
  MuonEFMS_nCSCeta = 0;
  MuonEFMS_nCSCphi = 0;
  MuonEFMS_nRPCeta = 0;
  MuonEFMS_nRPCphi = 0;
  MuonEFMS_nTGCeta = 0;
  MuonEFMS_nTGCphi = 0;

  //SA
  MuonEFSA_pt = 0;
  MuonEFSA_eta = 0;
  MuonEFSA_phi = 0;
  MuonEFSA_chi2 = 0;
  MuonEFSA_chi2prob = 0;

  MuonEFSA_d0 = 0;
  MuonEFSA_z0 = 0;

  //CB
  MuonEFCB_pt = 0;
  MuonEFCB_eta = 0;
  MuonEFCB_phi = 0;
  MuonEFCB_chi2 = 0;
  MuonEFCB_chi2prob = 0;

  MuonEFCB_d0 = 0;
  MuonEFCB_z0 = 0;

  MuonEFCB_matchChi2 = 0;
  MuonEFCB_nSCT = 0;
  MuonEFCB_nPIX = 0;
  MuonEFCB_nTRT = 0;
}

RoIData_Muon::~RoIData_Muon() {
}

//void RoIData_Muon::setMuFast(float pt, float eta, float phi, std::vector<TrigStatus> status) {
void RoIData_Muon::setMuFast(float pt, float eta, float phi, std::vector<std::string>& name, std::vector<int>& status) {
  MuFast = true;
  MuFast_pt = pt;
  MuFast_eta = eta;
  MuFast_phi = phi;
  //MuFastChains = status;
  MuFast_ChainNames = name;
  MuFast_ChainStatuses = status;
}

//void RoIData_Muon::setInDet(float pt, float eta, float phi, std::vector<TrigStatus> status) {
void RoIData_Muon::setInDet(float pt, float eta, float phi, std::vector<std::string>& name, std::vector<int>& status) {
  InDet = true;
  InDet_pt = pt;
  InDet_eta = eta;
  InDet_phi = phi;
  //InDetChains = status;
  InDet_ChainNames = name;
  InDet_ChainStatuses = status;
}

//void RoIData_Muon::setMuComb(float pt, float eta, float phi, std::vector<TrigStatus> status) {
void RoIData_Muon::setMuComb(float pt, float eta, float phi, std::vector<std::string>& name, std::vector<int>& status) {
  MuComb = true;
  MuComb_pt = pt;
  MuComb_eta = eta;
  MuComb_phi = phi;
  //MuCombChains = status;
  MuComb_ChainNames = name;
  MuComb_ChainStatuses = status;
}

//void RoIData_Muon::setMuIso(float pt, float eta, float phi, std::vector<TrigStatus> status) {
void RoIData_Muon::setMuIso(float pt, float eta, float phi, std::vector<std::string>& name, std::vector<int>& status) {
  MuIso = true;
  MuIso_pt = pt;
  MuIso_eta = eta;
  MuIso_phi = phi;
  //MuIsoChains = status;
  MuIso_ChainNames = name;
  MuIso_ChainStatuses = status;
}

//void RoIData_Muon::setMuonEF(float pt, float eta, float phi, std::vector<TrigStatus> status) {
void RoIData_Muon::setMuonEF(float pt, float eta, float phi, std::vector<std::string>& name, std::vector<int>& status) {
  MuonEF = true;
  MuonEF_pt = pt;
  MuonEF_eta = eta;
  MuonEF_phi = phi;
  //TrigMuonEFChains = status;
  MuonEF_ChainNames = name;
  MuonEF_ChainStatuses = status;
}


bool RoIData_Muon::l2exist() const { return MuComb;}
bool RoIData_Muon::efexist() const { return MuonEF;}

float RoIData_Muon::l2pt() const { return fabs(MuComb_pt)/1000;}
float RoIData_Muon::efpt() const { return MuonEF_pt/1000;}

float RoIData_Muon::l2eta() const { return MuComb_eta;}
float RoIData_Muon::efeta() const { return MuonEF_eta;}

float RoIData_Muon::l2phi() const { return MuComb_phi;}
float RoIData_Muon::efphi() const { return MuonEF_phi;}


