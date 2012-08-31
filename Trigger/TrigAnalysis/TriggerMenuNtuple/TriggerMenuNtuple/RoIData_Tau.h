/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __RoIData_Tau_h__
#define __RoIData_Tau_h__
/*
 RoIData_Tau.h
*/
#include "TriggerMenuNtuple/RoIData.h"

class TauObjectsInRoI;

class RoIData_Tau : public RoIData {
public:
  RoIData_Tau(int create=0);
  virtual ~RoIData_Tau();

  void setTauCluster(float et, float eta, float phi, std::vector<TrigStatus> status);
  void setL2Tau(float pt, float eta, float phi, std::vector<TrigStatus> status);
  void setEFTau(float pt, float eta, float phi, std::vector<TrigStatus> status);

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

  std::vector<TrigStatus> L2TauClusterChains;
  std::vector<TrigStatus> L2TauChains;
  std::vector<TrigStatus> EFTauChains;

  bool TauCluster;
  float TauClusterEt;
  float TauClusterEta;
  float TauClusterPhi;

  bool L2Tau;
  float L2TauPt;
  float L2TauEta;
  float L2TauPhi;

  bool EFTau;
  float EFTauPt;
  float EFTauEta;
  float EFTauPhi;
};

//bool convertTauData(RoIData_Tau& out, const TauObjectsInRoI& in);

#endif // __RoIData_Tau_h__
