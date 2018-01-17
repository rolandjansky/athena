/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBphysHypo/TrigEFBMuMuFex.h
 **
 **   Description: EF hypothesis algorithms for B0_s -> mu+ mu-  
 **                Currently a dummy algorithm
 **
 **   Author: J.Kirk
 **
 **   Created:   12.09.07
 **   Modified:     
 **
 **************************************************************************/ 

#ifndef TRIG_TrigEFBMuMuFex_H
#define TRIG_TrigEFBMuMuFex_H

// standard stuff
#include <string>
#include <map>
#include <cmath> 
// general athena stuff
//// trigger includes
#include "TrigInterfaces/ComboAlgo.h"
#include "xAODMuon/Muon.h"
#include "AthLinks/ElementLink.h"

// forward includes
class TrigBphysHelperUtilsTool;
class TrigTimer;

// class 
class TrigEFBMuMuFex: public HLT::ComboAlgo  {
  
  public:
    TrigEFBMuMuFex(const std::string & name, ISvcLocator* pSvcLocator);
    ~TrigEFBMuMuFex();
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltFinalize();     

    HLT::ErrorCode acceptInputs(HLT::TEConstVec& inputTE, bool& pass );
    HLT::ErrorCode hltExecute(HLT::TEConstVec& inputTE, HLT::TriggerElement* outputTE);
    
  private:
    
    void buildCombination(const xAOD::Muon *mu0, const xAOD::Muon *mu1); // does the combination and calls the fitter

    ToolHandle <TrigBphysHelperUtilsTool> m_bphysHelperTool;
  
    std::vector<xAOD::TrigBphys*> m_resultsHolder; // transient holder for results
    
    TrigTimer* m_BmmHypTot, *m_BmmHypVtx;
    
    
    unsigned int m_expectNumberOfInputTE;
    bool m_useRoIs;
    
    double m_massMuon;
    xAOD::Muon::TrackParticleType m_muonParticleType;
    
    bool m_oppositeCharge;
    bool m_sameCharge;
    float m_lowerMassCut;
    float m_upperMassCut;
    bool m_ApplyupperMassCut;
    std::string m_muonAlgo;
    std::string m_SAMuonLabel;
    // to set Accept-All mode: should be done with force-accept when possible
    bool m_acceptAll;
    bool m_noId; // use SA muon tracks in second roi
    bool m_useCombinedTP; // use ID-only or Combined muon info
    bool m_considerSameRoiCombinations; // look also at muon pairs within same roi
    
    //Monitored variables
    std::vector<int>   m_mon_Errors;
    std::vector<int>   m_mon_Acceptance;
    double m_mon_dEtaRoI;
    double m_mon_dPhiRoI;
    double m_mon_dEtaMuMu;
    double m_mon_dPhiMuMu;
    double m_mon_pTsum;
    double m_mon_mu1pT;
    double m_mon_mu2pT;
    double m_mon_mu1eta;
    double m_mon_mu2eta;
    double m_mon_mu1phi;
    double m_mon_mu2phi;
    double m_mon_MuMumass;
    double m_mon_BmassFit;
    double m_mon_Chi2;

    //Counters
    uint32_t m_lastEvent;
    uint32_t m_lastEventPassed;
    bool m_vtxpass, m_PassedBsMass;
    unsigned int m_countTotalEvents;
    unsigned int m_countTotalRoI;
    unsigned int m_countPassedEvents;
    unsigned int m_countPassedRoIs;
    unsigned int m_countPassedmumuPairsEv;
    unsigned int m_countPassedBsMassEv;
    unsigned int m_countPassedVtxFitEv;
    unsigned int m_countPassedmumuPairs2R;
    unsigned int m_countPassedBsMass2R;
    unsigned int m_countPassedVtxFit2R;

    
};


#endif
