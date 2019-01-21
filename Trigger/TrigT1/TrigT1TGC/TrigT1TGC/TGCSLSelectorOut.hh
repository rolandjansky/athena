/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCSLSelectorOut_hh
#define TGCSLSelectorOut_hh

namespace LVL1TGCTrigger {

const int NCandidateInSLSelector = 2;

class TGCSLSelectorOut {
public:
  void setPtLevel(int order, int ptLevel);
  void setR(int order, int rIn) { m_r[order]=rIn;};
  void setPhi(int order, int phiIn) {m_phi[order]=phiIn;};
  void setDR(int order, int drIn) { m_dr[order]=drIn;};
  void setDPhi(int order, int dphiIn) {m_dphi[order]=dphiIn;};
  void setInnerVeto(int order, bool vetoIn) {m_iVeto[order]=vetoIn;};

  int  getPtLevel(int order) const { return m_ptLevel[order];}; 
  int  getR(int order) const { return m_r[order];}; 
  int  getPhi(int order) const { return m_phi[order];}; 
  int  getDR(int order) const { return m_dr[order];}; 
  int  getDPhi(int order) const { return m_dphi[order];}; 
  bool getInnerVeto(int order) const { return m_iVeto[order];}; 
  int  getNCandidate() const { return m_nCandidate;};

  void print() const;

  TGCSLSelectorOut();
  ~TGCSLSelectorOut(){};

private:
  int  m_nCandidate;
  int  m_ptLevel[NCandidateInSLSelector];
  int  m_r[NCandidateInSLSelector];
  int  m_phi[NCandidateInSLSelector];
  int  m_dr[NCandidateInSLSelector];
  int  m_dphi[NCandidateInSLSelector];
  bool m_iVeto[NCandidateInSLSelector];
};

} //end of namespace bracket

#endif // TGCSLSelectorOut_hh
