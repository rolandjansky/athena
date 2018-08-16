/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     DQTGlobalWZFinderTool.h
// PACKAGE:  DataQualityTools
//
// AUTHORS:   Jahred Adelman (jahred.adelman@cern.ch)
//            Simon Viel (svielcern.ch)
//            Koos van Nieuwkoop (jvannieu@cern.ch)
//	     
//
// ********************************************************************
#ifndef DQTGlobalWZFinderTool_H
#define DQTGlobalWZFinderTool_H

#include "GaudiKernel/ToolHandle.h"
#include <stdint.h>
#include "DataQualityTools/DataQualityFatherMonTool.h"

#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigEgammaMatchingTool/ITrigEgammaMatchingTool.h"
#include "xAODEventInfo/EventInfo.h"

#include "TMath.h"
#include <string>
#include <iostream>


class TProfile;
class TH1F_LW;
class TH2F_LW;

namespace CP {
  class IMuonSelectionTool;
  class IIsolationSelectionTool;
}

namespace Trig {
  class ITrigMuonMatching;
  class ITrigEgammaMatchingTool;
}

class DQTGlobalWZFinderTool: public DataQualityFatherMonTool
{

 public:
  
  DQTGlobalWZFinderTool(const std::string & type, const std::string & name, const IInterface* parent);

  ~DQTGlobalWZFinderTool();

  StatusCode initialize();
    
  virtual StatusCode bookHistograms();
  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms( );
  virtual StatusCode checkHists(bool fromFinalize);

  bool bookDQTGlobalWZFinderTool();

private:
  void doMuonTriggerTP(const xAOD::Muon* , const xAOD::Muon*);
  void doMuonTruthEff(std::vector<const xAOD::Muon*>&);
  void doMuonLooseTP(std::vector<const xAOD::Muon*>& goodmuonsZ, const xAOD::Vertex* pVtx);

//----- Electron START -----//
  void doEleTriggerTP(const xAOD::Electron*,const xAOD::Electron*);
  void doEleTP(const xAOD::Electron*, const xAOD::Electron*, const xAOD::Vertex*, const xAOD::EventInfo*, bool);
  bool goodElectrons(const xAOD::EventInfo*, const xAOD::Electron*, const xAOD::Vertex*, bool); 

  ToolHandle<Trig::ITrigEgammaMatchingTool> m_elTrigMatchTool;

  TH1F_LW *m_eltrigtp_matches;

  TH1F_LW *m_ele_tight_bad_os;
  TH1F_LW *m_ele_tight_bad_ss;
  TH1F_LW *m_ele_tight_good_os;
  TH1F_LW *m_ele_tight_good_ss;

//----- Electron END ------//

      TH1F_LW *m_W_mt_ele;
      TH1F_LW *m_W_mt_mu;
      TH2F_LW *m_W_pt_v_met_ele;
      TH2F_LW *m_W_pt_v_met_mu;

      TH1F_LW *m_metHist;
      TH1F_LW *m_metPhiHist;

      TH1F_LW *m_Z_mass_opsele;
      TH1F_LW *m_Z_mass_opsmu;
      TH1F_LW *m_JPsi_mass_opsmu;
      TH1F_LW *m_Upsilon_mass_opsmu;
      TH1F_LW *m_Z_mass_ssele;
      TH1F_LW *m_Z_mass_ssmu;
      TH1F_LW *m_Z_Q_ele;
      TH1F_LW *m_Z_Q_mu;

      TProfile *m_livetime_lb;
      TProfile *m_lblength_lb;
      TProfile *m_mu_lb;
      TProfile *m_Z_ee_trig_ps;
      TProfile *m_Z_mm_trig_ps;
      
      TH1F_LW *m_muon_Pt;
      TH1F_LW *m_muon_Eta;
      TH1F_LW *m_ele_Et;
      TH1F_LW *m_ele_Eta;

      //Resonance Counters
      float m_minLumiBlock;
      float m_maxLumiBlock;
      float m_numBins;

      TH1F_LW *m_JPsiCounter_Mu;
      TH1F_LW *m_UpsilonCounter_Mu;
      TH1F *m_ZBosonCounter_El;
      TH1F *m_ZBosonCounter_Mu;
      TH1F *m_ZBosonCounter_Mu_CMS;


      //Trigger T&P
      TH1F_LW *m_mutrigtp_matches;
      TH1F_LW *m_mutrigtp_matches_CMS;


      //Reco T&P
      TH1F_LW *m_muloosetp_match_os;
      TH1F_LW *m_muloosetp_match_ss;
      TH1F_LW *m_muloosetp_nomatch_os;
      TH1F_LW *m_muloosetp_nomatch_ss;

      TH1F_LW *m_muloosetp_match_os_CMS;
      TH1F_LW *m_muloosetp_match_ss_CMS;
      TH1F_LW *m_muloosetp_nomatch_os_CMS;
      TH1F_LW *m_muloosetp_nomatch_ss_CMS;


      // MC truth
      TH1F_LW *m_mcmatch;
      
      //Second component of counter array is for trigger aware counter
      int m_JPsiCounterSBG[2];
      int m_JPsiCounter2BG[2];
      int m_JPsiCounterSignal[2];

      int m_UpsilonCounterSBG[2];
      int m_UpsilonCounterBG[2];
      int m_UpsilonCounterSignal[2];

      int m_ZBosonCounterSBG_El[2];
      int m_ZBosonCounterSBG_Mu[2];

      //uint32_t lumiBlock;
      //uint32_t eventNumber;

      int m_this_lb; //remove _t
      int m_eventNumber; //remove _t
      Float_t m_evtWeight;
      
      std::string m_electronContainerName;
      std::string m_egDetailContainerName;

      std::string m_VxPrimContainerName;
      std::string m_VxContainerName;

      std::string m_METName;
      std::string m_muonContainerName;
      std::string m_jetCollectionName;
      std::string m_tracksName;
      Float_t m_electronEtCut;
      Float_t m_muonPtCut;
      Float_t m_muonPtCut_CMS;

      Float_t m_metCut;
      Float_t m_zCutLow;
      Float_t m_zCutHigh;
      Float_t m_muonMaxEta;
      bool m_doTrigger;
      ToolHandle<CP::IMuonSelectionTool> m_muonSelectionTool;
      ToolHandle<CP::IIsolationSelectionTool> m_isolationSelectionTool;
      ToolHandle<Trig::ITrigMuonMatching> m_muTrigMatchTool;

      bool m_useOwnMuonSelection;
            
      // to guard against endless messages 
      bool m_printedErrorEleContainer;
      bool m_printedErrorMuColl;
      bool m_printedErrorMet;
      bool m_printedErrorTrackContainer;

      std::vector<std::string> m_Jpsi_mm_trigger;
      std::vector<std::string> m_Z_mm_trigger;
      std::vector<std::string> m_Z_ee_trigger;

};

#endif
