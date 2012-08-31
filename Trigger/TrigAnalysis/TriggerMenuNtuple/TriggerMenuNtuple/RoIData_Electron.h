/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __RoIData_Electron_h__
#define __RoIData_Electron_h__
/*
 RoIData_Electron.h
*/
#include "TriggerMenuNtuple/RoIData.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"

class ElectronObjectsInRoI;

class RoIData_Electron : public RoIData {
public:
  RoIData_Electron(int create=0);
  virtual ~RoIData_Electron();

  void setEMCluster(float e, float et, float eta, float phi, std::vector<TrigStatus> status);
  void setL2ID(std::vector<TrigStatus> status);
  void setL2Electron(int charge, float ethad, float eratio , float reta, 
				       float deta, float dphi, float etoverpt, float trackpt,
				       TrigInDetTrack::AlgoId trackalgo, std::vector<TrigStatus> status);
  void setEFegamma(float pt, float eta, float phi, std::vector<TrigStatus> status,
		   float e, float et, float rphi, float e233, float e237, float e277,
		   float f1, float emins, float fside, float emax, float ws3,
		   float stot, float weta, float ethad, float ethad1, int charge);
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

  std::vector<TrigStatus> L2CaloChains;
  std::vector<TrigStatus> L2IDChains;
  std::vector<TrigStatus> L2ElectronChains;
  std::vector<TrigStatus> EFegammaChains;

  bool EMCluster;
  float EMClusterEt;
  float EMClusterEta;
  float EMClusterPhi;

  bool EFegamma;
  float EFegammaPt;
  float EFegammaEta;
  float EFegammaPhi;



  /********** CommonD3PDProject Electron ***********/

  /*** TrigCaloEvent ***/
  float L2_E;
  float L2_Et;
  float L2_eta;
  float L2_phi;

  /*** TrigElectron ***/
  int L2_charge;
  float L2_Ethad1;
  float L2_Eratio;
  float L2_reta;
  float L2_trackclusterdeta;
  float L2_trackclusterdphi;
  float L2_Etoverpt;
  float L2_trackpt;
  TrigInDetTrack::AlgoId L2_trackalgo;

  /*** egammaContainer ***/
  float EF_E;
  float EF_Et;
  float EF_eta;
  float EF_phi;
  int EF_charge;
  float EF_Ethad1;               //EMShower
  float EF_Ethad;
  float EF_rphiallcalo;
  float EF_E233;
  float EF_E237;
  float EF_E277;
  float EF_f1;
  float EF_Emins1;
  float EF_fside;
  float EF_Emax2;
  float EF_ws3;
  float EF_stot;
  float EF_weta2;

  bool L2CaloChainsIsPassed();
  bool L2IDChainsIsPassed();
  bool L2ElectronChainsIsPassed();
  bool EFegammaChainsIsPassed();

  bool L2CaloChainsIsAssociated();
  bool L2IDChainsIsAssociated();
  bool L2ElectronChainsIsAssociated();
  bool EFegammaChainsIsAssociated();

  /*************************************************/

};

//bool convertElectronData(RoIData_Electron& out, const ElectronObjectsInRoI& in);

#endif // __RoIData_Electron_h__
