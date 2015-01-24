/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCSLSelectorOut_hh
#define TGCSLSelectorOut_hh

namespace LVL1TGCTrigger {

const int NCandidateInSLSelector = 2;

class TGCSLSelectorOut {
public:
  void setPtLevel(int order, int ptLevel);
  void setR(int order, int rIn) { r[order]=rIn;};
  void setPhi(int order, int phiIn) {phi[order]=phiIn;};
  void setDR(int order, int drIn) { dr[order]=drIn;};
  void setDPhi(int order, int dphiIn) {dphi[order]=dphiIn;};
  void setInnerVeto(int order, bool vetoIn) {iVeto[order]=vetoIn;};

  int  getPtLevel(int order) const { return ptLevel[order];}; 
  int  getR(int order) const { return r[order];}; 
  int  getPhi(int order) const { return phi[order];}; 
  int  getDR(int order) const { return dr[order];}; 
  int  getDPhi(int order) const { return dphi[order];}; 
  bool getInnerVeto(int order) const { return iVeto[order];}; 
  int  getNCandidate() const { return nCandidate;};

  void print() const;

  TGCSLSelectorOut();
  ~TGCSLSelectorOut(){};

private:
  int  nCandidate;
  int  ptLevel[NCandidateInSLSelector];
  int  r[NCandidateInSLSelector];
  int  phi[NCandidateInSLSelector];
  int  dr[NCandidateInSLSelector];
  int  dphi[NCandidateInSLSelector];
  bool iVeto[NCandidateInSLSelector];
};

} //end of namespace bracket

#endif // TGCSLSelectorOut_hh
