/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigT1TGC_TGCSLSelectorOut_h
#define TrigT1TGC_TGCSLSelectorOut_h

namespace LVL1TGCTrigger {

class TGCSLSelectorOut
{
 public:
  static constexpr int s_NCandidateInSLSelector = 2;

 public:
  void reset();

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
  int  m_ptLevel[s_NCandidateInSLSelector];
  int  m_r[s_NCandidateInSLSelector];
  int  m_phi[s_NCandidateInSLSelector];
  int  m_dr[s_NCandidateInSLSelector];
  int  m_dphi[s_NCandidateInSLSelector];
  bool m_iVeto[s_NCandidateInSLSelector];
};

} //end of namespace bracket

#endif // TGCSLSelectorOut_hh
