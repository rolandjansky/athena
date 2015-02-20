/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONTOOLINTERFACES_TRIGMUONEFMONVARS_H
#define TRIGMUONTOOLINTERFACES_TRIGMUONEFMONVARS_H

#include <vector>

struct TrigMuonEFSegmentMonVars {
  std::vector<int>   numberOfStations;
  std::vector<int>   numberOfSegComb;
  std::vector<int>   numberOfSegs;
  std::vector<float> phi;
  std::vector<float> eta;
  std::vector<float> chi2pNdof;
  std::vector<int>   nMdt;
  std::vector<int>   nCsc;
  std::vector<int>   nRpc;
  std::vector<int>   nTgc;

  std::vector<float> mdtRadius;
  std::vector<int>   mdtTDC;
  std::vector<int>   mdtADC;
  std::vector<float> mdtZ;
  std::vector<float> mdtPerp;
  std::vector<float> mdtEta;

  float phiL2CosmicMuon;
  float etaL2CosmicMuon;

  float efPhi;
  float efEta;

  int wasCached;
};


struct TrigMuonEFMSMonVars {
  std::vector<int>   numberOfTracks;
  std::vector<float> charge;
  std::vector<float> chi2;
  std::vector<float> chi2Prob;
  std::vector<float> pt;
  std::vector<float> phi;
  std::vector<float> eta;
  std::vector<float> d0;
  std::vector<float> z0;
  std::vector<int>   nMdt;
  std::vector<int>   nRpcEta;
  std::vector<int>   nRpcPhi;
  std::vector<int>   nCscEta;
  std::vector<int>   nCscPhi;
  std::vector<int>   nTgcEta;
  std::vector<int>   nTgcPhi;
  std::vector<int>   nHit;
  int wasCached;
};

struct TrigMuonEFSAMonVars : public TrigMuonEFMSMonVars {
  // no additional items for now. May change in the future
};


struct TrigMuonEFCBMonVars : public TrigMuonEFSAMonVars {
  std::vector<int>   type; // TrigMuonEF=1,TrigMuGirl=2,TrigMuGirlTag=3
  std::vector<float> matchChi2;
  std::vector<int>   nSct;
  std::vector<int>   nPixel;
  std::vector<int>   nTrt;
  std::vector<int>   nIDTrks;
  std::vector<float> IDTrk_phi;
  std::vector<float> IDTrk_eta;
  std::vector<float> IDTrk_pT;
};

struct TrigMuonEFMonVars {
  TrigMuonEFSegmentMonVars segs; // Segment finding
  TrigMuonEFMSMonVars MS; // Muon Spectrometer
  TrigMuonEFSAMonVars SA; // Standalone, i.e. extrapolated to IP
  TrigMuonEFCBMonVars CB; // Combined (with Inner Detector)
};

struct TrigMuonCaloTagMonVars {
  std::vector<int>   numberOfTracks;
  std::vector<int>   nHit;
  std::vector<int>   nSct;
  std::vector<int>   nPixel;
  std::vector<int>   nTrt;
  std::vector<int>   nIDTrks;
  std::vector<int>   IDTrk_CaloTag;
  std::vector<float> charge;
  std::vector<float> d0;
  std::vector<float> z0;
  std::vector<float> IDTrk_phi;
  std::vector<float> IDTrk_eta;
  std::vector<float> IDTrk_pT;
  std::vector<float> IDTrk_CaloLH;
};

struct TrigMuGirlMonVars : public TrigMuonEFMSMonVars {
  std::vector<float> cotTh;
  std::vector<int> nTgcRpcPhi;
  std::vector<int> nTgcRpcEta;
  std::vector<int> nMdtInseg;
  std::vector<int> nRpcInseg;
  std::vector<int> nTgcInseg;
  std::vector<int> nTgcRpcInseg;
  std::vector<int> nMdtSegs;
  std::vector<int> nRpcSegs;
  std::vector<int> nCscSegs;
  std::vector<int> nTgcSegs;
  std::vector<int> nTgcRpcSegs;
  std::vector<float> beta;
  std::vector<float> nnBarrel;
  std::vector<float> nnEndcap;
  std::vector<float> rpcHitsMeasTime;
  std::vector<float> rpcHitsPropTime;
  std::vector<float> rpcHitsTof;
  std::vector<float> rpcHitsDis;
};

#endif // TRIGMUONTOOLINTERFACES_TRIGMUONEFMONVARS_H
