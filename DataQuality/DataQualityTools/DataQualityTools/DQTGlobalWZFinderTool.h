/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "StoreGate/ReadHandleKey.h"
#include <stdint.h>
#include "DataQualityTools/DataQualityFatherMonTool.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"

#include "TrigDecisionTool/TrigDecisionTool.h"

#include "TMath.h"
#include <string>

class TProfile;
class TH1F_LW;
class TH2F_LW;

namespace CP {
  class IMuonSelectionTool;
  class IIsolationSelectionTool;
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

      //TH1F_LW *m_Z_ee_mass_lb;
      //TH1F_LW *m_Z_mm_mass_lb;
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
      TH1F_LW *m_ZBosonCounter_El;
      TH1F_LW *m_ZBosonCounter_Mu;
      
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
      
      Float_t m_electronEtCut;
      Float_t m_muonPtCut;
      Float_t m_metCut;
      Float_t m_zCutLow;
      Float_t m_zCutHigh;
      bool m_doTrigger;
      ToolHandle<CP::IMuonSelectionTool> m_muonSelectionTool;
      ToolHandle<CP::IIsolationSelectionTool> m_isolationSelectionTool;

            
      std::vector<std::string> m_Jpsi_mm_trigger;
      std::vector<std::string> m_Z_mm_trigger;
      std::vector<std::string> m_Z_ee_trigger;

  SG::ReadHandleKey<xAOD::EventInfo> m_EventInfoKey
    { this, "EventInfoKey", "EventInfo", "" };
  SG::ReadHandleKey<xAOD::MissingETContainer> m_MissingETContainerKey
    { this, "MetName", "MET_Reference_AntiKt4EMTopo", "" };
  SG::ReadHandleKey<xAOD::ElectronContainer> m_ElectronContainerKey
    { this, "ElectronContainerName", "Electrons", "" };
  SG::ReadHandleKey<xAOD::MuonContainer> m_MuonContainerKey
    { this, "MuonContainerName", "Muons", "" };
  SG::ReadHandleKey<xAOD::VertexContainer> m_VertexContainerKey
    { "PrimaryVertices" };
  //  SG::ReadHandleKey<xAOD::JetContainer> m_JetContainerKey;
  
  
};

#endif
