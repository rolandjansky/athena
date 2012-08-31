/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __RoIData_Jet_h__
#define __RoIData_Jet_h__
/*
 RoIData_Jet.h
*/
#include <vector>
#include "TriggerMenuNtuple/RoIData.h"

class JetObjectsInRoI;

class RoIData_Jet : public RoIData {
public:
  RoIData_Jet(int create=0);
  virtual ~RoIData_Jet();

  void setL2Jet(float et, float eta, float phi, std::vector<TrigStatus> status);
  void setEFJet(float et, float eta, float phi, std::vector<TrigStatus> status);

public:
  //
  bool isAssociatedL2(const std::string& chain_name) const;
  bool isAssociatedL2() const;
  bool isAssociatedEF(const std::string& chain_name) const;
  bool isAssociatedEF() const;

  bool l2exist() const;
  float l2pt() const;
  float l2eta() const;
  float l2phi() const;

  bool efexist() const;
  float efpt() const;
  float efeta() const;
  float efphi() const;
  //

  std::vector<TrigStatus> L2JetChains;
  std::vector<TrigStatus> EFJetChains;

  bool L2Jet;
  float L2JetEt;
  float L2JetEta;
  float L2JetPhi;

  bool EFJet;
  float EFJetEt;
  float EFJetEta;
  float EFJetPhi;
};

//bool convertJetData(RoIData_Jet& out, const JetObjectsInRoI& in);

#endif // __RoIData_Jet_h__
