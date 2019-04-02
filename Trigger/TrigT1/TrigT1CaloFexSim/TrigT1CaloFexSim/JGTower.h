/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOFEXSIM_JGTOWER_H
#define TRIGT1CALOFEXSIM_JGTOWER_H 
/*
* Class   : JGTower
*
* Author  : Lin, Chiao-Ying (cylin@cern.ch)
*
* Date    : May 2017 - Created class
*
* This class is built for JGTower algorithm in topology
* including mapping to SC, eta/phi and granularity
* 
*/

#include <vector>
#include <TMath.h>
#include <TString.h>
#include <TObjArray.h>
#include <TLorentzVector.h>

class JGTower{

 public:

  // constructors
  JGTower();
  JGTower(float eta, float deta, float phi, float dphi);

  // destructor
  virtual ~JGTower();

  float Eta() const {return m_eta;};
  float Phi() const {return m_phi;};
  float dEta() const {return m_deta;};
  float dPhi() const {return m_dphi;};
  float sampling() const {return m_sampling;};
  std::vector<int> GetSCIndices() const {return m_scellIndices;};
  std::vector<int> GetTileIndices() const {return m_tileIndices;};
  void SetSCIndices(int sc_id)  {m_scellIndices.push_back(sc_id);};
  void SetTileIndices(int tile_id)  {m_tileIndices.push_back(tile_id);};
  void SetSampling(int sampling) {m_sampling=sampling;};
 private:

  float m_eta;
  float m_deta;
  float m_phi;
  float m_dphi;
  int m_sampling;
  std::vector<int> m_scellIndices;
  std::vector<int> m_tileIndices;
  void Initialize(float eta, float deta, float phi, float dphi);

};

bool inBox(float eta1, float eta2, float deta, float phi1, float phi2, float dphi);
bool withinRadius(float eta1, float eta2, float phi1, float phi2, float dR, bool acceptEqual=false);
float deltaPhi(float phi1, float phi2);

#endif
