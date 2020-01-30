/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TGCTrackSelectorOut_hh
#define TGCTrackSelectorOut_hh


namespace LVL1TGCTrigger {


  class TGCTrackSelectorOut {
  public:


    void setPtLevel(int order, int ptLevel);
    void setR(int order, int rIn) { m_r[order]=rIn;};
    void setPhi(int order, int phiIn) {m_phi[order]=phiIn;};
    void setDR(int order, int drIn) { m_dr[order]=drIn;};
    void setDPhi(int order, int dphiIn) {m_dphi[order]=dphiIn;};
    void setInnerVeto(int order, bool vetoIn) {m_iVeto[order]=vetoIn;};
    void setCharge(int order, int chargeIn) {m_charge[order]=chargeIn;};
    void setCoincidenceType(int order, int CoincidenceTypeIn) {m_coincidenceTypeFlag[order]=CoincidenceTypeIn;};
    void setGoodMFFlag(int order, bool goodMFIn) {m_goodMFFlag[order]=goodMFIn;};
    void setInnerCoincidenceFlag(int order, bool InnerCoincidenceFlagIn) {m_innerCoincidenceFlag[order]=InnerCoincidenceFlagIn;};
    void setNCandidate(int nCandidateIn) { m_nCandidate=nCandidateIn;};

    int  getPtLevel(int order) const { return m_ptLevel[order];}; 
    int  getR(int order) const { return m_r[order];}; 
    int  getPhi(int order) const { return m_phi[order];}; 
    int  getDR(int order) const { return m_dr[order];}; 
    int  getDPhi(int order) const { return m_dphi[order];}; 
    bool getInnerVeto(int order) const { return m_iVeto[order];}; 
    int  getCharge(int order) const { return m_charge[order];}; 
    int  getCoincidenceType(int order) const { return m_coincidenceTypeFlag[order];}; 
    int  getGoodMFFlag(int order) const { return m_goodMFFlag[order];}; 
    int  getInnerCoincidenceFlag(int order) const { return m_innerCoincidenceFlag[order];}; 
    int  getNCandidate() const { return m_nCandidate;};

    enum {NCandidateInTrackSelector = 4};

    void print() const;
    void reset();
    
     TGCTrackSelectorOut();
    ~ TGCTrackSelectorOut();

  private:
    
    int  m_nCandidate;
    int  m_ptLevel[NCandidateInTrackSelector];
    int  m_r[NCandidateInTrackSelector];
    int  m_phi[NCandidateInTrackSelector];
    int  m_dr[NCandidateInTrackSelector];
    int  m_dphi[NCandidateInTrackSelector];
    bool m_iVeto[NCandidateInTrackSelector];
    int  m_charge[NCandidateInTrackSelector];
    bool m_coincidenceTypeFlag[NCandidateInTrackSelector];
    bool m_goodMFFlag[NCandidateInTrackSelector];
    bool m_innerCoincidenceFlag[NCandidateInTrackSelector];
    
  };

} //end of namespace bracket


#endif

