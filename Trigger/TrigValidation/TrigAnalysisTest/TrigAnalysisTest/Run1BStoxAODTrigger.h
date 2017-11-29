/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Test that fills histograms for some of HLT xAOD variables from Run 1
// and stores them to a root file, for the comparison with the AOD reference
//
#ifndef TRIGANALYSISTEST_RUN1BSTOXAODTRIGGER_H
#define TRIGANALYSISTEST_RUN1BSTOXAODTRIGGER_H

#include "TrigAnalysisTest/ITest.h"

//forward declarations
class TH1;
class TFile;

namespace TrigAnalysisTest {

  class Run1BStoxAODTrigger : public ITest {
  public:
    Run1BStoxAODTrigger();

    void processEvent(Trig::TrigDecisionTool &trigDecTool);
    int finalize();

  private:
    unsigned int m_passed_l1;
    unsigned int m_passed_hlt;
    //bool m_first_call;
    std::map<std::string,int> m_trigger_counts;

    TFile *m_histFile;
    // histograms
    // count all fired triggers
    TH1* m_all_triggers;
    // bphys
    TH1* m_HLT_EFBMuMuXFex_eta;
    TH1* m_HLT_EFBMuMuXFex_mass;
    // taus
    TH1* m_HLT_TrigTauJet;
    // muons
    TH1* m_HLT_MuonEF_NSegments;
    TH1* m_HLT_MuonEF_pt;
    // MET
    TH1* m_HLT_TrigEFMissingET;
    TH1* m_HLT_TrigEFMissingET_FEB;
    TH1* m_HLT_TrigEFMissingET_topocl;

  };

}

#endif
