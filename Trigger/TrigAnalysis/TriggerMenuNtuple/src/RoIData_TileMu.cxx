/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**  @file RoIData_TileMu.cxx
*
*    authors: Akimasa Ishikawa (akimasa@phys.sci.kobe-u.ac.jp)
*/
#include "TriggerMenuNtuple/RoIData_TileMu.h"
#include <cmath>

RoIData_TileMu::RoIData_TileMu(int create) : 
  RoIData(create), 
  //TileMuChains(0),InDetChains(0),TileTrackMuChains(0),TrigMuonEFChains(0),
  TileMu_ChainNames(0), TileMu_ChainStatuses(0),
  InDet_ChainNames(0), InDet_ChainStatuses(0),
  TileTrackMu_ChainNames(0), TileTrackMu_ChainStatuses(0),
  MuonEF_ChainNames(0), MuonEF_ChainStatuses(0),
  TileMu(false), TileMu_eta(0.0), TileMu_phi(0.0), TileMu_quality(0.0),
  InDet(false), InDet_pt(0.0), InDet_eta(0.0), InDet_phi(0.0), 
  TileTrackMu(false), TileTrackMu_pt(0.0), TileTrackMu_eta(0.0), TileTrackMu_phi(0.0),
  MuonEF(false), MuonEF_pt(0.0), MuonEF_eta(0.0), MuonEF_phi(0.0) {

  TileMu_enedep.clear();

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
  MuonEFCB_nTRT=0;
}

RoIData_TileMu::~RoIData_TileMu() {
}

//void RoIData_TileMu::setTileMu(float eta, float phi, std::vector<float> enedep, float quality, std::vector<TrigStatus> status) {
void RoIData_TileMu::setTileMu(float eta, float phi, std::vector<float> enedep, float quality, std::vector<std::string>& name, std::vector<int>& status) {
  TileMu = true;
  TileMu_eta = eta;
  TileMu_phi = phi;
  TileMu_enedep = enedep;
  TileMu_quality = quality;
  //TileMuChains = status;
  TileMu_ChainNames = name;
  TileMu_ChainStatuses = status;
}

//void RoIData_TileMu::setInDet(float pt, float eta, float phi, std::vector<TrigStatus> status) {
void RoIData_TileMu::setInDet(float pt, float eta, float phi, std::vector<std::string>& name, std::vector<int>& status) {
  InDet = true;
  InDet_pt = pt;
  InDet_eta = eta;
  InDet_phi = phi;
  //InDetChains = status;
  InDet_ChainNames = name;
  InDet_ChainStatuses = status;
}

//void RoIData_TileMu::setTileTrackMu(float pt, float eta, float phi, std::vector<TrigStatus> status) {
void RoIData_TileMu::setTileTrackMu(float pt, float eta, float phi, std::vector<std::string>& name, std::vector<int>& status) {
  TileTrackMu = true;
  TileTrackMu_pt = pt;
  TileTrackMu_eta = eta;
  TileTrackMu_phi = phi;
  //TileTrackMuChains = status;
  TileTrackMu_ChainNames = name;
  TileTrackMu_ChainStatuses = status;
}


//void RoIData_TileMu::setMuonEF(float pt, float eta, float phi, std::vector<TrigStatus> status) {
void RoIData_TileMu::setMuonEF(float pt, float eta, float phi, std::vector<std::string>& name, std::vector<int>& status) {
  MuonEF = true;
  MuonEF_pt = pt;
  MuonEF_eta = eta;
  MuonEF_phi = phi;
  //TrigMuonEFChains = status;
  MuonEF_ChainNames = name;
  MuonEF_ChainStatuses = status;
}


bool RoIData_TileMu::l2exist() const { return TileTrackMu;}
bool RoIData_TileMu::efexist() const { return MuonEF;}

float RoIData_TileMu::l2pt() const { return fabs(TileTrackMu_pt)/1000;}
float RoIData_TileMu::efpt() const { return MuonEF_pt/1000;}

float RoIData_TileMu::l2eta() const { return TileTrackMu_eta;}
float RoIData_TileMu::efeta() const { return MuonEF_eta;}

float RoIData_TileMu::l2phi() const { return TileTrackMu_phi;}
float RoIData_TileMu::efphi() const { return MuonEF_phi;}


