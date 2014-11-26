/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FAKEMISSINGET_TOOL_H 
#define FAKEMISSINGET_TOOL_H 

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ObjectVector.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ITHistSvc.h"

#include "MissingETPerformance/MissingETData.h"
#include "MissingETPerformance/MissingETMuonData.h"
#include "MissingETPerformance/JetVariablesTool.h"

#include <string>

#include "TH1.h"
#include "TH2.h"

static const InterfaceID IID_FakeMissingETTool("FakeMissingETTool", 1, 0);

class FakeMissingETTool : public AthAlgTool {

public:

  FakeMissingETTool(const std::string& type, const std::string& name,
	           const IInterface* parent);

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { return IID_FakeMissingETTool; };

  StatusCode CBNT_initialize();

  /** Overriding initialize and finalize */
  virtual StatusCode initialize();
  /** finalization */
  virtual StatusCode finalize();
  virtual StatusCode execute(MissingETData *data, MissingETMuonData *muondata);

  inline void SetFolderName(const std::string& folderName) { m_folderName=folderName; }

protected:

   /** Standard destructor */
   virtual ~FakeMissingETTool( );

 private:
  std::string m_folderName;

  StatusCode fakeMissingET(MissingETData *data, MissingETMuonData *muondata);

  /** a handle on the Hist/TTree registration service */
  ITHistSvc * m_thistSvc;

  ToolHandle<JetVariablesTool> _jetVariables;

  //job options variables
  float _minJetEtCut;
  float _fakeFullCut;
  float _fakeMuonCut;

  unsigned int _nbinsMET;
  float _lowerbinMET;
  float _upperbinMET;

  unsigned int _nbinsMETxy;
  float _lowerbinMETxy;
  float _upperbinMETxy;

  // Number of events with fake met from muon or jet above 
  // 50, 100, 200 GeV fake MET.
  //int npas_mlke_50;
  int npas_jlke_50;
  //int npas_mlke_100;
  int npas_jlke_100;
  //int npas_mlke_200;
  int npas_jlke_200;

  int fakemu_gt_60;
  int fakemu_gt_90;
  int fakemu_gt_120;
  int fakemu_gt_150;

  int fakefull_gt_60;
  int fakefull_gt_90;
  int fakefull_gt_120;
  int fakefull_gt_150;
  //int fakefull_gt_90_fakemu_gt_halffull;

  int countbadmuonevents;
  int numMissedMuonEvents;
  int failedcutsevent;
  int misreconevent;
  int fakeEvent;
  int noCat;
  int skippedevents;
  int skippedevents2;

  //histograms
  TH1D *h1_lead_dphicut_met_neut;
  TH1D *h1_dphicut_leadjet_met_neut_mc;
  TH1D *h1_dphi_leadjet_etmiss;
  TH1D *h1_dphi_secondjet_etmiss;

  TH1D *h1_met_neut;                       // MET_TruthNoInt
  TH1D *h1_met_fake;                       // MET_Final(x,y) - MET_TruthNoInt(x,y)
  //TH1D *h1_og27_met_fake;

  TH1D *h1_met_fake_with_mcmu;             //MET fake with truth muons

  TH1D *h1_lead_dphicut_met_fake_mc;
  TH1D *h1_lead_dphicut_met_fake;

  TH1D *h1_dphicut_met_fake_with_mcmu;
  TH1D *h1_dphicut_met_fake;
  TH1D *h1_dphicut_met_neut;
  TH1D *h1_dphi_worstjet_etmiss;
  TH1D *h1_fake_dphi_leadjet_etmiss;
  TH2D *h1_dphi_leadjet_etmiss_V_met_fake;
  TH1D *h1_dphi_leadjet_etmiss_mc;
  TH2D *h1_dphi_leadjet_etmiss_mc_V_met_neut;

  TH1D *h1_fake_dphi_secondjet_etmiss;
  TH2D *h1_dphi_secondjet_etmiss_V_met_fake;

  TH1D *h1_dphiseccut_met_fake_with_mcmu;
  TH1D *h1_dphiseccut_met_fake;
  TH1D *h1_dphiseccut_met_neut;
  TH1D *h1_dphi_sec_worstjet_etmiss;

  TH1D *h1_met_fake_muon;                  // Fake MET from Muon
  TH1D *h1_met_fake_muon_xy;                
  //
  TH1D *h1_met_fake_jets;                  // Fake MET from Jets
  TH1D *h1_met_fake_jets_xy; 
  TH1D *h1_met_fake_jets_incrack;          // Fake MET with Jet pointing to crack
  TH1D *h1_met_fake_jets_incrack_xy;   
  TH1D *h1_met_fake_jets_outcrack;         // Fake MET with no jet pointing to crack
  TH1D *h1_met_fake_jets_outcrack_xy;  
  //
  TH2D *h2_met_fakemuon_vs_fakefull;       // 2d scatter plot of fake MET (xy) vs fake MET (xy) from muon
  TH2D *h2_met_fakemuon_vs_fakefull_muon;  // selecting only muon contribution
  TH2D *h2_met_fakemuon_vs_fakefull_jets;  // selecting only jets contribution
  //

  TH1D *h1_n_reco_jets;
  TH1D *h1_reco_jets_et;

  TH1D *h1_n_reco_jets_fakemet;
  TH1D *h1_reco_jets_et_fakemet;

  //histograms for leading truth jet and reco jet
  TH1D *h1_leading_reco_jet_Et;            // leading reco jet Et
  TH1D *h1_leading_reco_jet_eta;           // leading reco jet eta
  TH1D *h1_leading_reco_jet_abseta;           // leading reco jet abseta
  TH2D *h2_leading_reco_jet_abseta_V_fakemet;

  TH1D *h1_leading_truth_jet_Et;            // leading reco jet Et
  TH1D *h1_leading_truth_jet_eta;           // leading reco jet eta
  TH1D *h1_leading_truth_jet_abseta;           // leading reco jet abseta

  TH1D *h1_second_reco_jet_Et;            // second reco jet Et
  TH1D *h1_second_reco_jet_eta;           // second reco jet eta
  TH1D *h1_second_reco_jet_abseta;           // second reco jet abseta
  TH2D *h2_second_reco_jet_abseta_V_fakemet;

  //these histograms are for events with large total MET mismeasurement and low muon MET mismeasurement
  TH1D *h1_met_worst_jet_maxdEt;           // error on Et measurement of worst measured jet
  TH1D *h1_met_worst_jet_eta;              // eta of worst measured jet
  TH1D *h1_met_worst_jet_abseta;           // abseta of worst measured jet
  TH1D *h1_met_worst_jet_phi;              // phi of worst measured jet
  TH2D *h2_met_worst_jet_eta_vs_phi;       // eta vs phi of worst measured jet
  TH2D *h2_met_worst_jet_dMET_vs_eta;      // total MET error vs. eta of worst measured jet
  //
  TH1D *h1_met_secondworst_jet_eta;        // eta of second-worst measured jet
  TH1D *h1_met_secondworst_jet_abseta;     // abseta of second-worst measured jet
  TH1D *h1_met_secondworst_jet_phi;        // phiof second-worst measured jet
  TH2D *h2_met_secondworst_jet_eta_vs_phi; // eta vs phi of second-worst measured jet
  //
  TH1D *h1_met_worst_jet_div_met;          //measure contribution of worst measured jet to fake MET
  TH1D *h1_met_secondworst_jet_div_met;    //measure contribution of second-worst measured jet to fake MET
  TH1D *h1_met_worst_jet_div_met_xy;       //measure contribution of worst measured jet to fake MET (x- and y- components)
  TH1D *h1_met_secondworst_jet_div_met_xy; //measure contribution of second-worst measured jet to fake MET (x- and y- components)

  //event em frac plots
  TH2F *h2_MET_Topo_vs_evt_emfrac;
  TH2F *h2_MET_RefFinal_vs_evt_emfrac;

  TH1F *h1_emfcut_MET_Topo_met;
  TH1F *h1_emfcut_MET_Topo_x;
  TH1F *h1_emfcut_MET_Topo_y;
  TH1F *h1_emfcut_MET_Topo_sumet;

  TH1F *h1_emfcut_MET_RefFinal_met;
  TH1F *h1_emfcut_MET_RefFinal_x;
  TH1F *h1_emfcut_MET_RefFinal_y;
  TH1F *h1_emfcut_MET_RefFinal_sumet;

  //plots vs n jets
  TH2F *h2_MET_Topo_met_vs_njets;
  TH2F *h2_MET_Topo_x_vs_njets;
  TH2F *h2_MET_Topo_y_vs_njets;
  TH2F *h2_MET_Topo_sumet_vs_njets;

  TH2F *h2_MET_RefFinal_met_vs_njets;
  TH2F *h2_MET_RefFinal_x_vs_njets;
  TH2F *h2_MET_RefFinal_y_vs_njets;
  TH2F *h2_MET_RefFinal_sumet_vs_njets;

  //missed muons
  TH1D *h1_mag_muon_diff;                  //magnitude of difference between reco and truth muons
  TH2D *h2_mag_muon_diff_V_truth_pt;       //magnitude of difference between reco and truth muons vs truth muon pt

  TH1D *h1_missedmuonpt;                   // pt spectrum of missed muons
  TH1D *h1_eta_muons_missed_events;        // eta of missed muons
  TH1D *h1_phi_muons_missed_events;        // phi of missed muons
  TH2D *h2_eta_v_phi_muons_missed_events;  // eta vs. phi of missed muons

  TH1D *h1_bad_muon_reco_algo_fake_muon;      // fake muon MET distribution in events in missedevent || failedcuts events
  TH1D *h1_bad_muon_reco_algo_fake_full;      // fake full MET distribution in events in missedevent || failedcuts events
  TH1D *h1_bad_muon_reco_mis_fake_muon;       // fake muon MET distribution in events in misrecon || fakeevent events
  TH1D *h1_bad_muon_reco_mis_fake_full;       // fake full MET distribution in events in misrecon || fakeevent events
};

#endif // FAKEMISSINGET_TOOL_H 
