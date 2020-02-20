/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBphysHypo/TrigEFMultiMuFex.h
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

#ifndef TRIG_TrigEFMultiMuCombo_H
#define TRIG_TrigEFMultiMuCombo_H

// standard stuff
#include <string>

#include "TrigInterfaces/ComboAlgo.h"
#include "xAODMuon/Muon.h"
#include "xAODTrigBphys/TrigBphysContainer.h"

class TrigBphysHelperUtilsTool;
class TrigTimer;

class TrigEFMultiMuFex: public HLT::ComboAlgo  {
  
  public:
    TrigEFMultiMuFex(const std::string & name, ISvcLocator* pSvcLocator);
    ~TrigEFMultiMuFex();
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltFinalize();     

    HLT::ErrorCode acceptInputs(HLT::TEConstVec& inputTE, bool& pass );
    HLT::ErrorCode hltExecute(HLT::TEConstVec& inputTE, HLT::TriggerElement* outputTE);
    
  private:

    void processTriMuon(HLT::TEConstVec& inputTE, xAOD::TrigBphysContainer & outputContainer);
    void buildDiMu (const std::vector<const xAOD::Muon*> &muons, xAOD::TrigBphysContainer & outputContainer);
    void buildTriMu(const std::vector<const xAOD::Muon*> &muons, xAOD::TrigBphysContainer & outputContainer);
    
    ToolHandle <TrigBphysHelperUtilsTool> m_bphysHelperTool;
    
    
    TrigTimer* m_BmmHypTot, *m_BmmHypVtx;


    unsigned int  m_expectNumberOfInputTE;

    // Mass window cuts
    bool m_oppositeCharge;
    int  m_NMassMuon;
    float m_lowerMassCut;
    float m_upperMassCut;
    bool m_ApplyupperMassCut;
  bool m_checkNinputTE;
    std::string m_muonAlgo;
    // to set Accept-All mode: should be done with force-accept when possible
    bool m_acceptAll;

    
  //Counters
  uint32_t m_lastEvent;
  uint32_t m_lastEventPassed;
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

    
  //Monitored variables 
  std::vector<int>   m_mon_Errors;
  std::vector<int>   m_mon_Acceptance;
  std::vector<float> m_mon_mu1pT;
  std::vector<float> m_mon_mu2pT;
  std::vector<float> m_mon_MuMumass;
  std::vector<float> m_mon_FitMass;
  std::vector<float> m_mon_Chi2;
    
    const double m_massMuon;
};


#endif
