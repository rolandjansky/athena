/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**  @file RoIData_TileMu.h
*
*    authors: Akimasa Ishikawa (akimasa@phys.sci.kobe-u.ac.jp)
*/
#ifndef __RoIData_TileMu_h__
#define __RoIData_TileMu_h__

#include <vector>
#include "TriggerMenuNtuple/RoIData.h"

class TileMuObjectsInRoI;
class TileMuFeature;
class TileTrackMuFeature;
class TrigMuonEFInfoContainer;

class RoIData_TileMu : public RoIData 
{
public:
  RoIData_TileMu(int create=0);
  virtual ~RoIData_TileMu();

  /*
  void setTileMu(float eta, float phi, std::vector<float> enedep, float quality, std::vector<TrigStatus> status);
  void setInDet(float pt, float eta, float phi, std::vector<TrigStatus> status);
  void setTileTrackMu(float pt, float eta, float phi, std::vector<TrigStatus> status);
  void setMuonEF(float pt, float eta, float phi, std::vector<TrigStatus> status);
  */

  void setTileMu(float eta, float phi, std::vector<float> enedep, float quality, std::vector<std::string>& name, std::vector<int>& status);
  void setInDet(float pt, float eta, float phi, std::vector<std::string>& name, std::vector<int>& status);
  void setTileTrackMu(float pt, float eta, float phi, std::vector<std::string>& name, std::vector<int>& status);
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

  /*
  std::vector<TrigStatus> TileMuChains;
  std::vector<TrigStatus> InDetChains;
  std::vector<TrigStatus> TileTrackMuChains;
  std::vector<TrigStatus> TrigMuonEFChains;
  */

  std::vector<std::string> TileMu_ChainNames;
  std::vector<int>         TileMu_ChainStatuses;
  std::vector<std::string> InDet_ChainNames;
  std::vector<int>         InDet_ChainStatuses;
  std::vector<std::string> TileTrackMu_ChainNames;
  std::vector<int>         TileTrackMu_ChainStatuses;
  std::vector<std::string> MuonEF_ChainNames;
  std::vector<int>         MuonEF_ChainStatuses;

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

  //TileMu
  bool TileMu;
  float TileMu_eta;
  float TileMu_phi;
  std::vector<float> TileMu_enedep;
  float TileMu_quality;

  //InDet
  bool InDet;
  float InDet_pt;
  float InDet_eta;
  float InDet_phi;

  //TileTrackMu
  bool TileTrackMu;
  float TileTrackMu_pt;
  float TileTrackMu_eta;
  float TileTrackMu_phi;


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

};

// bool convertTileMuData(RoIData_TileMu& out, const TileMuObjectsInRoI& in);

// void setMuonEF(RoIData_TileMu& m, const TrigMuonEFInfoContainer* c, std::vector<TrigStatus> status);

#endif // __RoIData_TileMu_h__
