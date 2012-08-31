/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __RoIData_Muon_h__
#define __RoIData_Muon_h__
/*
  RoIData_Muon.h
*/
#include <vector>
#include <stdint.h>
#include "TriggerMenuNtuple/RoIData.h"

class MuonObjectsInRoI;

class Muon_ROI;
class MuonFeature;
class CombinedMuonFeature;
class IsoMuonFeature;
//class TrigMuonEFContainer;
//class TrigMuonEF;
class TrigMuonEFInfoContainer;
//class TrigMuonEF;

class RoIData_Muon : public RoIData {
public:
  RoIData_Muon(int create=0);
  virtual ~RoIData_Muon();

  /*
  void setMuFast(float pt, float eta, float phi, std::vector<TrigStatus> status);
  void setInDet(float pt, float eta, float phi, std::vector<TrigStatus> status);
  void setMuComb(float pt, float eta, float phi, std::vector<TrigStatus> status);
  void setMuIso(float pt, float eta, float phi, std::vector<TrigStatus> status);
  void setMuonEF(float pt, float eta, float phi, std::vector<TrigStatus> status);
  */

  void setMuFast(float pt, float eta, float phi, std::vector<std::string>& name, std::vector<int>& status);
  void setInDet(float pt, float eta, float phi, std::vector<std::string>& name, std::vector<int>& status);
  void setMuComb(float pt, float eta, float phi, std::vector<std::string>& name, std::vector<int>& status);
  void setMuIso(float pt, float eta, float phi, std::vector<std::string>& name, std::vector<int>& status);
  void setMuonEF(float pt, float eta, float phi, std::vector<std::string>& name, std::vector<int>& status);

public:
  //
  bool l2exist() const;
  float l2pt() const;
  float l2eta() const;
  float l2phi() const;

  bool efexist() const;
  float efpt() const;
  float efeta() const;
  float efphi() const;
  //

  //L1 RoI
  int L1_RoISource;//0:Barrel, 1:Endcap, 2:Forward
  int L1_ThrNumber;
  int L1_RoINumber;
  int L1_SectorAddress;//full sector address
  bool L1_isAside;
  bool L1_isFirstCandidate;
  bool L1_isMoreCandInRoI;
  bool L1_isMoreCandInSector;

  float L1_pt;
  float L1_eta;
  float L1_phi;

  //muFast
  bool MuFast;
  float MuFast_pt;
  float MuFast_eta;
  float MuFast_phi;
  //muFast extended variables
  int MuFast_algoID;
  int MuFast_RoIID;
  int MuFast_sAddress;
  float MuFast_radius;
  float MuFast_dirPhi;
  float MuFast_zeta;
  float MuFast_dirZeta;
  float MuFast_beta;
  float MuFast_sp1R;
  float MuFast_sp1Z;
  float MuFast_sp1Slope;
  float MuFast_sp2R;
  float MuFast_sp2Z;
  float MuFast_sp2Slope;
  float MuFast_sp3R;
  float MuFast_sp3Z;
  float MuFast_sp3Slope;
  float MuFast_brRadius;
  float MuFast_brSagitta;
  float MuFast_ECAlpha;
  float MuFast_ECBeta;
  //muFast MuonFeatureDetails variables
  //uint32_t MuFast_RoIID;// this is in MuonFeature
  uint32_t MuFast_RoIThreshold;
  float MuFast_RoIEta;
  float MuFast_RoIPhi;

  float MuFast_RPC1x;
  float MuFast_RPC1y;
  float MuFast_RPC1z;
  float MuFast_RPC2x;
  float MuFast_RPC2y;
  float MuFast_RPC2z;
  float MuFast_RPC3x;
  float MuFast_RPC3y;
  float MuFast_RPC3z;

  float MuFast_TGCMid1Eta;
  float MuFast_TGCMid1Phi;
  float MuFast_TGCMid1R;
  float MuFast_TGCMid1Z;
  float MuFast_TGCMid2Eta;
  float MuFast_TGCMid2Phi;
  float MuFast_TGCMid2R;
  float MuFast_TGCMid2Z;
  float MuFast_TGCMidRhoChi2;
  long  MuFast_TGCMidRhoN;
  float MuFast_TGCMidPhiChi2;
  long  MuFast_TGCMidPhiN;
  float MuFast_TGCInnEta;
  float MuFast_TGCInnPhi;
  float MuFast_TGCInnR;
  float MuFast_TGCInnZ;
  float MuFast_TGCInnRhoStd;
  long  MuFast_TGCInnRhoN;
  float MuFast_TGCInnPhiStd;
  long  MuFast_TGCInnPhiN;
  float MuFast_TGCPt;
  float MuFast_ExtrInnerEta;
  float MuFast_ExtrInnerPhi;

  float MuFast_MDTInnerFitChi;
  float MuFast_MDTMiddleFitChi;
  float MuFast_MDTOuterFitChi;

  float MuFast_PhiMap;

  //InDet
  bool InDet;
  float InDet_pt;
  float InDet_eta;
  float InDet_phi;

  //muComb
  bool MuComb;
  float MuComb_pt;
  float MuComb_eta;
  float MuComb_phi;
  //muComb extended variables
  float MuComb_sigmaPt;
  //muComb associated ID track variables
  int   MuComb_ID_algo;
  float MuComb_ID_pt;
  float MuComb_ID_ept;
  float MuComb_ID_eta;
  float MuComb_ID_eeta;
  float MuComb_ID_phi0;
  float MuComb_ID_ephi0;
  float MuComb_ID_z0;
  float MuComb_ID_ez0;
  float MuComb_ID_a0;
  float MuComb_ID_ea0;
  float MuComb_ID_chi2;
  int   MuComb_ID_nPIX;
  int   MuComb_ID_nSCT;
  int   MuComb_ID_hitPattern; // Hit pattern of SCT space points
  int   MuComb_ID_nStrawHits; // Number of TRT hits associated with Track
  int   MuComb_ID_nStraw;     // Number of TRT straws intersected by track
  int   MuComb_ID_nStrawTime; // Number of TRT starws with valid drift time intersected by track
  int   MuComb_ID_nTRHits;        // Number of high-threshold TRT hits associated with track

  //L2 ID tracks within the cone dR<0.3 and pt>1GeV/c
  std::vector<int>   ID_algo;
  std::vector<float> ID_pt;
  std::vector<float> ID_ept;
  std::vector<float> ID_eta;
  std::vector<float> ID_eeta;
  std::vector<float> ID_phi0;
  std::vector<float> ID_ephi0;
  std::vector<float> ID_z0;
  std::vector<float> ID_ez0;
  std::vector<float> ID_a0;
  std::vector<float> ID_ea0;
  std::vector<float> ID_chi2;
  std::vector<int>   ID_nPIX;
  std::vector<int>   ID_nSCT;
  std::vector<int>   ID_hitPattern; // Hit pattern of SCT space points
  std::vector<int>   ID_nStrawHits; // Number of TRT hits associated with Track
  std::vector<int>   ID_nStraw;     // Number of TRT straws intersected by track
  std::vector<int>   ID_nStrawTime; // Number of TRT starws with valid drift time intersected by track
  std::vector<int>   ID_nTRHits;        // Number of high-threshold TRT hits associated with track



  //muIso
  bool MuIso;
  float MuIso_pt;
  float MuIso_eta;
  float MuIso_phi;

  //muIso extended variables
  float MuIso_EtICEC;
  float MuIso_EtOCEC;
  float MuIso_EtICHC;
  float MuIso_EtOCHC;

  int MuIso_nTracks;
  float MuIso_sumPtTracks;
  float MuIso_ptMuTracks;

  int MuIso_RoIId;
  float MuIso_LArWeight;
  float MuIso_TileWeight;

  

  //MuonEF
  bool MuonEF;
  float MuonEF_pt;
  float MuonEF_eta;
  float MuonEF_phi;

  //MuonEF extended variables
  int MuonEF_nTracks;
  int MuonEF_nSegments;
  int MuonEF_nMDT;
  int MuonEF_nRPC;
  int MuonEF_nTGC;
  int MuonEF_nCSC;

  //MS
  float MuonEFMS_pt;
  float MuonEFMS_eta;
  float MuonEFMS_phi;
  float MuonEFMS_chi2;
  float MuonEFMS_chi2prob;

  float MuonEFMS_d0;
  float MuonEFMS_z0;

  //int MuonEFMS_nMDT;
  int MuonEFMS_nCSCeta;
  int MuonEFMS_nCSCphi;
  int MuonEFMS_nRPCeta;
  int MuonEFMS_nRPCphi;
  int MuonEFMS_nTGCeta;
  int MuonEFMS_nTGCphi;

  //SA
  float MuonEFSA_pt;
  float MuonEFSA_eta;
  float MuonEFSA_phi;
  float MuonEFSA_chi2;
  float MuonEFSA_chi2prob;

  float MuonEFSA_d0;
  float MuonEFSA_z0;

  //CB
  float MuonEFCB_pt;
  float MuonEFCB_eta;
  float MuonEFCB_phi;
  float MuonEFCB_chi2;
  float MuonEFCB_chi2prob;

  float MuonEFCB_d0;
  float MuonEFCB_z0;

  float MuonEFCB_matchChi2;
  int MuonEFCB_nSCT;
  int MuonEFCB_nPIX;
  int MuonEFCB_nTRT;

  /*
  std::vector<TrigStatus> MuFastChains;
  std::vector<TrigStatus> InDetChains;
  std::vector<TrigStatus> MuCombChains;
  std::vector<TrigStatus> MuIsoChains;
  std::vector<TrigStatus> TrigMuonEFChains;
  */

  std::vector<std::string> MuFast_ChainNames;
  std::vector<int>         MuFast_ChainStatuses;
  std::vector<std::string> InDet_ChainNames;
  std::vector<int>         InDet_ChainStatuses;
  std::vector<std::string> MuComb_ChainNames;
  std::vector<int>         MuComb_ChainStatuses;
  std::vector<std::string> MuIso_ChainNames;
  std::vector<int>         MuIso_ChainStatuses;
  std::vector<std::string> MuonEF_ChainNames;
  std::vector<int>         MuonEF_ChainStatuses;



};

//bool convertMuonData(RoIData_Muon& out, const MuonObjectsInRoI& in);

#endif // __RoIData_Muon_h__
