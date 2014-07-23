/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***********************************************************************

SUSY reconstruction: make Meff for 0,1 leptons or taus and dilepton
distribution. Plots made in SUSYSusyRec should be for susy only. All general plots should be made with their own tools.

***********************************************************************/

#ifndef SUSYPLOT_SUSYSUSYREC_H
#define SUSYPLOT_SUSYSUSYREC_H

#include <CLHEP/Units/SystemOfUnits.h>
#include <AnalysisTools/AnalysisTools.h>

// #include <ITrackToVertex/ITrackToVertex.h>
// #include <GaudiKernel/ToolHandle.h>
#include <StoreGate/StoreGateSvc.h>

#include "rel17_SUSYPlotTool.h"
// #include "rel17_checkOQ.h"
#include "rel17_EnergyRescaler.h"
#include "rel17_egammaSFclass.h"
#include "rel17_SmearingClass.h"
#include "rel17_StacoCBScaleEffFactors.h"
// #include "rel17_StacoTightScaleEffFactors.h"

#include <xAODEgamma/ElectronContainer.h>
#include <xAODEgamma/PhotonContainer.h>
#include <xAODJet/JetContainer.h>
#include <xAODMuon/MuonContainer.h>
#include <xAODTau/TauJetContainer.h>
#include <xAODMissingET/MissingETContainer.h>
#include <xAODCaloEvent/CaloCluster.h>
#include <xAODTracking/VertexContainer.h>
// #include <xAODTruth/TruthParticleContainer.h>
// #include <xAODTruth/TruthVertexContainer.h>
#include <xAODEventInfo/xAODEventInfo/EventInfo.h>

#include <string>
#include <vector>

#include <TH1.h>
#include <TH2.h>

using namespace Analysis;
using namespace Rec;

class IAnalysisTools;

class SUSYSusyRec : public SUSYPlotTool {

 public:
  
  SUSYSusyRec(const std::string& type,
                const std::string& name,
	      const IInterface* parent);
   virtual ~SUSYSusyRec();
   
   virtual StatusCode initialize();
   virtual StatusCode takeAction();
   
   double CalcTranSphericity(double sumpx2,double sumpy2,double sumpxpy);
   
 private:
   
   /** get a handle to the tool helper */
   ToolHandle<AnalysisTools> m_analysisTools;
   
   //for muon track variables
   StoreGateSvc* m_storeGate;
//    ToolHandle<Reco::ITrackToVertex> m_trackToVertexTool;
   


    // Class used to detect dead OTX
    //egammaOQ m_egammaOQ; // Obsolete for 2011
    // Class used to rescale electron energy in data and smear electron energy in MC
    EnergyRescaler  m_eRescale;   
    // Class used to smear muon energy in MC
    SmearingClass  m_mcp_smear;
    // Class used to correct MC electron reco efficiencies
    egammaSFclass  m_egammaSFclass;
    // Class used to rescale MC muon reco efficiencies
    Analysis::StacoCBScaleEffFactors    m_StacoCBSCF;
//     Analysis::StacoTightScaleEffFactors m_StacoTightSCF;
   
   /// jobOption parameters
   
   std::string m_electronName;
   std::string m_muonName;
   std::string m_tauName;
   std::string m_jetName;
   std::string m_metName;
   std::string m_truthName;

   std::string m_SUSYjetName;
   std::string m_SUSYelectronName;
   std::string m_SUSYmetName;
   
   std::string m_SUSYmetmuonName;
   std::string m_SUSYmetjetName;
   
   std::string m_c_histBaseDirectory;
   std::string m_c_metContainerName;
   std::string m_c_eventInfoName;
   std::string m_c_pvContainerName;

   bool m_isRelease15;
   
   double m_ptlCut;
   double m_ptl2Cut;
   double m_etalCut;
   double m_etisolCut;
   double m_metCut;
   double m_etajCut;
   double m_ptjCut;
   double m_ptj0Cut;
   double m_ptj1Cut;
   double m_MeffCut;
   double m_mTCut;
   double m_STCut;
   
   std::vector<std::unique_ptr<xAOD::Electron> > calibElectrons;
   std::vector<std::unique_ptr<xAOD::Muon> > calibMuons;
   std::vector<const xAOD::Electron*> PreElectrons;
   std::vector<const xAOD::Electron*> PreSignalElectrons;
   std::vector<const xAOD::Electron*> goodElectrons;
   std::vector<const xAOD::Electron*> goodSignalElectrons;   
   std::vector<const xAOD::Muon*> PreMuons_forMET;
   std::vector<const xAOD::Muon*> PreMuons;
   std::vector<const xAOD::Muon*> PreSignalMuons;
   std::vector<const xAOD::Muon*> goodMuons;
   std::vector<const xAOD::Muon*> goodSignalMuons;
   std::vector<const xAOD::TauJet*> goodTaus;
   std::vector<const xAOD::Jet*> goodJets;
//    std::vector<const TrackParticle*> goodTracks;
//    const xAOD::JetContainer* goodJetsC;
   
   /// histograms
   TH1F* m_h_vx_n;
   TH1F* m_h_averageIntPerXing_n;
   TH1F* m_h_actualIntPerXing_n;
   TH2F* m_h_averageIntPerXing_vs_vx;
   TH1F* m_h_pre_Jet_n;
   TH1F* m_h_pre_Jet50_n;
   TH1F* m_h_pre_pt;
   TH1F* m_h_pre_ptlo;
   TH1F* m_h_pre_pt0;
   TH1F* m_h_pre_pt1;
   TH1F* m_h_pre_pt2;
   TH1F* m_h_pre_pt3;
   TH1F* m_h_pre_ptHardLep;
   TH1F* m_h_pre_etmiss;
   TH1F* m_h_pre_METangle0;
   TH1F* m_h_pre_METangle1;
   TH1F* m_h_pre_METangle2;
   TH1F* m_h_pre_METangle3;
   TH1F* m_h_pre_METangleLept;
   TH1F* m_h_pre_mT;
   TH1F* m_h_pre_ST;
   TH1F* m_h_pre_Meff;

   TH1F* m_h_pre_Ne;
   TH1F* m_h_pre_Nm;
   TH1F* m_h_pre_Nt;
   TH1F* m_h_pre_pte;
   TH1F* m_h_pre_ptm;
   TH1F* m_h_pre_ptt;
   TH1F* m_h_pre_etae;
   TH1F* m_h_pre_etam;
   TH1F* m_h_pre_etat;
 
   TH1F* m_h_Trigger_Flow;
   TH1F* m_h_susy_0lep_CutFlow;
   TH1F* m_h_susy_1muon_CutFlow;
   TH1F* m_h_susy_1el_CutFlow;
   TH1F* m_h_susy_2lep_CutFlow;
   TH1F* m_h_jet_CutFlow;
   TH1F* m_h_susy_pt0;
   TH1F* m_h_susy_pt1;
   TH1F* m_h_susy_etmiss;
   TH1F* m_h_susy_ST;
   TH1F* m_h_susy_Meff;
   TH1F* m_h_susy_Jet_n;

   TH1F* m_h_susy_ptHardLep;
   TH1F* m_h_susy_mT;
   TH1F* m_h_susy_etmiss1ell;
   TH1F* m_h_susy_etmiss1mu;
   TH1F* m_h_susy_etmiss1lep;
   TH1F* m_h_susy_Meff1ell;
   TH1F* m_h_susy_Meff1mu;
   TH1F* m_h_susy_Meff1lep;

//    TH1F* m_h_susy_mee;
//    TH1F* m_h_susy_mmm;
//    TH1F* m_h_susy_mem;
   TH1F* m_h_susy_mll;
   TH1F* m_h_susy_mllq;
   TH1F* m_h_susy_tllq;
   TH1F* m_h_susy_mlq1;
   TH1F* m_h_susy_mlq2;

//    TH1F* m_h_susy_mtautauos;
//    TH1F* m_h_susy_mtautauss;
   
  TH1D* h_elecPt;

  TH1F* h_select_el;
  TH1F* h_select_mu;
  
};

#endif

