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

#include <set>   
#include "GaudiKernel/ToolHandle.h"
#include <stdint.h>
#include "DataQualityTools/DataQualityFatherMonTool.h"
//#include "Particle/TrackParticle.h"
//EP #include "TrkVxEdmCnv/IVxCandidateXAODVertex.h"

#include "TrigDecisionTool/TrigDecisionTool.h"

#include "TMath.h"
#include <string>

class TProfile;

class DQTGlobalWZFinderTool: public DataQualityFatherMonTool
{

 public:
  
  DQTGlobalWZFinderTool(const std::string & type, const std::string & name, const IInterface* parent);

  ~DQTGlobalWZFinderTool();

  //StatusCode initialize();
    
  StatusCode bookHistograms();
  //StatusCode bookHistograms( bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun );
  StatusCode fillHistograms();
  StatusCode procHistograms( );
  //StatusCode procHistograms( bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun );
  StatusCode checkHists(bool fromFinalize);

  bool bookDQTGlobalWZFinderTool();

private:

      ToolHandle<Trig::TrigDecisionTool> m_trigDec;

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
      TH1F_LW *m_Kshort_mass_pipi;

      //Resonance Counters
      float minLumiBlock;
      float maxLumiBlock;
      float numBins;

      TH1F_LW *m_JPsiCounter_Mu;
      TH1F_LW *m_UpsilonCounter_Mu;
      TH1F_LW *m_ZBosonCounter_El;
      TH1F_LW *m_ZBosonCounter_Mu;
      TH1F_LW *m_KshortCounter_Pi;

      TH1F_LW *m_JPsiCounter_Mu_triggerAware;
      TH1F_LW *m_UpsilonCounter_Mu_triggerAware;
      TH1F_LW *m_ZBosonCounter_El_triggerAware;
      TH1F_LW *m_ZBosonCounter_Mu_triggerAware;
      TH1F_LW *m_KshortCounter_Pi_triggerAware;

      
      //Second component of counter array is for trigger aware counter
      int KshortCounterSBG[2];
      int KshortCounterBG[2];
      int KshortCounterSignal[2];

      int JPsiCounterSBG[2];
      int JPsiCounter2BG[2];
      int JPsiCounterSignal[2];

      int UpsilonCounterSBG[2];
      int UpsilonCounterBG[2];
      int UpsilonCounterSignal[2];

      int ZBosonCounterSBG_El[2];
      int ZBosonCounterSBG_Mu[2];

      //uint32_t lumiBlock;
      //uint32_t eventNumber;

      int lumiBlock; //remove _t
      int eventNumber; //remove _t


      TGraph *m_graph_Wenu;
      TGraph *m_graph_Wmunu;
      TGraph *m_graph_Zee_ops; 
      TGraph *m_graph_Zmumu_ops;       
      TGraph *m_graph_Zee_ss; 
      TGraph *m_graph_Zmumu_ss;       
      
      std::string m_electronContainerName;
      std::string m_egDetailContainerName;

      std::string m_VxPrimContainerName;
      std::string m_VxContainerName;

      std::string m_METName;
      std::string m_muonContainerName;
      std::string m_jetCollectionName;
      std::string m_tracksName;
      Float_t m_electronEtCut;
      Float_t m_metCut;
      Int_t m_doEtaCuts;
      Float_t m_zCutLow;
      Float_t m_zCutHigh;

      bool m_doTrigger;
      
      // to guard against endless messages 
      bool printedErrorEleContainer;
      bool printedErrorMuColl;
      bool printedErrorMet;
      bool printedErrorTrackContainer;

      //EP ToolHandle< Trk::IVxCandidateXAODVertex > m_xaodConverter;  // necessary to convert VxCandidate to xAOD::Vertex in case old API is used

};

#endif
